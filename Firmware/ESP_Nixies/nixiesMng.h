#ifndef nixiesMng_h
#define nixiesMng_h

  #include <RtcDateTime.h>

  // Nixies drived by TPIC6B595
  #define DATA_PIN  12    // D6 (GPIO12 Wemos D1 Mini) - SER IN
  #define CLOCK_PIN 13    // D7 (GPIO13 Wemos D1 Mini) - SRCK  
  #define LATCH_PIN 15    // D8 (GPIO15 Wemos D1 Mini) - RCK

  #define DIGITS_OFF 55    // Any value grater than 10 is ok

  // Intended public
  void nixiesBegin(void);
  void nixiesPrintTime(RtcDateTime newTime, bool litSeparators);
  
  // Intended private
  void nixiesPrintSeparator(bool b);
  void nixiesPrintDU(uint8_t v);
  void nixiesPrintDigit(uint8_t p, bool dot);
  void nixiesLatchDigits(void);
  void nixiesPrint123(void);

#endif
