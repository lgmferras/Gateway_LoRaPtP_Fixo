#include "wifi_mqtt.h"
#include "config.h"
#include "protocol.h"

WiFiMQTTConfig wifiMqttCfg;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
Preferences prefs;

static unsigned long lastWiFiCheck = 0;
static unsigned long lastMQTTCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;  // 10s
const unsigned long MQTT_CHECK_INTERVAL = 5000;   // 5s

// ===== INICIALIZAR =====
bool initWiFiMQTT() {
  Serial.println("\n📶 Inicializando WiFi/MQTT...");

  // Zerar estrutura antes de carregar
  memset(&wifiMqttCfg, 0, sizeof(wifiMqttCfg));
  wifiMqttCfg.mqttPort = 1883;

  loadWiFiMQTTConfig();

  if (!wifiMqttCfg.enabled) {
    Serial.println("   ⚠️  WiFi/MQTT desabilitado");
    return false;
  }

  if (strlen(wifiMqttCfg.ssid) == 0) {
    Serial.println("   ⚠️  SSID não configurado");
    return false;
  }

  WiFi.disconnect(true, true);  // Desconecta e apaga credenciais antigas
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  delay(100);

  connectWiFi();

  if (strlen(wifiMqttCfg.mqttServer) > 0 && WiFi.status() == WL_CONNECTED) {
    mqttClient.setServer(wifiMqttCfg.mqttServer, wifiMqttCfg.mqttPort);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
    connectMQTT();
  }
  
  return WiFi.status() == WL_CONNECTED;

}

// ===== CARREGAR CONFIG DA NVS =====
void loadWiFiMQTTConfig() {
  prefs.begin("wifimqtt", true);  // read-only

  prefs.getString("ssid", wifiMqttCfg.ssid, sizeof(wifiMqttCfg.ssid));
  prefs.getString("pass", wifiMqttCfg.password, sizeof(wifiMqttCfg.password));
  prefs.getString("mqtt_srv", wifiMqttCfg.mqttServer, sizeof(wifiMqttCfg.mqttServer));
  wifiMqttCfg.mqttPort = prefs.getUShort("mqtt_port", 1883);
  prefs.getString("mqtt_user", wifiMqttCfg.mqttUser, sizeof(wifiMqttCfg.mqttUser));
  prefs.getString("mqtt_pass", wifiMqttCfg.mqttPass, sizeof(wifiMqttCfg.mqttPass));
  prefs.getString("topic_pub", wifiMqttCfg.topicPub, sizeof(wifiMqttCfg.topicPub));
  prefs.getString("topic_sub", wifiMqttCfg.topicSub, sizeof(wifiMqttCfg.topicSub));
  wifiMqttCfg.enabled = prefs.getBool("enabled", false);

  prefs.end();

  Serial.println("   Config carregada da NVS");
  if (wifiMqttCfg.enabled && strlen(wifiMqttCfg.ssid) > 0) {
    Serial.printf("   SSID: %s\n", wifiMqttCfg.ssid);
    Serial.printf("   MQTT: %s:%d\n", wifiMqttCfg.mqttServer, wifiMqttCfg.mqttPort);
  }
}

// ===== SALVAR CONFIG NA NVS =====
void saveWiFiMQTTConfig() {
  prefs.begin("wifimqtt", false);  // read-write

  prefs.putString("ssid", wifiMqttCfg.ssid);
  prefs.putString("pass", wifiMqttCfg.password);
  prefs.putString("mqtt_srv", wifiMqttCfg.mqttServer);
  prefs.putUShort("mqtt_port", wifiMqttCfg.mqttPort);
  prefs.putString("mqtt_user", wifiMqttCfg.mqttUser);
  prefs.putString("mqtt_pass", wifiMqttCfg.mqttPass);
  prefs.putString("topic_pub", wifiMqttCfg.topicPub);
  prefs.putString("topic_sub", wifiMqttCfg.topicSub);
  prefs.putBool("enabled", wifiMqttCfg.enabled);

  prefs.end();

  Serial.println("✓ Config salva na NVS");
}

// ===== CONECTAR WIFI =====
void connectWiFi() {
  if (strlen(wifiMqttCfg.ssid) == 0) return;
  
  // Verificar se já está conectado
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  
  Serial.printf("📶 Conectando WiFi: %s", wifiMqttCfg.ssid);
  
  // Iniciar conexão
  WiFi.begin(wifiMqttCfg.ssid, wifiMqttCfg.password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" ✓");
    Serial.printf("   IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("   RSSI: %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println(" ✗");
    Serial.printf("   Status: %d\n", WiFi.status());
  }
}


// ===== CONECTAR MQTT =====
void connectMQTT() {
  if (strlen(wifiMqttCfg.mqttServer) == 0) return;
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.printf("📡 Conectando MQTT: %s:%d", wifiMqttCfg.mqttServer, wifiMqttCfg.mqttPort);

  String clientId = "ESP32_GW_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool connected = false;
  if (strlen(wifiMqttCfg.mqttUser) > 0) {
    connected = mqttClient.connect(clientId.c_str(),
                                   wifiMqttCfg.mqttUser,
                                   wifiMqttCfg.mqttPass);
  } else {
    connected = mqttClient.connect(clientId.c_str());
  }

  if (connected) {
    Serial.println(" ✓");

    // Subscribe no tópico configurado
    if (strlen(wifiMqttCfg.topicSub) > 0) {
      mqttClient.subscribe(wifiMqttCfg.topicSub);
      Serial.printf("   Subscribed: %s\n", wifiMqttCfg.topicSub);
    }
  } else {
    Serial.println(" ✗");
    Serial.printf("   Erro MQTT: %d\n", mqttClient.state());
  }
}

// ===== HANDLE PERIÓDICO =====
void handleWiFiMQTT() {
  if (!wifiMqttCfg.enabled) return;
  
  unsigned long now = millis();
  
  // Check WiFi a cada 30s
  if (now - lastWiFiCheck > 30000) {
    lastWiFiCheck = now;
    
    if (WiFi.status() != WL_CONNECTED && strlen(wifiMqttCfg.ssid) > 0) {
      Serial.println("⚠️  WiFi desconectado, reconectando...");
      WiFi.reconnect();  // Usa reconnect em vez de begin novamente
    }
  }
  
  // Check MQTT a cada 5s
  if (now - lastMQTTCheck > MQTT_CHECK_INTERVAL) {
    lastMQTTCheck = now;
    
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
      if (strlen(wifiMqttCfg.mqttServer) > 0) {
        connectMQTT();
      }
    }
  }
  
  // Loop MQTT
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
}

// ===== STATUS =====
bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool isMQTTConnected() {
  return mqttClient.connected();
}

// ===== PUBLICAR DADOS LORA =====
void publishLoRaData(const char* payload) {
  if (!mqttClient.connected()) return;
  if (strlen(wifiMqttCfg.topicPub) == 0) return;

  bool ok = mqttClient.publish(wifiMqttCfg.topicPub, payload);

#if DEBUG_MQTT
  Serial.printf("📤 MQTT→ %s: %s %s\n",
                wifiMqttCfg.topicPub,
                payload,
                ok ? "✓" : "✗");
#endif
}

// ===== CALLBACK MQTT =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\n📥 MQTT← %s: ", topic);

  char msg[256];
  int len = min((int)length, 255);
  memcpy(msg, payload, len);
  msg[len] = '\0';

  Serial.println(msg);

  // Aqui você pode processar comandos recebidos via MQTT
  // Exemplo: controlar o gateway remotamente
}
