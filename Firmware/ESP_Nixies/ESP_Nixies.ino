#include "rtcMng.h"
#include "nixiesMng.h"
#include "wifiMng.h"
#include "websocketMng.h"
#include "ntpMng.h"

#include <RtcDateTime.h>

const char* PROGMEM FIRMWARE_NAME = "ESP_NixieClock";
const char* PROGMEM FIRMWARE_VER = "1.0";

uint8_t updateNtcHour = 0;
const uint8_t NTC_UPDATE_HOUR_INTERVAL = 6;

uint32_t printRtcTime = 0;
const uint32_t PRINT_RTC_INTERVAL = 300000;    // 300s

uint32_t updateNixisTime = 0;
const uint32_t UPDATE_NIXIES_RTC_INTERVAL = 500;

// To manage HH:mm:ss separator
bool tick = true;
uint32_t tickTime = 0;
const uint32_t TICK_INTERVAL = 1000;


void setup() {
  
  Serial.begin(115200);    // Init debug serial
  printInfo();             // Print serial board and binary information (debug)

  nixiesBegin();
  nixiesPrint123();

  wifiBegin(FIRMWARE_NAME, FIRMWARE_VER);
  websocketBegin();

  rtcBegin();              // Initialize RTC (DS1307)

  ntpBegin();
  // printNtpSerial();
  
  RtcDateTime ntpTime = getNtpTime();
  setRtcDateTime(ntpTime);

  updateNtcHour = updateNtcHour + NTC_UPDATE_HOUR_INTERVAL;
  if (updateNtcHour > 24) updateNtcHour = updateNtcHour - 24;
  
  nixiesPrintTime(getRtcTime(), tick);
  
}

void loop() {

  if (millis() > printRtcTime){
    printRtcSerial();
    printNtpSerial();
    getNtpTime();
    printRtcTime = millis() + PRINT_RTC_INTERVAL;
  }

  if (millis() > tickTime){
    if(getRtcIsRunning()) tick = !tick;
    tickTime = millis() + TICK_INTERVAL;
  }

  if (millis() > updateNixisTime){
    RtcDateTime rtcTime = getRtcTime();
    nixiesPrintTime(rtcTime, tick);

    if (rtcTime.Hour() == updateNtcHour){

      RtcDateTime ntpTime = getNtpTime();
      setRtcDateTime(ntpTime);

      updateNtcHour = rtcTime.Hour() + NTC_UPDATE_HOUR_INTERVAL;
      if (updateNtcHour > 24) updateNtcHour = updateNtcHour - 24;
    }
    updateNixisTime = millis() + UPDATE_NIXIES_RTC_INTERVAL;
  }
  
  wifiLoop();
  websocketLoop();
  manageUart();
  
}

void printInfo(){

  Serial.print(F("\n\n\n"));
  Serial.print(FIRMWARE_NAME);
  Serial.print(F(" - "));
  Serial.println(FIRMWARE_VER);

  Serial.print(F("Compiled: "));
  Serial.print(__DATE__);
  Serial.print(F(" "));
  Serial.println(__TIME__);
  Serial.print(F("File: "));
  Serial.println(__FILE__);

  Serial.print(F("Sketch MD5: "));
  Serial.println(ESP.getSketchMD5());
  
  Serial.print(F("Flash CRC: "));
  Serial.println(ESP.checkFlashCRC());
  
  Serial.print(F("ChipId: "));
  Serial.println(ESP.getChipId());

  Serial.print(F("FlashChipId: "));
  Serial.println(ESP.getFlashChipId());

  Serial.print(F("CoreVersion: "));
  Serial.println(ESP.getCoreVersion());

  Serial.print(F("SdkVersion: "));
  Serial.println(ESP.getSdkVersion());

  Serial.print(F("CpuFreqMHz: "));
  Serial.println(ESP.getCpuFreqMHz());

  Serial.print(F("FlashChipSpeed: "));
  Serial.println(ESP.getFlashChipSpeed());

  Serial.print(F("ESP.getSketchSize() "));
  Serial.println(ESP.getSketchSize());

  Serial.print(F("ESP.getFreeSketchSpace() "));
  Serial.println(ESP.getFreeSketchSpace());
  
  Serial.print(F("ESP.getFlashChipSize() "));
  Serial.println(ESP.getFlashChipId());

  Serial.print(F("ESP.getFlashChipRealSize() "));
  Serial.println(ESP.getFlashChipId());

  Serial.println();
}

void manageUart(void){
  
  // Check if there is any UART command:
  if (!Serial.available()) {
    return;
  }
  
  uint8_t c = Serial.read();
  if(c == 0xA)                                      // if new line on UART print menu
  {
    Serial.println(F("\n\nUART MENU:"));
    Serial.println(F("?: Compilation info"));

    Serial.println(F("R: Initialize RTC"));
    Serial.println(F("r: Print RTC time"));
    
    Serial.println();
    return;
  }else if(c == 0x0D){
    return;    
  }else if (c == '?'){
    printInfo();
    return;
    
  }else if (c == 'R'){
    rtcBegin();
    return;
  }else if (c == 'r'){
    printRtcSerial();
    return;
    
  }else if (c == 'w'){
    printWifiInfo();
    return;  
    
  }else if (c == 'd'){
    digitalWrite(DATA_PIN, !digitalRead(DATA_PIN));
    return;
  }else if (c == 'c'){
    digitalWrite(CLOCK_PIN, !digitalRead(CLOCK_PIN));
    return;
  }else if (c == 'l'){
    digitalWrite(LATCH_PIN, !digitalRead(LATCH_PIN));
    return;
  }
}
