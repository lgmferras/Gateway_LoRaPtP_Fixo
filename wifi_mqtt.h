#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

// ===== ESTRUTURA CONFIGURAÇÃO =====
struct WiFiMQTTConfig {
  char ssid[32];
  char password[64];
  char mqttServer[64];
  uint16_t mqttPort;
  char mqttUser[32];
  char mqttPass[32];
  char topicPub[64];
  char topicSub[64];
  bool enabled;
};

extern WiFiMQTTConfig wifiMqttCfg;
extern WiFiClient espClient;
extern PubSubClient mqttClient;

// ===== FUNÇÕES =====
bool initWiFiMQTT();
void loadWiFiMQTTConfig();
void saveWiFiMQTTConfig();
void connectWiFi();
void connectMQTT();
void handleWiFiMQTT();
bool isWiFiConnected();
bool isMQTTConnected();
void publishLoRaData(const char* payload);
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif
