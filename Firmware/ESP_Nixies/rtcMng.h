#ifndef rtcMng_h
#define rtcMng_h

  #include <Wire.h>                   // Include for I2C
  #include <RtcDS1307.h>              // Include for RTC

  #include <FS.h>
  #include <ArduinoJson.h>
  
  // CONNECTIONS:
  // DS1307 SDA --> SDA --> D2  --> GPIO4 (Wemos D1 Mini)
  // DS1307 SCL --> SCL --> D1  --> GPIO5 (Wemos D1 Mini)
  // DS1307 VCC --> 5v
  // DS1307 GND --> GND

  #define SDA_PIN 4    // D7 (GPIO13 Wemos D1 Mini) - SRCK  
  #define SCL_PIN 5    // D8 (GPIO15 Wemos D1 Mini) - RCK

  #define countof(a) (sizeof(a) / sizeof(a[0]))
  
  // Intended public
  void rtcBegin(void);
  void setRtcDateTime(RtcDateTime newTime);
  RtcDateTime getRtcTime(void); 

  bool getRtcIsRunning(void);
  void setRtcRunning(bool b);
  
  void setManualOffset(uint32_t offset);
  void increasH(void);
  void decreaseH(void);
  void increasM(void);
  void decreaseM(void);
  void increaseS(void);
  void decreaseS(void);
  
  // Intended private
  void printRtcSerial(void);
  void printDateTimeSerial(const RtcDateTime& dt);
  void saveManualOffset(void);
  
#endif
