/* Bewegungsmelder mit WEMOS D1
 * Wichtig! 10k Pulldown Widerderstand in der GPIO Lietung zu Pin D7 Setzen!
 * R9 auf der Sensorplatine mit Zange Rausknipsen!

Wirering for the Motion Sensor:
HC-SR501    NodeMCU
VCC         3.3V
GND         G
OUT         D7
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "GALiLEO"
#define wifi_password "!Nospam4me2020"

#define mqtt_server "10.10.1.215"  //#define mqtt_server "your_MQTT_Server_IP"
#define mqtt_user ""         
#define mqtt_password ""     

#define ESPHostname "ESP8266_02"

#define motion_topic "esp02/motion"

#define inTopic "esp02/inTopic"
#define outTopic "esp02/outTopic"


float diff = 1.0;

unsigned long delayTime;
//const int MOTION_PIN = 5;
int pirPin = D7;
int val;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(9600);
//  pinMode(MOTION_PIN, INPUT);
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

val = digitalRead(pirPin);
  if (val == LOW)
     {
      Serial.println("No motion");
      client.publish(motion_topic, "false");
     }
  else
     {
      Serial.println("Motion detected  ALARM");
      client.publish(motion_topic, "true");
     }
    delay(1500);
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
