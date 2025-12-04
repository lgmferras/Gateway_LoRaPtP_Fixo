#ifndef LORA_P2P_H
#define LORA_P2P_H

#include <Arduino.h>
#include <LoRa.h>
#include "protocol.h"

// ===== FUNÇÕES =====
bool initLoRa();
void loraReceive();
bool loraSendAck();
void loraSetFrequency(long freq);
void loraSetSpreadingFactor(int sf);
void loraSetBandwidth(long bw);
void loraSetTxPower(int power);
void loraPrintConfig();

#endif
