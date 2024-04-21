#ifndef CONFIG_H
#define CONFIG_H

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_SSID_PASSWORD";
const char *mqttServer = "YOUR_MQTT_SERVER_IP";
int GTMOffset = 3; // Timezone
char feeding_type[] = "periodical";
int feeding_period = 5; // Once a every 5 hours
int default_periodical_feeding_amount = 1;
int default_mqtt_feeding_amount = 1;

#endif