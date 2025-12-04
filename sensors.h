#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// ===== DADOS SENSORES =====
struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  bool valid;
  unsigned long lastRead;
};

extern SensorData gwSensors;

// ===== FUNÇÕES =====
bool initSensors();
void readSensors();
void printSensorData();

#endif
