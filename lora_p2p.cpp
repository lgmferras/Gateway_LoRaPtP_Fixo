#include "lora_p2p.h"
#include "config.h"
#include "sensors.h"
#include "display_oled.h"
#include "wifi_mqtt.h"

// ===== VARIÁVEIS LOCAIS =====
static long currentFreq = DEFAULT_FREQ;
static int currentSF = DEFAULT_SF;
static long currentBW = DEFAULT_BW;

// ===== INICIALIZAR =====
bool initLoRa() {
  Serial.println("\n📡 Inicializando LoRa...");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(currentFreq)) {
    Serial.println("   ❌ FALHA!");
    return false;
  }

  LoRa.setSpreadingFactor(currentSF);
  LoRa.setSignalBandwidth(currentBW);
  LoRa.setCodingRate4(DEFAULT_CR);
  LoRa.setSyncWord(DEFAULT_SYNC);
  LoRa.setPreambleLength(DEFAULT_PREAMBLE);
  LoRa.setTxPower(DEFAULT_TX_POWER);
  LoRa.enableCrc();
  LoRa.enableInvertIQ();

  LoRa.receive();

  Serial.println("   ✓ OK");
  loraPrintConfig();

  return true;
}

// ===== RECEBER PACOTES =====
void loraReceive() {
  int size = LoRa.parsePacket();
  if (size <= 0) return;

  // Ler dados
  uint8_t buf[MAX_PACKET_SIZE];
  int len = 0;
  while (LoRa.available() && len < MAX_PACKET_SIZE) {
    buf[len++] = LoRa.read();
  }

  int rssi = LoRa.packetRssi();
  float snr = LoRa.packetSnr();

  gwStats.packetsReceived++;
  gwStats.lastRssi = rssi;
  gwStats.lastSnr = snr;
  gwStats.lastRxTime = millis();

#if DEBUG_LORA
  Serial.printf("\n📦 RX: %d bytes | RSSI:%ddBm | SNR:%.1fdB\n", len, rssi, snr);
  Serial.print("Hex: ");
  for (int i = 0; i < min(32, len); i++) {
    Serial.printf("%02X ", buf[i]);
  }
  if (len > 32) Serial.print("...");
  Serial.println();
#endif

  // Decodificar payload
  if (len == sizeof(PayloadStruct)) {
    memcpy(&lastNode.payload, buf, sizeof(PayloadStruct));
    lastNode.valid = true;
    lastNode.lastRx = millis();
    lastNode.rssi = rssi;
    lastNode.snr = snr;

    Serial.println("\n┌─ END-NODE DECODIFICADO ─────────────────┐");
    Serial.printf("│ Temp: %.1f°C | Hum: %.0f%%\n",
                  lastNode.payload.temp / 100.0F,
                  lastNode.payload.hum / 100.0F);
    Serial.printf("│ GPS: %.6f, %.6f | Sats: %u\n",
                  lastNode.payload.lat / 1e6F,
                  lastNode.payload.lon / 1e6F,
                  lastNode.payload.sats);
    Serial.printf("│ Bat: %.1fV | RSSI: %ddBm\n",
                  lastNode.payload.bat / 100.0F, rssi);
    Serial.println("└──────────────────────────────────────────┘");

    // Enviar ACK
    delay(200);
    loraSendAck();
    // Publicar no MQTT se conectado
    if (isMQTTConnected()) {
      char jsonPayload[1024];

      // Calcular uptime
      unsigned long uptimeSeconds = millis() / 1000;
      unsigned int hours = uptimeSeconds / 3600;
      unsigned int minutes = (uptimeSeconds % 3600) / 60;
      unsigned int seconds = uptimeSeconds % 60;

      snprintf(jsonPayload, sizeof(jsonPayload),
               "{"
               "\"device\":\"ESP32_GW_%08X\","
               "\"fw_version\":\"" FW_VERSION "\","
               "\"timestamp\":%lu,"
               "\"uptime\":\"%02u:%02u:%02u\","
               "\"node\":{"
               "\"temp\":%.1f,"
               "\"hum\":%.0f,"
               "\"press\":%.0f,"
               "\"gps\":{"
               "\"lat\":%.6f,"
               "\"lon\":%.6f,"
               "\"alt\":%u,"
               "\"sats\":%u"
               "},"
               "\"bat\":%.2f"
               "},"
               "\"gateway\":{"
               "\"temp\":%.1f,"
               "\"hum\":%.1f,"
               "\"press\":%.1f,"
               "\"wifi_rssi\":%d"
               "},"
               "\"lora\":{"
               "\"freq\":%.1f,"
               "\"sf\":%d,"
               "\"rssi\":%d,"
               "\"snr\":%.1f"
               "},"
               "\"stats\":{"
               "\"rx\":%lu,"
               "\"tx\":%lu"
               "}"
               "}",
               // Device info
               (uint32_t)ESP.getEfuseMac(),
               millis() / 1000,
               hours, minutes, seconds,
               // Node data
               lastNode.payload.temp / 100.0F,
               lastNode.payload.hum / 100.0F,
               (float)lastNode.payload.press,
               lastNode.payload.lat / 1e6F,
               lastNode.payload.lon / 1e6F,
               lastNode.payload.alt,
               lastNode.payload.sats,
               lastNode.payload.bat / 100.0F,
               // Gateway sensors
               gwSensors.temperature,
               gwSensors.humidity,
               gwSensors.pressure,
               WiFi.RSSI(),
               // LoRa info
               DEFAULT_FREQ / 1e6,
               DEFAULT_SF,
               rssi,
               snr,
               // Stats
               gwStats.packetsReceived,
               gwStats.packetsSent);

      publishLoRaData(jsonPayload);
    }
  }
}

