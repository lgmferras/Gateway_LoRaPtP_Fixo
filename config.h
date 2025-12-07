#ifndef CONFIG_H
#define CONFIG_H

// ===== VERSÃO =====
#define FW_VERSION "1.4"
#define FW_DATE "2025-12-07"

// ===== PINOS LoRa SX1276 =====
#define LORA_SCK 12
#define LORA_MISO 13
#define LORA_MOSI 11
#define LORA_SS 10
#define LORA_RST 14
#define LORA_DIO0 4

// ===== PINOS GPS GY-GPS6MV2 =====
#define GPS_RX_PIN 16   // ESP32 RX2 ← GPS TX
#define GPS_TX_PIN 17   // ESP32 TX2 → GPS RX
#define GPS_BAUD 9600   // Padrão do NEO-6M

// ===== PINOS GERAIS =====
#define GW_LED 2

// ===== CONFIG LORA P2P =====
#define DEFAULT_FREQ 915400000  // 915.4 MHz
#define DEFAULT_SF 10
#define DEFAULT_BW 125E3
#define DEFAULT_CR 5
#define DEFAULT_SYNC 0x34
#define DEFAULT_PREAMBLE 8
#define DEFAULT_TX_POWER 14

// ===== INTERVALOS (ms) =====
#define SENSOR_READ_INTERVAL 10000   // 10s
#define GPS_UPDATE_INTERVAL 1000     // 1s
#define DISPLAY_UPDATE_INTERVAL 2000  // 2s
#define STATS_INTERVAL 60000          // 60s

// ===== TIMEOUTS =====
#define GPS_TIMEOUT 30000     // 30s sem atualização = sem fix
#define NODE_TIMEOUT 30000    // 30s sem pacote = offline

// ===== LIMITES =====
#define MAX_PACKET_SIZE 256
#define PAYLOAD_SIZE 20

// ===== DEBUG =====
#define DEBUG_SERIAL 1
#define DEBUG_LORA 1
#define DEBUG_SENSORS 1
#define DEBUG_GPS 1
#define DEBUG_GPS_RAW 0      // 0=off, 1=mostrar NMEA bruto
#define DEBUG_MQTT 1

// ===== AT COMMANDS =====
#define AT_CMD_ENABLED 1
#define AT_SERIAL_BAUD 115200

// ===== MQTT/WiFi =====
#define MQTT_BUFFER_SIZE 1024

#endif
