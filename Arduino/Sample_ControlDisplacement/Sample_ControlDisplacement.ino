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
void PIDEnable(float kc, float taui, float taud, unsigned int sms) {
    wheel1.PIDEnable(kc, taui, taud, sms);
    wheel2.PIDEnable(kc, taui, taud, sms);
    wheel3.PIDEnable(kc, taui, taud, sms);
}

void PIDRegulate() {
    wheel1.PIDRegulate();
    wheel2.PIDRegulate();
    wheel3.PIDRegulate();
}

void setRPM(int rpm1, int rpm2, int rpm3) {
    wheel1.setGearedSpeedRPM(rpm1);
    wheel2.setGearedSpeedRPM(rpm2);
    wheel3.setGearedSpeedRPM(rpm3);
}

void getCurrPulse(long& pulse1, long& pulse2, long& pulse3) {
    pulse1 = wheel1.getCurrPulse();
    pulse2 = wheel2.getCurrPulse();
    pulse3 = wheel3.getCurrPulse();
}

int readSerialNum(char endc) {
    int inputByte;
    int tmp = 0;
    int out = 0;
    bool minus = false;
    if (Serial.available() > 0) {
        while (inputByte != endc || inputByte != '\n') {
            inputByte = Serial.read();
            if (inputByte == endc) {
                out = tmp;
                tmp = 0;
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
    out = tmp;
    if (minus) out *= -1;
    return out;
}

const int geared_ppr = CPR * REDUCTION_RATIO;
const float kc = 0.1, taui = 0.02, taud = 0;
const unsigned int sms = 10;

int inputByte = 0;
bool minus = false;
long start1, start2, start3;
long crr1, crr2, crr3;
float deg1 = 0, deg2 = 0, deg3 = 0;

/*****************************************/
// setup()
void setup() {
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

    Serial.begin(19200);
    
    // SONAR::init(13);
    
    PIDEnable(kc, taui, taud, sms);
}

/****************************************/
// loop()
void loop() {
    deg1 = readSerialNum(' ');
    deg2 = readSerialNum(' ');
    deg3 = readSerialNum('\n');

    PIDRegulate();
}
