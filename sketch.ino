#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include <Wire.h>
#include <MQ2.h>

#define MQTT_USER_KEY        "D2ZBH079DEYG2P3B"
#define CHANNEL_ID           "1330804"
#define CHANNEL_API_KEY_WR   "G9B274K95Z4LOUZ0"

#define MQTT_UPDATE_INTERVAL 15000
#define MQTT_TOPIC           "channels/" CHANNEL_ID "/publish/" CHANNEL_API_KEY_WR
#define MQTT_USER_ID         "anyone" 
#define MQTT_SERVER          "mqtt.thingspeak.com"

AutoConnect  portal;
WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);

int lpg_gas, co_gas, smoke_gas;

bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 10;
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT broker:" MQTT_SERVER);

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, MQTT_USER_ID, MQTT_USER_KEY)) {
      Serial.println("Established:" + String(clientId));
      return true;
    } else {
      Serial.print("Connection failed:" + String(mqttClient.state()));
      if (!--retry)
        return false;
    }
    delay(3000);
  }
}

void mqttPublish(String msg) {
  String path = String(MQTT_TOPIC);
  int    tLen = path.length();
  char   topic[tLen];
  path.toCharArray(topic, tLen + 1);

  int    mLen = msg.length();
  char   payload[mLen];
  msg.toCharArray(payload, mLen + 1);

  mqttClient.publish(topic, payload);
}

unsigned long   lastPub = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Serial.print("WiFi ");
  if (portal.begin()) {
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
  } else {
    Serial.println("connection failed:" + String(WiFi.status()));
    while (1) {
      delay(100);
      yield();
    }
  }
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void loop() {
  if (millis() - lastPub > MQTT_UPDATE_INTERVAL) {
    if (!mqttClient.connected()) {
      mqttConnect();
    }
    String item = "&field1="+String(lpg_gas)+"&field2="+String(co_gas)+"&field3="+String(smoke_gas)+"\r\n\r\n"; 
    mqttPublish(item);
    mqttClient.loop();
    lastPub = millis();
  }
  portal.handleClient();
}
