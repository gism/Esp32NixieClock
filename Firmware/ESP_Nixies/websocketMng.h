#ifndef websocketMng_h
#define websocketMng_h

  #include <WebSocketsServer.h>
  #include <ArduinoJson.h>
  #include <RtcDateTime.h>

  #include "rtcMng.h"
  #include "ntpMng.h"

  #include "wifiMng.h"

  // Intended public
  void websocketBegin(void);
  void websocketLoop(void);
  void broadcastJson(JsonObject& obj);

  // Intended private
  void updateGuest();
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
  
#endif
