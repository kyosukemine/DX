// ヘッダファイルのインクルード
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
// Motor
irqISR(irq1,isr1);
MotorWheel wheel1(9,8,6,7,&irq1);        // Pin9:PWM, Pin8:DIR, Pin6:PhaseA, Pin7:PhaseB

irqISR(irq2,isr2);
MotorWheel wheel2(10,11,14,15,&irq2);    // Pin10:PWM, Pin11:DIR, Pin14:PhaseA, Pin15:PhaseB

irqISR(irq3,isr3);
MotorWheel wheel3(3,2,4,5,&irq3);        // Pin3:PWM, Pin2:DIR, Pin4:PhaseA, Pin5:PhaseB
/******************************************/

const byte byteMax = 127;
const float kc = 0.1, taui = 0.02, taud = 0.0;  // 
const unsigned int sms = 10;                    // サンプリング周期
const unsigned short threshDist = 20            // 停止する距離の閾値[cm]

byte inputByte;
byte rpmMax = 30;
float rpm1 = 0.0, rpm2 = 0.0, rpm3 = 0.0;
bool dir1 = DIR_ADVANCE, dir2 = DIR_ADVANCE, dir3 = DIR_ADVANCE;

/******************************************/
// Functions
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

void PIDEnable() {
    wheel1.PIDEnable(kc, taui, taud, sms);
    wheel2.PIDEnable(kc, taui, taud, sms);
    wheel3.PIDEnable(kc, taui, taud, sms);
}

void PIDRegulate() {
    wheel1.PIDRegulate();
    wheel2.PIDRegulate();
    wheel3.PIDRegulate();
}

void setCurrDir() {
    if (rpm1 > 0.0) dir1 = DIR_ADVANCE;
    else if (rpm1 < 0.0) dir1 = DIR_BACKOFF;
    if (rpm2 > 0.0) dir2 = DIR_ADVANCE;
    else if (rpm2  < 0.0) dir2 = DIR_BACKOFF;
    if (rpm3 > 0.0) dir3 = DIR_ADVANCE;
    else if (rpm3 < 0.0) dir3 = DIR_BACKOFF;
}

void setGearedSpeedRPM() {
    wheel1.setGearedSpeedRPM(abs(rpm1), dir1);
    wheel2.setGearedSpeedRPM(abs(rpm2), dir2);
    wheel3.setGearedSpeedRPM(abs(rpm3), dir3);
}
/******************************************/

/*****************************************/
// setup
void setup() {
    // PWM出力の周波数を設定
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

    PIDEnable();

    rpm1 = 0.0;
    rpm2 = -11.6;
    rpm3 = 11.6;
}

/****************************************/
// loop
void loop() {
    sonarsUpdate();

    if (distBuf[0] < threshDist) {
        rpm1 = 0.0;
        rpm2 = 0.0;
        rpm3 = 0.0;
    }

    setCurrDir();
    setGearedSpeedRPM();
    PIDRegulate();
}
