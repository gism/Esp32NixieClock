#include "Arduino.h"
#include "wifiMng.h"

ESP8266WebServer server(80);      // Create a webserver object that listens for HTTP request on port 80

const char* fwName;
const char* fwVersion;

// Default custom static IP
char static_ip[16] = "192.168.0.36";
char static_gw[16] = "192.168.0.1";
char static_sn[16] = "255.255.255.0";

char timeZone[6] = "1";

//flag for saving data
bool shouldSaveConfig = false;

void wifiBegin(const char* apName, const char* apVersion){

  Serial.println(F("\nWifi initialize"));
  
  fwName = apName;
  fwVersion = apVersion;

  fsBegin();          // Initialize File System

  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println(F("Reading config file"));
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      Serial.println(F("Opened config file"));
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      json.printTo(Serial);
      
      if (json.success()) {
        Serial.println(F("\nParsed json OK"));
        
        strcpy(timeZone, json["timeZone"]);

        uint32_t manualOffset = json["manualOffset"];
        setManualOffset(manualOffset);        

        if(json["ip"]) {
            Serial.println(F("Setting custom ip from config"));
            strcpy(static_ip, json["ip"]);
            strcpy(static_gw, json["gateway"]);
            strcpy(static_sn, json["subnet"]);
            Serial.println(static_ip);
          } else {
            Serial.println(F("No custom ip in config"));
          }
        
      } else {
        Serial.println(F("Failed to parse json config"));
      }
      configFile.close();
    }
  } else {
    Serial.println(F("/config.json does NOT exist"));
  }

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_timeZone("timeZone", "UTC+", timeZone, 6);
  
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Reset settings - for testing
  //wifiManager.resetSettings();
  
  //set static ip
  IPAddress _ip,_gw,_sn;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  // Sets timeout until configuration portal gets turned off
  // useful to make it all retry or go to sleep
  // in seconds
  wifiManager.setTimeout(WIFI_MANAGER_TIMEOUT);

  // Set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Add configuration parameter for UTC zone
  wifiManager.addParameter(&custom_timeZone);

  // To remove serial outputs from wifiManager:
  //wifiManager.setDebugOutput(false);

  // Fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect(apName)) {
    Serial.println(F("wifiMng: Failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 

  // If you get here you have connected to the WiFi
  Serial.println(F("wifiMng: Connected...yeey :)"));
  
  // Read updated parameters
  strcpy(timeZone, custom_timeZone.getValue());

  // Save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println(F("Saving config"));
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["timeZone"] = timeZone;
    json["manualOffset"] = 0;

    json["ip"] = WiFi.localIP().toString();
    json["gateway"] = WiFi.gatewayIP().toString();
    json["subnet"] = WiFi.subnetMask().toString();


    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println(F("Failed to open config file for writing"));
    }

    json.printTo(Serial);
    Serial.println();
    
    json.printTo(configFile);

    configFile.close();
    //end save
  }

  server.on(F("/ip"), [](){
    server.send(200, "text/plain", WiFi.localIP().toString().c_str());
  });

  server.on(F("/name"), [&](){
    server.send(200, "text/plain", fwName);
  } );

  server.on(F("/version"), [&](){
    server.send(200, "text/plain", fwVersion);
  } );

  server.onNotFound([&]() {                    // If the client requests any URI
    if (!handleFileRead(server.uri()))         // send it if it exists

    //server.send(404, "text/plain", "Nixie Clock\n404: Not Found");  // otherwise, respond with a 404 (Not Found) error

    // Redirect all not found to localhost/404.html.
    // This is captive portal
    // 303: redirect
    server.sendHeader("Location","/index.html");
    server.send(303);       
  });

  server.begin();                                   // Actually start the server
  Serial.println(F("HTTP server started"));
  
}


void wifiLoop(void){
  // Handle background tasks:
  server.handleClient();
}

void fsBegin(void){
  
  // Clean FS, for testing
  //SPIFFS.format();
  
  if (SPIFFS.begin()) {
    Serial.println(F("Mounted File System"));
  } else {
    Serial.println(F("Failed to mount FS"));
  }
 
}

bool handleFileRead(String path) {                      // Send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

// Callback notifying us of the need to save config
void saveConfigCallback (void) {
  Serial.println(F("Should save config"));
  shouldSaveConfig = true;
}

void printWifiInfo(void) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    char line[100];

    obj["ID"] = "debug";
    
    sprintf(line, "SSID: %s", WiFi.SSID().c_str());
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);

    sprintf(line, "Signal strength: %ld", WiFi.RSSI());
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);   
    
    IPAddress ip;
    ip = WiFi.localIP();
    sprintf(line, "IP: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);

    byte mac[6];
    WiFi.macAddress(mac);
    char macAddr[23];
    sprintf(line, "MAC: %2X:%2X:%2X:%2X:%2X:%2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);

    IPAddress gateway;
    gateway = WiFi.gatewayIP();
    sprintf(line, "Getway: %u.%u.%u.%u", gateway[0], gateway[1], gateway[2], gateway[3]);
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);
        
    IPAddress subnet;
    subnet = WiFi.subnetMask();
    sprintf(line, "Netmask: %u.%u.%u.%u", subnet[0], subnet[1], subnet[2], subnet[3]);
    Serial.println(line);
    obj["data"] = line;
    broadcastJson(obj);  
}
