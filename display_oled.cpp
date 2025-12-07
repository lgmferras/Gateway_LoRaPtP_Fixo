#include "display_oled.h"
#include "sensors.h"
#include "gps.h"
#include "protocol.h"
#include "config.h"

// ===== OBJETO DISPLAY =====
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ===== VARIÁVEIS =====
NodeData lastNode = {{0}, false, 0, 0, 0};
static uint8_t displayPage = 0;  // Página do display (0, 1, 2)
static unsigned long lastPageChange = 0;

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
  u8g2.drawStr(15, 58, "+ GPS Gateway");
  
  u8g2.sendBuffer();
  delay(3000);
}

// ===== ATUALIZAR DISPLAY (COM ROTAÇÃO) =====
void updateDisplay() {
  // Rotacionar página a cada 5 segundos
  if (millis() - lastPageChange > 5000) {
    displayPage = (displayPage + 1) % 3;  // 0, 1, 2
    lastPageChange = millis();
  }
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  char buf[32];
  
  switch (displayPage) {
    case 0:  // Página 1: Status Geral + Node
      // Linha 1: Info GW
      sprintf(buf, "GW 915.4 Rx:%lu", gwStats.packetsReceived);
      u8g2.drawStr(0, 10, buf);
      
      // Linha 2-3: Sensores GW
      sprintf(buf, "%.1fC %.0f%%", gwSensors.temperature, gwSensors.humidity);
      u8g2.drawStr(0, 22, buf);
      
      sprintf(buf, "%.0fhPa", gwSensors.pressure);
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
      break;
      
    case 1:  // Página 2: GPS Gateway
      u8g2.drawStr(0, 10, "GPS Gateway:");
      
      if (hasGPSFix()) {
        sprintf(buf, "%.6f", gwGPS.latitude);
        u8g2.drawStr(0, 22, buf);
        
        sprintf(buf, "%.6f", gwGPS.longitude);
        u8g2.drawStr(0, 34, buf);
        
        sprintf(buf, "Alt:%.0fm S:%u", gwGPS.altitude, gwGPS.satellites);
        u8g2.drawStr(0, 46, buf);
        
        sprintf(buf, "HDOP:%.1f %s", gwGPS.hdop, getGPSStatus().c_str());
        u8g2.drawStr(0, 58, buf);
      } else {
        u8g2.drawStr(0, 22, "Sem fix GPS");
        sprintf(buf, "Sats: %u", gwGPS.satellites);
        u8g2.drawStr(0, 34, buf);
        
        if (gwGPS.charsProcessed == 0) {
          u8g2.drawStr(0, 46, "Sem dados!");
          u8g2.drawStr(0, 58, "Checar ligacao");
        } else {
          u8g2.drawStr(0, 46, "Aguardando...");
          u8g2.drawStr(0, 58, "(Area aberta)");
        }
      }
      break;
      
    case 2:  // Página 3: Estatísticas
      u8g2.drawStr(0, 10, "Estatisticas:");
      
      sprintf(buf, "RX: %lu", gwStats.packetsReceived);
      u8g2.drawStr(0, 22, buf);
      
      sprintf(buf, "TX: %lu", gwStats.packetsSent);
      u8g2.drawStr(0, 34, buf);
      
      unsigned long uptimeSec = millis() / 1000;
      unsigned int hours = uptimeSec / 3600;
      unsigned int mins = (uptimeSec % 3600) / 60;
      sprintf(buf, "Uptime:%uh%um", hours, mins);
      u8g2.drawStr(0, 46, buf);
      
      sprintf(buf, "GPS:%s", hasGPSFix() ? "OK" : "NO FIX");
      u8g2.drawStr(0, 58, buf);
      break;
  }
  
  u8g2.sendBuffer();
}
