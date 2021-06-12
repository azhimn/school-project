#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ThingSpeak.h>

int buzzer = D2;
int gasSensor = A0;

unsigned long channelNum = 1414766;
const char * writeAPI = "CBAAZZPTVRDPG7EK";

void setup() {
  Serial.begin(115200);
  
  pinMode(buzzer, OUTPUT);
  pinMode(gasSensor, INPUT);
  
  WiFiManager wifimanager;
  Serial.println("Connecting...");
  wifimanager.autoConnect("Alat Pendeteksi Asap", "12345678");
  Serial.println("Connected");
}

void loop() {
  int sensor = analogRead(gasSensor);
  
  Serial.print("Value: ");
  Serial.print(sensor);
  Serial.print("ppm");
  
  ThingSpeak.writeField(channelNum, 1, sensor, writeAPI);

  if (sensor > 300) {
    Serial.println("High consentration of gas detected!");
    digitalWrite (buzzer, HIGH);
    delay(1000);
    digitalWrite (buzzer, LOW);
  }
  else {
    Serial.println("No high consentration of gas detected.");
  }
}
