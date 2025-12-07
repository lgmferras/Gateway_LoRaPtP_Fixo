#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>

// ===== ESTRUTURA DADOS GPS =====
struct GPSData {
  double latitude;
  double longitude;
  double altitude;
  uint8_t satellites;
  double hdop;           // Horizontal Dilution of Precision
  double speed;          // km/h
  double course;         // graus (0-360)
  bool valid;
  unsigned long lastUpdate;
  uint32_t charsProcessed;
  uint32_t sentencesWithFix;
  uint32_t failedChecksum;
};

extern GPSData gwGPS;
extern TinyGPSPlus gps;
extern HardwareSerial GPS_Serial;  // ← ADICIONADO: Tornar visível externamente

// ===== FUNÇÕES =====
bool initGPS();
void updateGPS();
void printGPSData();
bool hasGPSFix();
String getGPSStatus();

#endif