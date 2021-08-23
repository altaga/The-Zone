#include <M5Core2.h>
#include <WiFi.h>
#include "time.h"

const char* ssid       = "YOUR_SSID";
const char* password   = "YOUR_PASSWORD";
const int offset = -5; // Your local timezone gmt offset

// Mexico offset -5 

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*offset;
const int   daylightOffset_sec = 0;

RTC_TimeTypeDef RTCtime;
RTC_DateTypeDef RTCDate;

char timeStrbuff[64];

void flushTime(){
    M5.Rtc.GetTime(&RTCtime); //Gets the time in the real-time clock.  
    M5.Rtc.GetDate(&RTCDate);
    sprintf(timeStrbuff,"%d/%02d/%02d %02d:%02d:%02d",  //Stores real-time time and date data to timeStrbuff.   
                        RTCDate.Year,RTCDate.Month,RTCDate.Date,
                        RTCtime.Hours,RTCtime.Minutes,RTCtime.Seconds);
    M5.lcd.setCursor(10,100); //Move the cursor position to (x,y). 
    M5.Lcd.println(timeStrbuff);  //Output the contents of.  
}

void setupTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  RTCtime.Hours = timeinfo.tm_hour; //Set the time.  
  RTCtime.Minutes = timeinfo.tm_min;
  RTCtime.Seconds = timeinfo.tm_sec;
  M5.Rtc.SetTime(&RTCtime); //and writes the set time to the real time clock.  

  RTCDate.Year = timeinfo.tm_year+1900;  //Set the date.  
  RTCDate.Month = timeinfo.tm_mon+1;
  RTCDate.Date = timeinfo.tm_mday;
  M5.Rtc.SetDate(&RTCDate);
}
/* After M5Core2 is started or reset
the program in the setUp () function will be run, and this part will only be run once.*/
void setup() {
  Serial.begin(115200);
  M5.begin();  //Init M5Core2.  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time aka Sync the RTC only once
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setupTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  flushTime();
  delay(1000);
}
