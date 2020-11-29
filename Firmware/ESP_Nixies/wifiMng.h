#ifndef wifiMng_h
#define wifiMng_h

  #include <FS.h>                   // This needs to be first, or it all crashes and burns...
  #include <ESP8266WiFi.h>          // ESP8266 Core WiFi Library (you most likely already have this in your sketch)
  #include <WiFiClient.h>

//  #include <ESP8266mDNS.h>

  #include <ESP8266WebServer.h>     // Local WebServer used to serve the configuration portal
  #include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

  #include <ArduinoJson.h>

  #include "websocketMng.h"

  #define WIFI_MANAGER_TIMEOUT 120  // Seconds
 
  void wifiBegin(const char* apName, const char* apVersion);
  void wifiLoop(void);
  void printWifiInfo(void);

  // Intended private
  void fsBegin(void);
  
  bool handleFileRead(String path);
  String getContentType(String filename);
  void saveConfigCallback(void);
  
#endif
