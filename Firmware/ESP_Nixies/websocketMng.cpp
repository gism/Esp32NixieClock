#include "Arduino.h"
#include "websocketMng.h"

WebSocketsServer webSocket = WebSocketsServer(81);   // Create a webSocket object to communicate with web interface


void websocketBegin(void){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
}

void websocketLoop(void){
  webSocket.loop(); 
}

void broadcastJson(JsonObject& obj){
    String pl;
    obj.printTo(pl);
    webSocket.broadcastTXT(pl);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
   if (type == WStype_TEXT){
      String str = (char*)payload;
       
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& obj = jsonBuffer.createObject();
      
      if (str.equalsIgnoreCase("NTC")){
        Serial.println("Update NTC request");
        
        RtcDateTime ntpTime = getNtpTime();
        setRtcDateTime(ntpTime);
        
        obj["ID"] = "debug";
        obj["data"] = "OK: Request update by NTC";
      }
      else if (str.equalsIgnoreCase("Wifi")){
        Serial.println("Print Wifi-info request");
        printWifiInfo();
        
        obj["ID"] = "debug";
        obj["data"] = "OK: Print Wifi-info request";
      }else if (str.equals("hola")){
        Serial.println("Update guest time");
        updateGuest();
      }
      else if (str.equalsIgnoreCase("stop")){
        Serial.println("STOP request");
        setRtcRunning(false);
        
        obj["ID"] = "debug";
        obj["data"] = "OK: Stop request";
      }
      else  if (str.equalsIgnoreCase("start")){
        Serial.println("START request");
        setRtcRunning(true);
        
        obj["ID"] = "debug";
        obj["data"] = "OK: Start request";
      }
      else if (str.equals("H")){
          increasH();
          
//        tick();          
//        updateGuest();
//        saveConfig();

        Serial.println("+H");
        obj["ID"] = "debug";
        obj["data"] = "OK: +H";       
      }
      else if (str.equals("h")){
          decreaseH();

//        tick();
//        updateGuest();
//        saveConfig();

        Serial.println("-h");
        obj["ID"] = "debug";
        obj["data"] = "OK: -h";       
      }
      else if (str.equals("M")){
          increasM();
          
//        tick();
//        updateGuest();
//        saveConfig();

        Serial.println("+M");
        obj["ID"] = "debug";
        obj["data"] = "OK: +M";       
      }
      else if (str.equals("m")){
          
          decreaseM();
          
//        tick();
//        updateGuest();
//        saveConfig();

        Serial.println("-m");
        obj["ID"] = "debug";
        obj["data"] = "OK: -m";       
      }
      else if (str.equals("S")){
        
          increaseS();
//
//        tick();
//        updateGuest();
//        saveConfig();
        Serial.println("+S");
        obj["ID"] = "debug";
        obj["data"] = "OK: +S";       
      }
      else if (str.equals("s")){

          decreaseS();
          
//        tick();
//        updateGuest();
//        saveConfig();
        Serial.println("-s");
        obj["ID"] = "debug";
        obj["data"] = "OK: -s";       
      }
      else{
        Serial.print("Unknown command: ");
        Serial.println(str);
        obj["ID"] = "debug";
        obj["data"] = "Unknonwn command: " + str;
      }
      broadcastJson(obj);
   }else if(type == WStype_DISCONNECTED){
      Serial.printf("[%u] Disconnected!\n", num);
      
   }else if(type == WStype_CONNECTED){
    
      // Just print client info
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload); 

      updateGuest();
      
   }else if(type == WStype_BIN){
      Serial.printf("[%u] get binary length: %u\n", num, length);
   }
}

void updateGuest(){

  RtcDateTime dt = getRtcTime();
  uint8_t h = dt.Hour();
  uint8_t m = dt.Minute();
  uint8_t s = dt.Second();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["ID"] = "update_time";
  obj["data"] = String(h) + ";" + String(m) + ";" + String(s);
  broadcastJson(obj);

 if(getRtcIsRunning()){
  obj["ID"] = "start";
  obj["data"] = "?";
  broadcastJson(obj);
 }
 
}
