#ifndef ntpMng_h
#define ntpMng_h

  #include <NTPClient.h>
  #include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
  #include <WiFiUdp.h>
  
  #include <RtcDateTime.h>

  void ntpBegin(void);
  RtcDateTime getNtpTime(void);

  // Intended private:
  void printNtpSerial(void);
  
#endif
