#include "sensors.h"
#include "config.h"

// ===== OBJETOS =====
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

// ===== VARIÁVEIS GLOBAIS =====
SensorData gwSensors = {0, 0, 0, false, 0};

// ===== INICIALIZAR =====
bool initSensors() {
  bool success = true;
  
  #if DEBUG_SENSORS
  Serial.println("\n🌡️  Inicializando sensores...");
  #endif
  
  // AHT20
  Serial.print("   AHT20... ");
  if (!aht.begin()) {
    Serial.println("❌");
    success = false;
  } else {
    Serial.println("✓");
  }
  
  // BMP280
  Serial.print("   BMP280... ");
  if (!bmp.begin(0x76)) {
    Serial.print("0x76 ❌ ");
    if (!bmp.begin(0x77)) {
      Serial.println("0x77 ❌");
      success = false;
    } else {
      Serial.println("0x77 ✓");
      goto config_bmp;
    }
  } else {
    Serial.println("0x76 ✓");
  }
  
config_bmp:
  if (success) {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
  }
  
  return success;
}

// ===== LER SENSORES =====
void readSensors() {
  sensors_event_t h, t;
  
  if (aht.getEvent(&h, &t)) {
    gwSensors.temperature = t.temperature;
    gwSensors.humidity = h.relative_humidity;
    gwSensors.valid = true;
  }
  
  gwSensors.pressure = bmp.readPressure() / 100.0F;
  gwSensors.lastRead = millis();
  
  #if DEBUG_SENSORS
  printSensorData();
  #endif
}

// ===== IMPRIMIR =====
void printSensorData() {
  Serial.println("\n📊 Sensores Gateway:");
  Serial.printf("   Temp: %.1f°C\n", gwSensors.temperature);
  Serial.printf("   Umid: %.1f%%\n", gwSensors.humidity);
  Serial.printf("   Press: %.1f hPa\n", gwSensors.pressure);
}

