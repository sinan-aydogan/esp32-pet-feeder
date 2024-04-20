#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "wifi_config.h"

// Mqtt
WiFiClient espClient;
PubSubClient client(espClient);

// Servo
Servo myservo; 
int servoPin = 18;

// Heart-beat
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);

  // Servo
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin,1000,2000);
  myservo.writeMicroseconds(1500);

  // Network
  connect_wifi();
}

void loop() {
  unsigned long currentMillis = millis();

  if (WiFi.isConnected()) {
    if (!client.connected()) {
      mqtt_reconnect();
    }
  }else{
    connect_wifi();
  }

  client.loop();

  if (currentMillis - previousMillis >= 10000) {
    previousMillis = currentMillis;  // Güncel zamanı kaydet
    client.publish("pet_feeder/beat", "live");  // MQTT mesajını gönder
  }
}


void connect_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  delay(1000);
  setup_mqtt();
}

// Mqtt methods
void setup_mqtt(){
  client.setServer(mqttServer, 1883);
  client.setCallback(mqtt_callback);
}

void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (client.connect("PET_FEEDER")) {
      Serial.println("Connected");
      Serial.println("Subscribed pet_feeder/feed topic");
      client.subscribe("pet_feeder/feed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // Deserialize payload
  JsonDocument doc;
  deserializeJson(doc, payload, length);

  // Heart beat
  if(strcmp(topic, "pet_feeder/beat") == 0){
    send_beat();
  }

  // Feed
  if(strcmp(topic, "pet_feeder/feed") == 0){
    handle_feed(doc);
  }
}

// Servo methods
void rotateServo(int turn, int frequence = 1000) {
  for(int i = 0; i < turn; i++){
    Serial.println(turn);
    myservo.writeMicroseconds(frequence);
    delay(300);
    myservo.writeMicroseconds(1500);
    delay(1000);
  }
}

// Feed method
void handle_feed(JsonDocument &doc) {
  String time = doc["time"];
  int amount = doc["amount"];
  int frequence = doc["frequence"];

  rotateServo(amount, frequence);
  String response = "Feeding completed, " + String(amount) + " meal/s";
  Serial.println(response.c_str());
  client.publish("pet_feeder/feed/reponse", response.c_str());
}

// Beat method
void send_beat(){
  client.publish("pet_feeder/beat", "live");
}