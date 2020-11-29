#include "Arduino.h"
#include "ntpMng.h"

long utcOffsetInSeconds = 3600;
  
//// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void ntpBegin(void){
  timeClient.begin();
  Serial.println("\nNTP client started");
}

void printNtpSerial(void){
  
  timeClient.update();

  Serial.print(F("NTP server time: "));
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(" ");

  // NTPClient library does NOT have method to get day, month or year
  // but it is possible to calculate form with epochtime (time in seconds since Jan. 1, 1970)
  // To make it easier it is udesd time.h clases from esp32 sdk
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  
  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);
  
  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);
  
  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  Serial.print(dayStr);
  Serial.print("/");
  Serial.print(monthStr);
  Serial.print("/");
  Serial.print(yearStr);
  Serial.print(" ");
  
//  if (timeClient.getHours() < 10) Serial.print("0");
//  Serial.print(timeClient.getHours());
//  Serial.print(":");
//  if (timeClient.getMinutes() < 10) Serial.print("0");
//  Serial.print(timeClient.getMinutes());
//  Serial.print(":");
//  if (timeClient.getSeconds() < 10) Serial.print("0");
//  Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());

}

RtcDateTime getNtpTime(void){
  
  timeClient.update();

  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);

  uint16_t year = ti->tm_year + 1900;
  uint8_t month = ti->tm_mon + 1;
  uint8_t dayOfMonth = ti->tm_mday;
  
  uint8_t hour = timeClient.getHours();
  uint8_t minute = timeClient.getMinutes();
  uint8_t second = timeClient.getSeconds();
  
  RtcDateTime ntpTime = RtcDateTime(year, month, dayOfMonth, hour, minute, second);
  return ntpTime;
}