// ===== ENVIAR ACK =====
bool loraSendAck() {
  PayloadStruct gwPkt;
  gwPkt.temp = (int16_t)(gwSensors.temperature * 100);
  gwPkt.hum = (uint16_t)(gwSensors.humidity * 100);
  gwPkt.press = (uint16_t)gwSensors.pressure;
  gwPkt.lat = 0;
  gwPkt.lon = 0;
  gwPkt.alt = 0;
  gwPkt.sats = 255;  // GW marker
  gwPkt.bat = 0;

  LoRa.idle();
  LoRa.beginPacket();
  LoRa.write((uint8_t*)&gwPkt, sizeof(gwPkt));
  bool ok = LoRa.endPacket();

  if (ok) {
    gwStats.packetsSent++;
    digitalWrite(GW_LED, HIGH);
    delay(100);
    digitalWrite(GW_LED, LOW);
  }

  Serial.printf("📤 ACK→Node: %s\n", ok ? "✓" : "✗");

  LoRa.receive();
  return ok;
}

// ===== CONFIGURAÇÕES DINÂMICAS =====
void loraSetFrequency(long freq) {
  currentFreq = freq;
  LoRa.setFrequency(freq);
  Serial.printf("LoRa Freq: %.1f MHz\n", freq / 1e6);
}

void loraSetSpreadingFactor(int sf) {
  if (sf < 7 || sf > 12) return;
  currentSF = sf;
  LoRa.setSpreadingFactor(sf);
  Serial.printf("LoRa SF: %d\n", sf);
}

void loraSetBandwidth(long bw) {
  currentBW = bw;
  LoRa.setSignalBandwidth(bw);
  Serial.printf("LoRa BW: %.0f kHz\n", bw / 1e3);
}

void loraSetTxPower(int power) {
  LoRa.setTxPower(power);
  Serial.printf("LoRa TxPwr: %d dBm\n", power);
}

void loraPrintConfig() {
  Serial.println("   Config:");
  Serial.printf("   Freq: %.1f MHz\n", currentFreq / 1e6);
  Serial.printf("   SF: %d | BW: %.0f kHz\n", currentSF, currentBW / 1e3);
  Serial.printf("   CR: 4/%d | Sync: 0x%02X\n", DEFAULT_CR, DEFAULT_SYNC);
}
