/********************************************************************/
/*


				        Power Switch

				        Sonar0x11

				 -------------------------
				/                         \
			       /		           \
			      /			            \
			M3   /			             \ M2
		       INT0 /			              \INT1
			   /			               \
			  /			                \
			 /			                 \
			 \			                 /
			  \			                /
		  	   \			               /
			    \			              /
		  Sonar0x12  \		  	             / Sonar0x13
			      \		                    /
			       \	                   /
				--------------------------
					    M1

 */

#define _NAMIKI_MOTOR

#include <MotorWheel.h>
#include <SONAR.h>

/******************************************/
// SONAR

SONAR sonar11(0x11),sonar12(0x12),sonar13(0x13);

unsigned short distBuf[3];
void sonarsUpdate() {
    static unsigned char sonarCurr=1;
    if(sonarCurr==3) sonarCurr=1;
    else ++sonarCurr;
    if(sonarCurr==1) {        
        distBuf[1]=sonar12.getDist();        
        sonar12.trigger(); 
        sonar12.showDat();       
    } else if(sonarCurr==2) {
        distBuf[2]=sonar13.getDist();
        sonar13.trigger();
        sonar13.showDat();
    } else {
        distBuf[0]=sonar11.getDist();
        sonar11.trigger();
        sonar11.showDat();
    }
}

/*********************************************/

/*******************************************/
// Motors

irqISR(irq1,isr1);
MotorWheel wheel1(9,8,6,7,&irq1);        // Pin9:PWM, Pin8:DIR, Pin6:PhaseA, Pin7:PhaseB

irqISR(irq2,isr2);
MotorWheel wheel2(10,11,14,15,&irq2);    // Pin10:PWM, Pin11:DIR, Pin14:PhaseA, Pin15:PhaseB

irqISR(irq3,isr3);
MotorWheel wheel3(3,2,4,5,&irq3);        // Pin3:PWM, Pin2:DIR, Pin4:PhaseA, Pin5:PhaseB
/******************************************/

/*******************************************/
// Functions
const byte byteMax = 127;
byte inputByte;
byte rpmMax = 30;
float rpm1, rpm2, rpm3;

float decodeByte() {
    int i = (int)inputByte;
    if ((i & 0x80) == 0x80){
        i ^= 0xFF;
        i++;
        i *= -1;
    }
    return (float)i * (float)rpmMax / (float)byteMax;
}

void readRPM() {
    if (Serial.available() >= 4) {
        rpmMax = Serial.read();
        inputByte = Serial.read();
        rpm1 = decodeByte();
        inputByte = Serial.read();
        rpm2 = decodeByte();
        inputByte = Serial.read();
        rpm3 = decodeByte();
    }
}

/*******************************************/

/*****************************************/
// setup()
byte b;
int i;
void setup() {
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz    

    Serial.begin(19200);
    Serial.println("1,2,3");
    

    
    // SONAR::init(13);
    
    // wheel1.PIDEnable(kc, taui,taud,sms);
    // wheel2.PIDEnable(kc, taui,taud,sms);
    // wheel3.PIDEnable(kc, taui,taud,sms);
}

/****************************************/
// loop()
void loop() {
    readRPM();
    Serial.print(rpm1);
    Serial.print(',');
    Serial.print(rpm2);
    Serial.print(',');
    Serial.print(rpm3);
    Serial.println();
}
