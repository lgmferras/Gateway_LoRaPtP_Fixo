/*
 * Gateway_LoRaPtP_Fixo v1.3.1
 * Comandos AT+ para configuração
 */

#include "config.h"
#include "protocol.h"
#include "sensors.h"
#include "display_oled.h"
#include "lora_p2p.h"
#include "at_commands.h"
#include "wifi_mqtt.h"

// ===== VARIÁVEIS GLOBAIS =====
GatewayStats gwStats = {0, 0, 0, 0, 0, 0, 0};

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastStatsReport = 0;

// ===== SETUP =====
void setup() {
  Serial.begin(AT_SERIAL_BAUD);
  delay(2000);
  
  pinMode(GW_LED, OUTPUT);
  digitalWrite(GW_LED, LOW);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   Gateway LoRa P2P v" FW_VERSION "               ║");
  Serial.println("║   Build: " FW_DATE "                     ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Inicializar módulos
  initDisplay();
  initSensors();
  initLoRa();
  initWiFiMQTT();
  initATCommands();
  
  // Primeira leitura
  readSensors();
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
  
  // Atualizar display
  if (ms - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = ms;
  }
  
  // Estatísticas periódicas
  if (ms - lastStatsReport >= STATS_INTERVAL) {
    Serial.printf("\n📊 Stats: RX=%lu TX=%lu Uptime=%.1fmin\n",
                  gwStats.packetsReceived, gwStats.packetsSent, ms/60000.0);
    lastStatsReport = ms;
  }
  
  // Receber LoRa
  loraReceive();
  
  // Handle WiFi/MQTT
  handleWiFiMQTT();
  
  delay(10);
}
