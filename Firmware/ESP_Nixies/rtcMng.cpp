#include "Arduino.h"
#include "rtcMng.h"

RtcDS1307<TwoWire> Rtc(Wire);

uint32_t manualOffset = 0;


void rtcBegin(void) {
  
  delay(1000);
  Serial.print(F("\nRTC initialize:\nCode compiled: "));
  Serial.print(__DATE__);
  Serial.print(" @ ");
  Serial.println(__TIME__);

  Wire.begin(SDA_PIN, SCL_PIN); // GPIO 4 and GPIO 5 used for DS1307 I2C
  Rtc.Begin();
  
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //printDateTime(compiled);
  //Serial.println();

  if (!Rtc.IsDateTimeValid()) {
      if (Rtc.LastError() != 0) {
          // we have a communications error
          // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
          // what the number means
          Serial.print(F("RTC communications error = "));
          Serial.println(Rtc.LastError());
      } else {
          // Common Causes:
          //    1) first time you ran and the device wasn't running yet
          //    2) the battery on the device is low or even missing

          Serial.println(F("RTC lost confidence in the DateTime!"));
          // following line sets the RTC to the date & time this sketch was compiled
          // it will also reset the valid flag internally unless the Rtc device is
          // having an issue

          Rtc.SetDateTime(compiled);
      }
  }

  if (!Rtc.GetIsRunning()) {
      Serial.println(F("RTC was not actively running, starting now"));
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
      Serial.println(F("RTC is older than compile time!  (Updating DateTime)"));
      Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
      Serial.println(F("RTC is newer than compile time. (OK. This is expected)"));
  } else if (now == compiled) {
      Serial.println(F("RTC is the same as compile time! (not expected but all is fine)"));
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 

  Serial.print(F("RTC initialization OK.\n"));
  printRtcSerial();
  Serial.println();
}

RtcDateTime getRtcTime(void){

  RtcDateTime nowRtcTime = Rtc.GetDateTime();

  // This mess is to avoid changing utcOffsetInSeconds in NTP server. 
  // TO DO: Use same methods for manualOffset & utcOffsetInSeconds
  
  uint8_t offset_h = 0;
  uint8_t offset_m = 0;
  uint8_t offset_s = 0;

  uint8_t display_h = 0;
  uint8_t display_m = 0;
  uint8_t display_s = 0;

  offset_h = manualOffset / (60 * 60);
  offset_m = (manualOffset - offset_h * (60 * 60)) / 60;
  offset_s = manualOffset - (offset_h * 60 * 60) - (offset_m *60);

  display_s = nowRtcTime.Second() + offset_s;
  if (display_s > 59){
    display_s = display_s - 60;
    offset_m = offset_m + 1;
  }

  display_m = nowRtcTime.Minute() + offset_m;
  if (display_m > 59){
    display_m = display_m - 60;
    offset_h = offset_h + 1;
  }

  display_h = nowRtcTime.Hour() + offset_h;
  if (display_h > 23){
    display_h = display_h - 24;
  }

  RtcDateTime displayTime = RtcDateTime(nowRtcTime.Year(), nowRtcTime.Month(), nowRtcTime.Day(), display_h, display_m, display_s);
  return displayTime;
}

bool getRtcIsRunning(void){
  Rtc.GetIsRunning();
}

void setRtcRunning(bool b){
  Rtc.SetIsRunning(b);
}

void setRtcDateTime(RtcDateTime newTime){
  
  Serial.println(F("Set new RTC time: "));
  printDateTimeSerial(newTime);
  Serial.println();
  
  Rtc.SetDateTime(newTime);
}


void printRtcSerial(void){
  Serial.print(F("RTC time: "));
  RtcDateTime now = Rtc.GetDateTime();
  if (!Rtc.IsDateTimeValid()) {
      Serial.println(F("RTC lost confidence in the DateTime!"));
  }
  printDateTimeSerial(now);
  Serial.println();
}

void printDateTimeSerial(const RtcDateTime& dt) {
    char datestring[20];
    snprintf_P(datestring,
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

void setManualOffset(uint32_t offset){
  manualOffset = offset;
}

void increasH(void){
  manualOffset = manualOffset + (1 * 60 * 60);
  if (manualOffset >= (24 * 60 * 60)){
    manualOffset = manualOffset - (24 * 60 * 60);
  }
  saveManualOffset();
}

void decreaseH(void){
  if(manualOffset < (1 * 60 *60)){
    manualOffset = manualOffset + (23 * 60 * 60);
  }else{
    manualOffset = manualOffset - (1 * 60 *60);
  }
  saveManualOffset();
}

void increasM(void){
  manualOffset = manualOffset + (1 * 60);
  if (manualOffset >= (24 * 60 * 60)){
    manualOffset = manualOffset - (24 * 60 * 60);
  }
  saveManualOffset();
}

void decreaseM(void){
  if(manualOffset < (1 * 60)){
    manualOffset = manualOffset + (23 * 60 * 60) + (59 * 60);
  }else{
    manualOffset = manualOffset - (1 * 60);
  }
  saveManualOffset();
}

void increaseS(void){
  manualOffset = manualOffset + 1;
  if (manualOffset >= (24 * 60 * 60)){
    manualOffset = 0;
  }
  saveManualOffset();
}

void decreaseS(void){
  if (manualOffset == 0){
    manualOffset = (24 * 60 * 60) - 1;
  }else{
    manualOffset = manualOffset - 1;
  }
  saveManualOffset();
}

void saveManualOffset(void){
  
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
        
        json["manualOffset"] = manualOffset;

        configFile.close();
              
        File configFileW = SPIFFS.open("/config.json", "w");
        if (!configFileW) {
          Serial.println(F("Failed to open config file for writing"));
        }

        Serial.print("New Json: ");
        json.printTo(Serial);
        Serial.println();
        
        Serial.print("Json.printTo(configFile): ");
        uint8_t n = json.printTo(configFileW);
        Serial.print(n);
        
        configFileW.close();

        return;

      } else {
        Serial.println(F("Failed to parse json config"));
        return;
      }
      configFile.close();
    }
  }
  
}
