#include "gps.h"
#include "config.h"

// ===== OBJETOS =====
TinyGPSPlus gps;
HardwareSerial GPS_Serial(2);  // Serial2

// ===== VARIÁVEIS GLOBAIS =====
GPSData gwGPS = {0, 0, 0, 0, 0, 0, 0, false, 0, 0, 0, 0};

// ===== INICIALIZAR =====
bool initGPS() {
  #if DEBUG_GPS
  Serial.println("\n🛰️  Inicializando GPS...");
  #endif
  
  // Iniciar Serial2 (RX=16, TX=17)
  GPS_Serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  Serial.printf("   Porta: Serial2 (RX:%d, TX:%d)\n", GPS_RX_PIN, GPS_TX_PIN);
  Serial.printf("   Baud: %d\n", GPS_BAUD);
  Serial.println("   Aguardando fix GPS...");
  Serial.println("   (pode levar 30s-2min em ambiente aberto)");
  
  // Limpar buffer
  while(GPS_Serial.available()) {
    GPS_Serial.read();
  }
  
  return true;
}

// ===== ATUALIZAR DADOS GPS =====
void updateGPS() {
  // Processar dados disponíveis
  while (GPS_Serial.available() > 0) {
    char c = GPS_Serial.read();
    
    #if DEBUG_GPS_RAW
    Serial.write(c);  // Debug: mostrar NMEA bruto
    #endif
    
    if (gps.encode(c)) {
      gwGPS.charsProcessed++;
      
      // Atualizar dados se tiver fix
      if (gps.location.isValid()) {
        gwGPS.latitude = gps.location.lat();
        gwGPS.longitude = gps.location.lng();
        gwGPS.valid = true;
        gwGPS.lastUpdate = millis();
        gwGPS.sentencesWithFix++;
      }
      
      if (gps.altitude.isValid()) {
        gwGPS.altitude = gps.altitude.meters();
      }
      
      if (gps.satellites.isValid()) {
        gwGPS.satellites = gps.satellites.value();
      }
      
      if (gps.hdop.isValid()) {
        gwGPS.hdop = gps.hdop.hdop();
      }
      
      if (gps.speed.isValid()) {
        gwGPS.speed = gps.speed.kmph();
      }
      
      if (gps.course.isValid()) {
        gwGPS.course = gps.course.deg();
      }
    }
  }
  
  // Verificar timeout (30s sem atualização = sem fix)
  if (millis() - gwGPS.lastUpdate > GPS_TIMEOUT) {
    gwGPS.valid = false;
  }
  
  // Atualizar estatísticas de erro
  gwGPS.failedChecksum = gps.failedChecksum();
}

// ===== VERIFICAR FIX =====
bool hasGPSFix() {
  return gwGPS.valid && (millis() - gwGPS.lastUpdate < GPS_TIMEOUT);
}

// ===== STATUS GPS =====
String getGPSStatus() {
  if (!hasGPSFix()) {
    return "NO FIX";
  }
  
  if (gwGPS.satellites >= 6) {
    return "EXCELLENT";
  } else if (gwGPS.satellites >= 4) {
    return "GOOD";
  } else {
    return "WEAK";
  }
}

// ===== IMPRIMIR DADOS =====
void printGPSData() {
  Serial.println("\n📡 GPS Gateway:");
  
  if (!hasGPSFix()) {
    Serial.println("   ❌ Sem fix GPS");
    Serial.printf("   Satélites: %u\n", gwGPS.satellites);
    Serial.printf("   Chars processados: %lu\n", gwGPS.charsProcessed);
    Serial.printf("   Checksum falhos: %lu\n", gwGPS.failedChecksum);
    
    if (gwGPS.charsProcessed == 0) {
      Serial.println("   ⚠️  Nenhum dado recebido - verificar ligação!");
    } else if (gwGPS.satellites == 0) {
      Serial.println("   ⚠️  Nenhum satélite - mover para área aberta");
    }
    return;
  }
  
  Serial.println("   ✓ Fix GPS obtido!");
  Serial.printf("   Lat: %.6f°\n", gwGPS.latitude);
  Serial.printf("   Lon: %.6f°\n", gwGPS.longitude);
  Serial.printf("   Alt: %.1f m\n", gwGPS.altitude);
  Serial.printf("   Sats: %u | HDOP: %.1f\n", gwGPS.satellites, gwGPS.hdop);
  
  if (gwGPS.speed > 0.5) {  // Ignorar velocidades muito baixas (ruído)
    Serial.printf("   Velocidade: %.1f km/h\n", gwGPS.speed);
    Serial.printf("   Curso: %.0f°\n", gwGPS.course);
  }
  
  // Tempo desde última atualização
  unsigned long age = millis() - gwGPS.lastUpdate;
  Serial.printf("   Última atualização: %lu ms atrás\n", age);
  
  // Estatísticas
  Serial.printf("   Sentenças com fix: %lu\n", gwGPS.sentencesWithFix);
  
  // Coordenadas em formato Google Maps
  Serial.printf("   Google Maps: https://maps.google.com/?q=%.6f,%.6f\n",
                gwGPS.latitude, gwGPS.longitude);
}
