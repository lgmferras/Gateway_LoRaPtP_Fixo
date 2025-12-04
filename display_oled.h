#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "protocol.h"

// ===== DADOS END-NODE =====
struct NodeData {
  PayloadStruct payload;
  bool valid;
  unsigned long lastRx;
  int16_t rssi;
  float snr;
};

extern NodeData lastNode;

// ===== FUNÇÕES =====
bool initDisplay();
void updateDisplay();
void displayWelcome();

#endif
