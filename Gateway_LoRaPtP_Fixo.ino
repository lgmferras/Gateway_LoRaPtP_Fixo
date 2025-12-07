/*
 * Gateway_LoRaPtP_Fixo v1.4
 * Estrutura modular profissional
 * Comandos AT+ para configuração
 * + GPS no Gateway
 */

#include "config.h"
#include "protocol.h"
#include "sensors.h"
#include "gps.h"
#include "display_oled.h"
#include "lora_p2p.h"
#include "at_commands.h"
#include "wifi_mqtt.h"

// ===== VARIÁVEIS GLOBAIS =====
GatewayStats gwStats = {0, 0, 0, 0, 0, 0, 0};

unsigned long lastSensorRead = 0;
unsigned long lastGPSUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastStatsReport = 0;

// ===== SETUP =====
void setup() {
  Serial.begin(AT_SERIAL_BAUD);
  delay(2000);
  
  pinMode(GW_LED, OUTPUT);
  digitalWrite(GW_LED, LOW);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   Gateway LoRa P2P v" FW_VERSION "       ║");
  Serial.println("║   Build: " FW_DATE "                     ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Inicializar módulos
  initDisplay();
  initSensors();
  initGPS();          // ← NOVO
  initLoRa();
  initWiFiMQTT();
  initATCommands();
  
  // Primeira leitura
  readSensors();
  updateGPS();        // ← NOVO
  updateDisplay();
  
  Serial.println("\n✅ Gateway pronto!");
  Serial.println("═══════════════════════════════════════════════\n");
}

// ===== LOOP =====
void loop() {
  unsigned long ms = millis();
  
  // Processar comandos AT+
  processATCommands();
  
  // Ler sensores
  if (ms - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readSensors();
    lastSensorRead = ms;
  }
  
  // Atualizar GPS
  if (ms - lastGPSUpdate >= GPS_UPDATE_INTERVAL) {
    updateGPS();
    lastGPSUpdate = ms;
  }
  
  // Atualizar display
  if (ms - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = ms;
  }
  
  // Estatísticas periódicas
  if (ms - lastStatsReport >= STATS_INTERVAL) {
    Serial.printf("\n📊 Stats: RX=%lu TX=%lu GPS=%s Uptime=%.1fmin\n",
                  gwStats.packetsReceived, gwStats.packetsSent,
                  hasGPSFix() ? "OK" : "NO", ms/60000.0);
    lastStatsReport = ms;
  }
  
  // Receber LoRa
  loraReceive();
  
  // Handle WiFi/MQTT
  handleWiFiMQTT();
  
  // GPS continua sendo atualizado no background (updateGPS processa serial)
  updateGPS();
  
  delay(10);
}
