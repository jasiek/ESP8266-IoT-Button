#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

WiFiManager wifiManager;
WiFiClient clientRegular;
WiFiClientSecure clientSecure;
char mqtt_server[40] = "192.168.178.8";
char mqtt_port[5] = "1883";
char mqtt_username[40];
char mqtt_password[40];
char mqtt_ssl[3] = "no";
MQTTClient mqtt(128);

// Find out if we've been intentionally reset, or is this a regular power on.
bool triggered() {
  struct rst_info *rinfo = ESP.getResetInfoPtr();
  Serial.println(ESP.getResetInfo());
  return (rinfo->reason == REASON_DEEP_SLEEP_AWAKE);
}

// Handle Wi-Fi configuration and any additional plugins we may want to configure.
bool connect_to_network() {
  // TODO: -> plugins
  WiFiManagerParameter p_mqtt_server("mqtt_server", "MQTT Hostname", mqtt_server, 40);
  WiFiManagerParameter p_mqtt_port("mqtt_port", "MQTT Port", mqtt_port, 5);
  WiFiManagerParameter p_mqtt_username("mqtt_username", "MQTT Username", mqtt_username, 40);
  WiFiManagerParameter p_mqtt_password("mqtt_password", "MQTT Password", mqtt_password, 40);
  WiFiManagerParameter p_mqtt_ssl("mqtt_ssl", "Enable SSL", mqtt_ssl, 3);

  wifiManager.addParameter(&p_mqtt_server);
  wifiManager.addParameter(&p_mqtt_port);
  wifiManager.addParameter(&p_mqtt_username);
  wifiManager.addParameter(&p_mqtt_password);
  wifiManager.addParameter(&p_mqtt_ssl);

  bool success = wifiManager.autoConnect("IoT Button");

  if (success) {
    strcpy(mqtt_server, p_mqtt_server.getValue());
    strcpy(mqtt_port, p_mqtt_port.getValue());
    strcpy(mqtt_username, p_mqtt_username.getValue());
    strcpy(mqtt_password, p_mqtt_password.getValue());
    strcpy(mqtt_ssl, p_mqtt_ssl.getValue());
  } else {
    // UNHANDLED
  }

  return success;
}

void notify() {
  // TODO: -> plugins
  bool ssl = (strcmp(mqtt_ssl, "yes") == 0);

  int i_mqtt_port = atoi(mqtt_port);
  mqtt.begin(mqtt_server, i_mqtt_port, ssl ? clientSecure : clientRegular);
  if (mqtt.connect("iot button")) {
    Serial.println("connected");
    if (mqtt.publish("devices/button", "click")) {
      Serial.println("published");
    }
  }
}

void setup() {
  pinMode(D0, INPUT_PULLUP);
  if (digitalRead(D0)) {
    wifiManager.resetSettings();
    delay(1000);
  }

  Serial.begin(115200);
  Serial.println("ESP8266-IoT-Button starting up...");
  // Have we been powered up, or this is an intentional reset?
  if (triggered()) {
    Serial.println("Looks like we were triggered.");
    if (connect_to_network()) {
      Serial.println("Successfully connected to WiFi network.");
      notify();
    } else {
      // UNHANDLED
    }
  }
  Serial.println("Going to sleep.");
  ESP.deepSleep(0); // Sleep forever.
}

void loop() {
  // NOOP
}
