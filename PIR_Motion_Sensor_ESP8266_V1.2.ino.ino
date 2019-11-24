/*
Project:  PIR Motion dedector with a ESP8266 / NodeMCU
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.11.2019 / updated xx.xx.xxxx
Version:  Edi's Techlab Template V1.1

Changelog Techlab Template:
OTA implementation 
Placeholder for inTopics
 
Required libraries (sketch -> include library -> manage libraries)
 - ESP8266WiFi by Ivan Grokhotkov
 - PubSubClient by Nick ‘O Leary
 - ArduinoOTA
 - Adafruit Sensor library

Wirering for the Motion Sensor:
HC-SR501    NodeMCU
VCC         3.3V
GND         G
OUT         D1 / GPIO5
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Adafruit_Sensor.h>

#define wifi_ssid "MacWorld"  //#define wifi_ssid "your_SSID"
#define wifi_password "E2943228373F65C0"  //#define wifi_password "your_Password"

#define mqtt_server "PiHomeserver.local"  //#define mqtt_server "your_MQTT_Server_IP"
#define mqtt_user "your_username"         
#define mqtt_password "your_password"     

#define ESPHostname "ESP8266_02"

#define motion_topic "esp02/motion"

#define inTopic "esp02/inTopic"
#define outTopic "esp02/outTopic"


float diff = 1.0;

unsigned long delayTime;
const int MOTION_PIN = 5;
int pir = 1;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  pinMode(MOTION_PIN, INPUT);
  setup_wifi();
   ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delayTime = 1000;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();

  if ((digitalRead(MOTION_PIN)) == 0) delay(250);

  else {
    client.publish(motion_topic, "TRUE");
    Serial.println("Message Published: TRUE");
    while (digitalRead(MOTION_PIN) == 1) {
      client.publish(motion_topic, "TRUE");
      Serial.println("Message Published: TRUE");
      delay(500);
    }
    pir = 0;
    client.publish(motion_topic, "FALSE");
    Serial.println("Message Published: FALSE");
    delay(500);
  }
  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    //If the first Value of the payload is 1 
  } else {
    // Room for Code
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, ESPHostname);
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
