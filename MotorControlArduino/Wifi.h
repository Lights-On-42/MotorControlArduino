//#pragma once
//#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <WiFiManager.h>  //von tzapu

class Wifi
{
public:
    static void login();
    static void loginInspecifictWLan(char* WLan_Name,char* WLan_Password);
    static const char *ssid();
    static void hotspot();
    static WiFiManager manager;
};
WiFiManager Wifi::manager = WiFiManager();


void Wifi::login()
{
  
  //Wifi::loginInspecifictWLan("ZW public","");


  Wifi::hotspot();

  
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  WiFi.setHostname("RAuto");
  WiFi.hostname("RAuto");
  //             http://rauto/
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
}

void Wifi::loginInspecifictWLan(char* WLan_Name,char* WLan_Password)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLan_Name, WLan_Password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter == 20)
    {
      Serial.println("WiFi not connected!!!");
      break;
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    //wir geben die IP adresse die uns das Wlan gegeben hat aus
    Serial.println(WiFi.localIP());
  }
}

void Wifi::hotspot()
{   
  if(WiFi.status() == WL_CONNECTED)
  {
    return;
  }
  //manager.resetSettings();
  manager.autoConnect("PW:passwort","passwort");

  Serial.println("WiFi created");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
}