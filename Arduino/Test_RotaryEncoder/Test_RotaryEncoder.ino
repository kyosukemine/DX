#include <PinChangeInt.h>

#define ENC_A 6
#define ENC_B 7

bool cur[2];  // current state

void encRead() {
  cur[0] = digitalRead(ENC_A);
  cur[1] = digitalRead(ENC_B);
}

void pcAttachInt(int pin) {
  if(pin == 2 || ENC_A == 3) attachInterrupt(pin, encRead, CHANGE);
  else PCattachInterrupt(pin, encRead, CHANGE);
}

void setup() {
  Serial.begin(19200);
  Serial.println("A,B");

  pcAttachInt(ENC_A);
  pcAttachInt(ENC_B);
}

void loop() {
  Serial.print(cur[0]);
  Serial.print(',');
  Serial.print(cur[1]);
  Serial.println();
}
