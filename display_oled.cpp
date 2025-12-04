#include "display_oled.h"
#include "sensors.h"
#include "protocol.h"
#include "config.h"

// ===== OBJETO DISPLAY =====
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ===== VARIÁVEIS =====
NodeData lastNode = {{0}, false, 0, 0, 0};

// ===== INICIALIZAR =====
bool initDisplay() {
  u8g2.begin();
  displayWelcome();
  return true;
}

// ===== TELA INICIAL =====
void displayWelcome() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  u8g2.drawStr(10, 15, "Gateway LoRa P2P");
  u8g2.drawStr(25, 30, "915.4 MHz");
  u8g2.drawStr(20, 45, "Versao " FW_VERSION);
  
  u8g2.sendBuffer();
  delay(2000);
}

// ===== ATUALIZAR DISPLAY =====
void updateDisplay() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  char buf[32];
  
  // Linha 1: Info GW
  sprintf(buf, "GW 915.4 Rx:%lu", gwStats.packetsReceived);
  u8g2.drawStr(0, 10, buf);
  
  // Linha 2-3: Sensores GW
  sprintf(buf, "%.1fC %.0f%%", gwSensors.temperature, gwSensors.humidity);
  u8g2.drawStr(0, 22, buf);
  
  sprintf(buf, "%.0f hPa", gwSensors.pressure);
  u8g2.drawStr(0, 34, buf);
  
  // Linha 4-5: End-Node
  if (lastNode.valid && (millis() - lastNode.lastRx < NODE_TIMEOUT)) {
    sprintf(buf, "Node:%.1fC R:%d", 
            lastNode.payload.temp/100.0F, lastNode.rssi);
    u8g2.drawStr(0, 46, buf);
    
    sprintf(buf, "%.1fdB %.1fV", 
            lastNode.snr, lastNode.payload.bat/100.0F);
    u8g2.drawStr(0, 58, buf);
  } else {
    u8g2.drawStr(0, 46, "Node: OFFLINE");
  }
  
  u8g2.sendBuffer();
}
