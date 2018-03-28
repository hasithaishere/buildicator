/**
  E25 Hack | 2018
  Name: Buildicator Client
  Purpose: IOT enabled build notifier for Jenkins, following code used in ESP8266 module.

  @author Hasitha Gamage <hasitha.hpmax@gmail.com>
  @version 1.0
*/

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your hardware/network.
const char* ssid = "hasimnet";
const char* password = "123456789Hp@";

const char* mqtt_server = "m10.cloudmqtt.com";
const char* mqtt_username = "ebshoqjf";
const char* mqtt_password = "6VL4GW2Ngk0D";
const char* mqtt_topic = "buildicator-control";

const char* clientId = "buildicator";
const char* deviceId = "did-001";

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;
long lightTime = 0;

boolean reconnect() {
  if (client.connect(clientId,mqtt_username,mqtt_password)) {
    // Once connected, publish an announcement...
    client.publish(mqtt_topic,"Buildicator|did-001 - CONNECTED");
    // ... and resubscribe
    client.subscribe(mqtt_topic);
  }
  return client.connected();
}

void setup()
{
  Serial.begin(57600);
  setup_wifi();
  client.setServer(mqtt_server, 11895);
  client.setCallback(callback);

  lastReconnectAttempt = 0;

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void setup_wifi() {
  delay(10);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  char *arraym = (char*)payload;
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(arraym);
  
  const char* did = root["did"];

  if(strcmp(did, deviceId) == 0) {
    int isIn = root["isin"];
    if (isIn == 1) {
      int d1 = root["state"][0];
      int d2 = root["state"][1];
      int d3 = root["state"][2];
      
      controlDevice(0, d1);
      controlDevice(2, d2);
      controlDevice(3, d3);
    }
  }
}

void controlDevice(int deviceId, int state){
  if(state == 1){
    lightTime = millis();
    digitalWrite(deviceId, HIGH);
  } else if(state == 0){
    digitalWrite(deviceId, LOW);
  }    
}

void loop()
{
  long now = millis();
  if (!client.connected()) {
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client.loop();
  }

  if (now - lightTime > 10000) {
      controlDevice(0, 0);
      controlDevice(2, 0);
      controlDevice(3, 0);
  }
}
