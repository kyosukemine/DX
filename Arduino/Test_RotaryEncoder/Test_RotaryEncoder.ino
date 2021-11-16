#include <PinChangeInt.h>

#define ENC_A 6
#define ENC_B 7

byte bq;      // binary queue
int acc = 0;  // accumulation

void encRead() {
  bq = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
  if ((bq & 0x01) ^ (bq >> 1) == 1) acc++;
  else acc--;
}

void pcAttachInt(int pin) {
  if(pin == 2 || ENC_A == 3) attachInterrupt(pin, encRead, CHANGE);
  else PCattachInterrupt(pin, encRead, CHANGE);
}

void setup() {
  Serial.begin(19200);
//  Serial.println("A,B");
  Serial.println("acc");

  pcAttachInt(ENC_A);
}

void loop() {
//  Serial.print(cur[0]);
//  Serial.print(',');
//  Serial.print(cur[1]);
//  Serial.println();
  Serial.println(acc);
}
