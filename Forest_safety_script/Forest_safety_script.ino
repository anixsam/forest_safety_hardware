#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "#################";    // Replace with your WiFi network name
const char* password = "#################";   // Replace with your WiFi network password
const char* webSocketServer = "192.168.1.8";   // Replace with your WebSocket server address
int webSocketPort = 8080;   // Replace with your WebSocket server port
int i = 0;
WiFiClient wifiClient;
WebSocketsClient webSocketClient;

StaticJsonDocument<200> jsonBuffer;
const int pirPin = 16;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  pinMode(pirPin,INPUT);
  webSocketClient.begin(webSocketServer, webSocketPort, "/");
  webSocketClient.onEvent(webSocketEvent);
}

void loop() {
  const int motionSensorValue = digitalRead(pirPin);
  String message;
  jsonBuffer.clear();
  jsonBuffer["device"] = "node";
  if(motionSensorValue == 1)
  {
    jsonBuffer["sensor"] = "motion";
    jsonBuffer["value"] = true;
    serializeJson(jsonBuffer,message);
    webSocketClient.sendTXT(message);
  }
  else 
  {
    jsonBuffer["sensor"] = "motion";
    jsonBuffer["value"] = false;
    serializeJson(jsonBuffer,message);
    webSocketClient.sendTXT(message);
  }
  webSocketClient.loop();
}

void webSocketEvent(WStype_t eventType, uint8_t * payload, size_t length) {
  switch (eventType) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      
      break;
    case WStype_TEXT:
      Serial.print("Received message: ");
      Serial.println((char *)payload);
      const char* msg = (char*)payload;
      if((char *)payload=="0")
        digitalWrite(LED_BUILTIN, HIGH);
      else
        digitalWrite(LED_BUILTIN,LOW);
      break;
  }
}