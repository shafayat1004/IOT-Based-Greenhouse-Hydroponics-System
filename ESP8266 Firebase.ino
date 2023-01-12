#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

#define FIREBASE_HOST "YOUR HOST LINK"
#define FIREBASE_AUTH "Your Firebase Database Secret" 
#define WIFI_SSID "your WiFi SSID"
#define WIFI_PASSWORD "your WiFi password"

#define SUNRISESUNSETAPI "https://api.sunrise-sunset.org/json?lat=23.815862779496594&lng=90.4277160918885"

char command;


FirebaseData firebaseData;
FirebaseJson currentConfig;
FirebaseJson sensorDataJson;

StaticJsonDocument<256> sensorData;

const long utcOffsetInSeconds = 21600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Defining NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;
DynamicJsonDocument sunriseSunsetAPI(1024);
StaticJsonDocument<64> sunriseSunset;
StaticJsonDocument<64> currentTime;

int sunriseHour;
int sunriseMinute;
int sunriseSecond;
int sunsetHour;
int sunsetMinute;
int sunsetSecond;

void setup()
{
  Serial.begin(4800);
  /* ================================================================= */
  /* ==============================WIFI=============================== */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    // Serial.print(".");
    delay(300);
  }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();

  timeClient.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}

void loop()
{
  command = Serial.read();
  if(command == 'T'){
    timeClient.update();
    currentTime["D"] = timeClient.getDay();
    currentTime["H"] = timeClient.getHours();
    currentTime["M"] = timeClient.getMinutes();
    currentTime["S"] = timeClient.getSeconds();

    Serial.print('R');
    serializeJson(currentTime, Serial);
    
  }
  else if (command == 'V'){
    Firebase.getJSON(firebaseData, "/current_config");
    currentConfig = firebaseData.to<FirebaseJson>();
    Serial.print('R');
    currentConfig.toString(Serial);
  }
  else if (command == 'U'){

    deserializeJson(sensorData, Serial);

    sensorDataJson.add("airTemp", int(sensorData["airTemp"]));
    sensorDataJson.add("humidity", int(sensorData["humidity"]));
    sensorDataJson.add("lux", int(sensorData["lux"]));
    sensorDataJson.add("nutrientTemp", int(sensorData["nutrientTemp"]));
    sensorDataJson.add("pH", float(sensorData["pH"]));

    Firebase.setJSON(firebaseData, "/current_sensor_data", sensorDataJson);
    Serial.flush();
  } 
  else if (command == 'S'){
      http.begin(client, "SUNRISESUNSETAPI");
      http.GET();
      deserializeJson(sunriseSunsetAPI, http.getStream());
      JsonObject results = sunriseSunsetAPI["results"];
      const char* results_sunrise = results["sunrise"]; // "12:23:58 AM"
      const char* results_sunset = results["sunset"]; // "11:11:57 AM"
      http.end();

      sunriseSunset["sunrise"] = results_sunrise; // "12:23:58 AM"
      sunriseSunset["sunset"] = results_sunset; // "11:11:57 AM"
      Serial.print('R');
      serializeJson(sunriseSunset, Serial);
  }
  
  delay(1000);
}