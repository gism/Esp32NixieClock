#include "Arduino.h"
#include "nixiesMng.h"

void nixiesBegin(void){
  Serial.print("Initializing Nixies HW...");
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  digitalWrite(DATA_PIN, HIGH);
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(LATCH_PIN, HIGH);
  
  Serial.println("OK");

  // Clear nixies digits
  for (uint8_t i = 0; i < 8; i = i + 1) {
    nixiesPrintDigit(DIGITS_OFF, false);
  }
  nixiesLatchDigits();
  
}


void nixiesPrintTime(RtcDateTime dt, bool litSeparators){

  nixiesPrintSeparator(litSeparators);

  uint8_t h = dt.Hour();
  uint8_t m = dt.Minute();
  uint8_t s = dt.Second();
  
  nixiesPrintDU(h);
  nixiesPrintDU(m);
  nixiesPrintDU(s);

  nixiesLatchDigits();
  
}

void nixiesPrintDU(uint8_t v){
  
  uint8_t u = (v / 1U) % 10;
  uint8_t d = (v / 10U) % 10;

  nixiesPrintDigit (d, false);
  nixiesPrintDigit (u, false);
  
}
  
void nixiesPrintSeparator(bool b){
  
  digitalWrite(CLOCK_PIN, HIGH);
  if (b) {
    digitalWrite(DATA_PIN, LOW);
  }else{
    digitalWrite(DATA_PIN, HIGH);
  }
  digitalWrite(CLOCK_PIN, LOW);
  
  digitalWrite(DATA_PIN, HIGH);
  digitalWrite(CLOCK_PIN, HIGH);

}

void nixiesPrintDigit(uint8_t p, bool dot){
  
  if (p > 9) p = DIGITS_OFF;
  if (p == 0) p = 10;
  
  // PRINT DIGIT
  for (uint8_t i = 10; i > 0; i = i - 1) {
    digitalWrite(CLOCK_PIN, HIGH);
    if (i == p){
      digitalWrite(DATA_PIN, LOW);
    }else{
      digitalWrite(DATA_PIN, HIGH);
    }
    digitalWrite(CLOCK_PIN, LOW);
  }
  digitalWrite(CLOCK_PIN, HIGH);

  // PRINT DOT
  if (dot){
    digitalWrite(DATA_PIN, LOW);
  }else{
    digitalWrite(DATA_PIN, HIGH);
  }
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(DATA_PIN, HIGH);

  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(DATA_PIN, HIGH);

}

void nixiesLatchDigits(void){
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(LATCH_PIN, HIGH);
}

void nixiesPrint123(void){

  for (uint8_t i = 7; i > 0; i = i - 1) {
    nixiesPrintDigit(DIGITS_OFF, false);
    nixiesLatchDigits();
  }

  for (uint8_t i = 0; i < 10; i = i + 1) {
    nixiesPrintDigit(i, false);
    nixiesLatchDigits();
    delay(1000);
  }

  for (uint8_t i = 6; i > 0; i = i - 1) {
    nixiesPrintDigit(DIGITS_OFF, false);
    nixiesLatchDigits();
    delay(1000);
  }

}  
