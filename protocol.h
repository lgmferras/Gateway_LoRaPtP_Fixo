#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

// ===== ESTRUTURA PAYLOAD (20 bytes) =====
struct __attribute__((packed)) PayloadStruct {
  int16_t temp;    // °C * 100
  uint16_t hum;    // % * 100
  uint16_t press;  // hPa
  int32_t lat;     // * 1e6
  int32_t lon;     // * 1e6
  uint16_t alt;    // metros
  uint8_t sats;    // satélites (255 = GW marker)
  uint16_t bat;    // V * 100
};

// ===== STATS GLOBAIS =====
struct GatewayStats {
  uint32_t packetsReceived;
  uint32_t packetsSent;
  uint32_t crcErrors;
  uint32_t uptime;
  int16_t lastRssi;
  float lastSnr;
  unsigned long lastRxTime;
};

extern GatewayStats gwStats;

#endif
