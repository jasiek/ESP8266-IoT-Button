#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

void setup() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("IoT Button");
}

void loop() {
  // put your main code here, to run repeatedly:
}
