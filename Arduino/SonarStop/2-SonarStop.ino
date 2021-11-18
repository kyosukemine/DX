// ヘッダファイルのインクルード
#include <EEPROM.h>

#include <fuzzy_table.h>
#include <PID_Beta6.h>

#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

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
// 各モータのインスタンスを設定
irqISR(irq1,isr1);
MotorWheel wheel1(9,8,6,7,&irq1);        // Pin9:PWM, Pin8:DIR, Pin6:PhaseA, Pin7:PhaseB

irqISR(irq2,isr2);
MotorWheel wheel2(10,11,14,15,&irq2);    // Pin10:PWM, Pin11:DIR, Pin14:PhaseA, Pin15:PhaseB

irqISR(irq3,isr3);
MotorWheel wheel3(3,2,4,5,&irq3);        // Pin3:PWM, Pin2:DIR, Pin4:PhaseA, Pin5:PhaseB
/******************************************/

/******************************************/
/// 3モータのPWM制御を行う関数, 引数が正の値の場合は正転，負の値の場合は逆転
/// PWM1: 後方のモータに対するPWM出力(-255~255)
/// PWM2: 右前方のモータに対するPWM出力(-255~255)
/// PWM3: 左前方のモータに対するPWM出力(-255~255)
void setCarMotorPWM(int PWM1, int PWM2, int PWM3){
  if(PWM1 < 0) wheel1.backoffPWM(-PWM1);
  else wheel1.advancePWM(PWM1);
  if(PWM2 < 0) wheel2.backoffPWM(-PWM2);
  else wheel2.advancePWM(PWM2);
  if(PWM3 < 0) wheel3.backoffPWM(-PWM3);
  else wheel3.advancePWM(PWM3);
}
/// 静止させる関数
void setCarStop() {
  setCarMotorPWM(0, 0, 0);
}
/// 前進させる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarAdvancePWM(int PWM) {
  setCarMotorPWM(0, -PWM, PWM);
}
/// 後退させる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarBackoffPWM(int PWM) {
  setCarMotorPWM(0, PWM, -PWM);
}
/// 左へ進ませる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarLeftPWM(int PWM) {
  setCarMotorPWM(PWM, -PWM * 0.5, -PWM * 0.5);
}
/// 右へ進ませる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarRightPWM(int PWM) {
  setCarMotorPWM(-PWM, PWM * 0.5, PWM * 0.5);
}
/// 左へ旋回させる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarTurnLeftPWM(int PWM) {
    setCarMotorPWM(-PWM, -PWM, -PWM);
}
/// 右へ旋回させる関数
/// PWM: モータに対するPWM出力(0~255)
void setCarTurnRightPWM(int PWM) {
    setCarMotorPWM(PWM, PWM, PWM);
}

/*****************************************/
// 起動時の処理
void setup() {
  // PWM出力の周波数を設定
  TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
  TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

  Serial.begin(19200);
  sonarsUpdate();
  delay(500);
  
  setCarAdvancePWM(64);
}

/****************************************/
// 繰り返す処理
void loop() {
  char key;

  sonarsUpdate();

  if (distBuf[0] < 20) setCarStop();

  delay(10);
}
