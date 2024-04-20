#include "WiFi.h"
#include "wifi_config.h"

void setup() {
  Serial.begin(9600);
  connect_wifi();
}

void loop() {
  // put your main code here, to run repeatedly:

}


void connect_wifi(){
  WiFi.disconnect(true);

  //Wifi events
  WiFi.onEvent(ConnectedToAP_Handler, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(GotIP_Handler, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFi_Disconnected_Handler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Wifi connection
  int limitcounter = 0;
  Serial.println("Connecting to WiFi Network ..");
  WiFi.begin(ssid, password);

  while( (WiFi.status() != WL_CONNECTED) && (limitcounter < 30)) {
    Serial.print(".");
    limitcounter++;
    if(limitcounter>=30){
      ESP.restart();
    }
    delay(500);
  }
  
}

void ConnectedToAP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Connected To The WiFi Network");
}
 
void GotIP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}
 
void WiFi_Disconnected_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Disconnected From WiFi Network");
  // Attempt Re-Connection
  WiFi.begin(ssid, password);
}