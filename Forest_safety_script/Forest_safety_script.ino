#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Iot test network";    // Replace with your WiFi network name
const char* password = "test@123";   // Replace with your WiFi network password
const char* webSocketServer = "192.168.1.3";   // Replace with your WebSocket server address

int webSocketPort = 8080;   // Replace with your WebSocket server port

WiFiClient wifiClient;
WebSocketsClient webSocketClient;

const int pirPin = 16;
const int flamePin = 5;
const int buzzerPin = 4;
const int ledPin = 0;
const int fencePin = 14;

boolean motionFlag = false;
boolean flameFlag = false;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");
  
  pinMode(flamePin,INPUT);
  pinMode(pirPin,INPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(fencePin,OUTPUT);
  
  webSocketClient.begin(webSocketServer, webSocketPort, "/");
  webSocketClient.onEvent(webSocketEvent);
}

void loop() {
  const int motionSensorValue = digitalRead(pirPin);
  const int flameSensorValue = digitalRead(flamePin);
  Serial.println("Motion Sensor : " + String(motionSensorValue) + " Flame Sensor : " + String(flameSensorValue));
    
  if(flameSensorValue == 0)
  {
    if(!flameFlag)
    {
      flameFlag = true;
      StaticJsonDocument<200> jsonMessage;
      jsonMessage.clear();
      jsonMessage["from"] = "device";
      String message;
      jsonMessage["sensor"] = "fire";
      jsonMessage["value"] = true;
      serializeJson(jsonMessage,message);
      webSocketClient.sendTXT(message);    
      Serial.println("Fire Detected");
    }
  }
  else
  {
    if(flameFlag)
    {
      flameFlag = false;
      StaticJsonDocument<200> jsonMessage;
      jsonMessage.clear();
      jsonMessage["from"] = "device";
      String message;
      jsonMessage["sensor"] = "fire";
      jsonMessage["value"] = false;
      serializeJson(jsonMessage,message);
      webSocketClient.sendTXT(message); 
      Serial.println("Fire Not Detected");
    } 
  }
  if(motionSensorValue == 1)
  {
    if(!motionFlag)
    {
      motionFlag = true;
      StaticJsonDocument<200> jsonMessage;
      jsonMessage.clear();
      jsonMessage["from"] = "device";
      String message;
      jsonMessage["sensor"] = "motion";
      jsonMessage["value"] = true;
      serializeJson(jsonMessage,message);
      webSocketClient.sendTXT(message); 
      Serial.println("Motion Detected");
    }
  }
  else 
  {
    if(motionFlag)
    {
      motionFlag = false;
      StaticJsonDocument<200> jsonMessage;
      jsonMessage.clear();
      jsonMessage["from"] = "device";
      String message;
      jsonMessage["sensor"] = "motion";
      jsonMessage["value"] = false;
      serializeJson(jsonMessage,message);
      webSocketClient.sendTXT(message);
      Serial.println("Motion Not Detected");
    }
  }
  webSocketClient.loop();
}

void webSocketEvent(WStype_t eventType, uint8_t * payload, size_t length) {
  switch (eventType) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("websocket connected");
      break;
    case WStype_TEXT:
      String msg = String((char *) payload);
      Serial.println("Received message: ");
      Serial.print(msg);

      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, msg);
      
      if (error) 
      {
        Serial.print("Failed to deserialize JSON: ");
        Serial.println(error.c_str());
        return;
      }

      if (doc.containsKey("id"))
      {
        String type = doc["id"];
        Serial.print("Type : ");
          Serial.println(type);
        if(type == "motion")
        {
          handleMotionMessage(doc);
        }
        else if(type == "fire")
        {
          handleFireMessage(doc);
        }
        else if(type == "normal")
        {
          handleNormalMessage(doc);
        }
      }
      break;
  }
}


void handleNormalMessage(StaticJsonDocument<1024> msg)
{
  digitalWrite(buzzerPin,LOW);
  digitalWrite(fencePin,LOW);
  digitalWrite(ledPin,LOW);
}

void handleMotionMessage(StaticJsonDocument<1024> msg)
{
  digitalWrite(ledPin,HIGH);
  digitalWrite(fencePin,HIGH);
}

void handleFireMessage(StaticJsonDocument<1024> msg)
{
  digitalWrite(buzzerPin,HIGH);
  digitalWrite(ledPin,HIGH);
}
