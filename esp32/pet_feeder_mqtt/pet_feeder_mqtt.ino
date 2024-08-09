#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "config.h"

// NTP Client Settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", GTMOffset * 60 * 60, 36000);

// Mqtt
WiFiClient espClient;
PubSubClient client(espClient);
const char* device_id = "PET_FEEDER_1923";
const char* feed_topic = "pet_feeder/feed";
const char* heart_beat_topic = "pet_feeder/heart_beat";

// Servo
Servo myservo;
int servoPin = 18;

// Heart-beat
unsigned long previousMillis = 0;

// Feeding
int last_feeding = 0;
int feeding_counter = 0;

void setup() {
  Serial.begin(115200);

  // Servo
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 1000, 2000);
  myservo.writeMicroseconds(1500);

  // Network
  connect_wifi();

  // NTP Client Start
  timeClient.begin();
}

void loop() {
  // Reconnect wifi
  wifi_reconnect();

  // NTP
  timeClient.update();

  // Mqtt client
  client.loop();

  // Publishing heart-beat
  heart_beat_timer();

  //
  check_feeding();
}


void connect_wifi() {
  delay(500);
  WiFi.disconnect();
  delay(500);
  unsigned long connectionCounter = 0;
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);
  while (WiFi.isConnected() == 0) {
    connectionCounter++;
    delay(500);
    Serial.print(".");
    if (connectionCounter >= 30) {
      Serial.println("Card restarting...");
      ESP.restart();
      connectionCounter = 0;
    }
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(5000);
  setup_mqtt();
  delay(2000);
  String success_message = "Device connected: " + WiFi.localIP().toString() + "  -  " + timeClient.getFormattedTime();
  client.publish("pet_feeder/status", success_message.c_str());
}

void wifi_reconnect() {
  if (int(WiFi.isConnected()) == 6) {
    strcpy(feeding_type, "periodical");
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    connect_wifi();
  }
}

// Mqtt methods
void setup_mqtt() {
  client.setServer(mqttServer, 1883);
  mqtt_reconnect();
  client.setCallback(mqtt_callback);
}

void mqtt_reconnect() {
  unsigned long currentMillis = millis();
  client.disconnect();
  delay(500);
  while (!client.connected()) {
    strcpy(feeding_type, "periodical");
    Serial.println("Attempting MQTT connection...");

    if (client.connect("PET_FEEDER")) {
      Serial.println("Connected");
      Serial.println("Subscribed pet_feeder/feed topic");
      client.subscribe("pet_feeder/feed");
      publish_heart_beat();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      if (!WiFi.isConnected() && (currentMillis - previousMillis >= 15000)) {
        setup_mqtt();
        wifi_reconnect();
      }
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

  // Feed
  if (strcmp(topic, feed_topic) == 0) {
    handle_feed(doc);
  }
}

// Servo methods
void rotateServo(int turn) {
  for (int i = 0; i < turn; i++) {
    feeding_counter++;
    Serial.println(turn);
    myservo.writeMicroseconds(2000);
    delay(150);
    myservo.writeMicroseconds(1000);
    delay(150);
    myservo.writeMicroseconds(1000);
    delay(350);
    myservo.writeMicroseconds(2000);
    delay(150);
    myservo.writeMicroseconds(1000);
    delay(150);
    myservo.writeMicroseconds(1500);
    delay(1000);
  }
}

// Feed method
void handle_feed(JsonDocument& doc) {
  String time = doc["time"] ? doc["time"] : timeClient.getFormattedTime();
  int amount = doc["amount"] ? doc["amount"] : default_mqtt_feeding_amount;

  rotateServo(amount);
  String response = "Feeding completed, " + String(amount) + " meal/s";
  Serial.println(response.c_str());
  client.publish("pet_feeder/status", response.c_str());
}

// Heart-beat method
void heart_beat_timer() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= (heart_beat_period * 60 * 1000)) {
    previousMillis = currentMillis;
    publish_heart_beat();
  }
}


void publish_heart_beat() {
  Serial.print("Pet feeder is live: ");
  Serial.println(timeClient.getFormattedTime());

  // Generate heart beat message
  JsonDocument message;
  message["command"] = "heart_beat";
  message["time"] = timeClient.getFormattedTime();
  char buffer[256];
  serializeJson(message, buffer);

  // Publish message to heart_beat topic
  client.publish(heart_beat_topic, buffer);
}

void check_feeding() {
  unsigned long currentMillis = millis();
  if ((strcmp(feeding_type, "periodical") == 0) || !client.connected()) {
    int period = feeding_period * 60 * 60 * 1000;
    if (currentMillis - last_feeding >= period) {
      Serial.println(currentMillis);
      Serial.println(last_feeding);
      Serial.println(period);
      last_feeding = currentMillis;
      rotateServo(default_periodical_feeding_amount);
    }
  }
}