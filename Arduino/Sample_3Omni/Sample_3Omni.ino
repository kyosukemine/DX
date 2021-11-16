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

/******************************************/
// demo
void setRPM(int rpm1, int rpm2, int rpm3) {
    wheel1.setGearedSpeedRPM(rpm1);
    wheel2.setGearedSpeedRPM(rpm2);
    wheel3.setGearedSpeedRPM(rpm3);
    
    wheel1.PIDRegulate();
    wheel2.PIDRegulate();
    wheel3.PIDRegulate();
}

int rpm = -30;
int tmp = 0;
int inputByte = 0;
bool minus = false;
unsigned long old = 0;
unsigned long now = 0;
float kc = 0.1, taui = 0.02, taud = 0;
unsigned int sms = 10;

/*****************************************/
// setup()
void setup() {
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz    

    Serial.begin(19200);
    
    // SONAR::init(13);
    
    wheel1.PIDEnable(kc, taui,taud,sms);
    wheel2.PIDEnable(kc, taui,taud,sms);
    wheel3.PIDEnable(kc, taui,taud,sms);
}

/****************************************/
// loop()
void loop() {
    now = millis();
    if (old == 0) old = now;
    if (now - old > 5000) {
        rpm *= -1;
        old = now;
    }
    setRPM(0, rpm, -rpm);
    Serial.println(wheel2.getGearedSpeedRPM());
    
    if (Serial.available() > 0) {
        inputByte = Serial.read();
        if (inputByte == '\n') {
            rpm = tmp;
            tmp = 0;
            if (minus) rpm *= -1;
            minus = false;
        } else if (inputByte == '-') {
            minus = true;
        }
        else {
            inputByte -= '0';
            tmp *= 10;
            tmp += inputByte;
        }
    }
}
