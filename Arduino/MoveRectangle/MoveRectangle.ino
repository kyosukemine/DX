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

float rpm1 = 0.0, rpm2 = 0.0, rpm3 = 0.0;
bool dir1 = DIR_ADVANCE, dir2 = DIR_ADVANCE, dir3 = DIR_ADVANCE;

/******************************************/
// Functions
void setCurrDir() {
    if (rpm1 > 0.0) dir1 = DIR_ADVANCE;
    else if (rpm1 < 0.0) dir1 = DIR_BACKOFF;
    if (rpm2 > 0.0) dir2 = DIR_ADVANCE;
    else if (rpm2  < 0.0) dir2 = DIR_BACKOFF;
    if (rpm3 > 0.0) dir3 = DIR_ADVANCE;
    else if (rpm3 < 0.0) dir3 = DIR_BACKOFF;
}

void setGearedSpeedRPM() {
    wheel1.setGearedSpeedRPM(rpm1, dir1);
    wheel2.setGearedSpeedRPM(rpm2, dir2);
    wheel3.setGearedSpeedRPM(rpm3, dir3);
}
/******************************************/

float kc = 0.1, taui = 0.02, taud = 0.0;
unsigned int sms = 10;
int cnt = 0;
unsigned long old = 0;
unsigned long interval = 1000;
float baserpm = 10.0;

/*****************************************/
// setup
void setup() {
    // PWM出力の周波数を設定
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

    Serial.begin(19200);

    wheel1.PIDEnable(kc, taui, taud, sms);
    wheel2.PIDEnable(kc, taui, taud, sms);
    wheel3.PIDEnable(kc, taui, taud, sms);

    rpm1 = 0.0;
    rpm2 = -11.6;
    rpm3 = 11.6;
    cnt = 0;
    old = millis();
}

/****************************************/
// loop
void loop() {
    if (millis() - old > interval){
        if (cnt % 2 == 0) {
            rpm1 = 0;
            rpm2 = 0;
            rpm3 = 0;
        } else {
            if (cnt / 2 == 0) {
                rpm1 = baserpm;
                rpm2 = -baserpm * 0.5;
                rpm3 = -baserpm * 0.5;
            } else if (cnt / 2 == 1) {
                rpm1 = 0.0;
                rpm2 = baserpm * 1.16;
                rpm3 = -baserpm * 1.16;
            } else if (cnt / 2 == 2) {
                rpm1 = -baserpm;
                rpm2 = baserpm * 0.5;
                rpm3 = baserpm * 0.5;
            } else if (cnt / 2 == 3) {
                rpm1 = 0.0;
                rpm2 = -baserpm * 1.16;
                rpm3 = baserpm * 1.16;
            }
        }
        old = millis();
        cnt++;
        cnt %= 8;
    }

    setCurrDir();
    setGearedSpeedRPM();

    wheel1.PIDRegulate();
    wheel2.PIDRegulate();
    wheel3.PIDRegulate();
}
