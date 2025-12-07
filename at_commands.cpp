#include "at_commands.h"
#include "config.h"
#include "lora_p2p.h"
#include "sensors.h"
#include "gps.h"
#include "protocol.h"
#include "wifi_mqtt.h"

static String atBuffer = "";

void initATCommands() {
  Serial.println("\n📟 Comandos AT+ habilitados");
  Serial.println("   Digite 'AT+HELP' para lista completa\n");
}

void printATHelp() {
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║            COMANDOS AT+ GATEWAY v" FW_VERSION "             ║");
  Serial.println("╠════════════════════════════════════════════════════╣");
  Serial.println("║ INFORMAÇÕES                                        ║");
  Serial.println("║ AT+VER          - Versão firmware                  ║");
  Serial.println("║ AT+STATUS       - Status do gateway                ║");
  Serial.println("║ AT+SENSORS      - Ler sensores                     ║");
  Serial.println("║ AT+GPS          - Dados GPS do gateway             ║");
  Serial.println("║ AT+GPSRAW       - NMEA sentences (debug)           ║");
  Serial.println("║ AT+STATS        - Estatísticas LoRa                ║");
  Serial.println("╠════════════════════════════════════════════════════╣");
  Serial.println("║ CONFIGURAÇÃO LoRa                                  ║");
  Serial.println("║ AT+FREQ=<MHz>   - Mudar frequência (915.2-928.0)   ║");
  Serial.println("║ AT+SF=<n>       - Spreading Factor (7-12)          ║");
  Serial.println("║ AT+BW=<kHz>     - Bandwidth (125/250/500)          ║");
  Serial.println("║ AT+PWR=<dBm>    - TX Power (2-20)                  ║");
  Serial.println("╠════════════════════════════════════════════════════╣");
  Serial.println("║ WiFi/MQTT                                          ║");
  Serial.println("║ AT+WSSID=<ssid>      - Configurar SSID WiFi        ║");
  Serial.println("║ AT+WPASS=<pass>      - Configurar senha WiFi       ║");
  Serial.println("║ AT+MQSRV=<server>    - Servidor MQTT               ║");
  Serial.println("║ AT+MQPORT=<port>     - Porta MQTT (padrão 1883)    ║");
  Serial.println("║ AT+MQUSER=<user>     - Usuário MQTT                ║");
  Serial.println("║ AT+MQPASS=<pass>     - Senha MQTT                  ║");
  Serial.println("║ AT+MQPUB=<topic>     - Tópico Publish              ║");
  Serial.println("║ AT+MQSUB=<topic>     - Tópico Subscribe            ║");
  Serial.println("║ AT+WIFION           - Habilitar WiFi/MQTT          ║");
  Serial.println("║ AT+WIFIOFF          - Desabilitar WiFi/MQTT        ║");
  Serial.println("║ AT+WSTATUS          - Status WiFi/MQTT             ║");
  Serial.println("╠════════════════════════════════════════════════════╣");
  Serial.println("║ SISTEMA                                            ║");
  Serial.println("║ AT+RESET        - Reiniciar gateway                ║");
  Serial.println("║ AT+HELP         - Esta ajuda                       ║");
  Serial.println("╚════════════════════════════════════════════════════╝\n");
}


void processATCommands() {
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      atBuffer.trim();
      
      if (atBuffer.length() > 0) {
        // Guardar versão original para comandos com valores case-sensitive
        String originalBuffer = atBuffer;
        
        // Versão uppercase só para comparação de comandos
        String cmdUpper = atBuffer;
        cmdUpper.toUpperCase();
        
        // Processar comando
        if (cmdUpper == "AT+HELP" || cmdUpper == "AT+?") {
          printATHelp();
          
        } else if (cmdUpper == "AT+VER") {
          Serial.printf("Firmware: v%s (%s)\n", FW_VERSION, FW_DATE);
          
        } else if (cmdUpper == "AT+STATUS") {
          Serial.println("\n📊 Status Gateway:");
          Serial.printf("   Uptime: %lu s\n", millis()/1000);
          Serial.printf("   Packets RX: %lu | TX: %lu\n", 
                        gwStats.packetsReceived, gwStats.packetsSent);
          Serial.printf("   Last RSSI: %d dBm | SNR: %.1f dB\n",
                        gwStats.lastRssi, gwStats.lastSnr);
          Serial.printf("   GPS: %s | Sats: %u\n",
                        getGPSStatus().c_str(), gwGPS.satellites);
          
        } else if (cmdUpper == "AT+SENSORS") {
          readSensors();
          
        } else if (cmdUpper == "AT+GPS") {
          printGPSData();
          
        } else if (cmdUpper == "AT+GPSRAW") {
          Serial.println("\n📡 NMEA Raw Data (10 segundos):");
          Serial.println("Press any key to stop...\n");
          
          unsigned long start = millis();
          while (millis() - start < 10000) {
            while (GPS_Serial.available()) {
              Serial.write(GPS_Serial.read());
            }
            if (Serial.available()) break;  // Parar se tecla pressionada
          }
          Serial.println("\n--- Fim NMEA Raw ---");
          
        } else if (cmdUpper == "AT+STATS") {
          loraPrintConfig();
          Serial.printf("Packets RX: %lu\n", gwStats.packetsReceived);
          Serial.printf("Packets TX: %lu\n", gwStats.packetsSent);
          Serial.printf("GPS Fix: %s\n", hasGPSFix() ? "YES" : "NO");
          Serial.printf("Uptime: %.1f min\n", millis()/60000.0);
          
        } else if (cmdUpper.startsWith("AT+FREQ=")) {
          long freq = cmdUpper.substring(8).toInt() * 1000000;
          if (freq >= 915000000 && freq <= 928000000) {
            loraSetFrequency(freq);
            Serial.println("OK");
          } else {
            Serial.println("ERROR: Freq 915-928 MHz");
          }
          
        } else if (cmdUpper.startsWith("AT+SF=")) {
          int sf = cmdUpper.substring(6).toInt();
          if (sf >= 7 && sf <= 12) {
            loraSetSpreadingFactor(sf);
            Serial.println("OK");
          } else {
            Serial.println("ERROR: SF 7-12");
          }
          
        } else if (cmdUpper.startsWith("AT+BW=")) {
          long bw = cmdUpper.substring(6).toInt() * 1000;
          if (bw == 125000 || bw == 250000 || bw == 500000) {
            loraSetBandwidth(bw);
            Serial.println("OK");
          } else {
            Serial.println("ERROR: BW 125/250/500");
          }
          
        } else if (cmdUpper.startsWith("AT+PWR=")) {
          int pwr = cmdUpper.substring(7).toInt();
          if (pwr >= 2 && pwr <= 20) {
            loraSetTxPower(pwr);
            Serial.println("OK");
          } else {
            Serial.println("ERROR: Power 2-20 dBm");
          }

        // ===== WiFi/MQTT - USAR originalBuffer para manter case! =====
        } else if (cmdUpper.startsWith("AT+WSSID=")) {
          String ssid = originalBuffer.substring(9);
          ssid.trim();
          strncpy(wifiMqttCfg.ssid, ssid.c_str(), sizeof(wifiMqttCfg.ssid)-1);
          wifiMqttCfg.ssid[sizeof(wifiMqttCfg.ssid)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.printf("SSID configurado: %s\n", wifiMqttCfg.ssid);
          
        } else if (cmdUpper.startsWith("AT+WPASS=")) {
          String pass = originalBuffer.substring(9);
          pass.trim();
          strncpy(wifiMqttCfg.password, pass.c_str(), sizeof(wifiMqttCfg.password)-1);
          wifiMqttCfg.password[sizeof(wifiMqttCfg.password)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.println("Senha WiFi configurada");
          
        } else if (cmdUpper.startsWith("AT+MQSRV=")) {
          String srv = originalBuffer.substring(9);
          srv.trim();
          strncpy(wifiMqttCfg.mqttServer, srv.c_str(), sizeof(wifiMqttCfg.mqttServer)-1);
          wifiMqttCfg.mqttServer[sizeof(wifiMqttCfg.mqttServer)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.printf("Servidor MQTT: %s\n", wifiMqttCfg.mqttServer);
          
        } else if (cmdUpper.startsWith("AT+MQPORT=")) {
          int port = cmdUpper.substring(10).toInt();
          if (port > 0 && port < 65536) {
            wifiMqttCfg.mqttPort = port;
            saveWiFiMQTTConfig();
            Serial.printf("Porta MQTT: %d\n", port);
          } else {
            Serial.println("ERROR: Porta inválida (1-65535)");
          }
          
        } else if (cmdUpper.startsWith("AT+MQUSER=")) {
          String user = originalBuffer.substring(10);
          user.trim();
          strncpy(wifiMqttCfg.mqttUser, user.c_str(), sizeof(wifiMqttCfg.mqttUser)-1);
          wifiMqttCfg.mqttUser[sizeof(wifiMqttCfg.mqttUser)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.printf("Usuário MQTT: %s\n", wifiMqttCfg.mqttUser);
          
        } else if (cmdUpper.startsWith("AT+MQPASS=")) {
          String pass = originalBuffer.substring(10);
          pass.trim();
          strncpy(wifiMqttCfg.mqttPass, pass.c_str(), sizeof(wifiMqttCfg.mqttPass)-1);
          wifiMqttCfg.mqttPass[sizeof(wifiMqttCfg.mqttPass)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.println("Senha MQTT configurada");
          
        } else if (cmdUpper.startsWith("AT+MQPUB=")) {
          String topic = originalBuffer.substring(9);
          topic.trim();
          strncpy(wifiMqttCfg.topicPub, topic.c_str(), sizeof(wifiMqttCfg.topicPub)-1);
          wifiMqttCfg.topicPub[sizeof(wifiMqttCfg.topicPub)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.printf("Tópico Pub: %s\n", wifiMqttCfg.topicPub);
          
        } else if (cmdUpper.startsWith("AT+MQSUB=")) {
          String topic = originalBuffer.substring(9);
          topic.trim();
          strncpy(wifiMqttCfg.topicSub, topic.c_str(), sizeof(wifiMqttCfg.topicSub)-1);
          wifiMqttCfg.topicSub[sizeof(wifiMqttCfg.topicSub)-1] = '\0';
          saveWiFiMQTTConfig();
          Serial.printf("Tópico Sub: %s\n", wifiMqttCfg.topicSub);
          
        } else if (cmdUpper == "AT+WIFION") {
          wifiMqttCfg.enabled = true;
          saveWiFiMQTTConfig();
          Serial.println("WiFi/MQTT habilitado");
          Serial.println("Reinicie o gateway (AT+RESET)");
          
        } else if (cmdUpper == "AT+WIFIOFF") {
          wifiMqttCfg.enabled = false;
          saveWiFiMQTTConfig();
          WiFi.disconnect(true);
          mqttClient.disconnect();
          Serial.println("WiFi/MQTT desabilitado");
          
        } else if (cmdUpper == "AT+WSTATUS") {
          Serial.println("\n📊 Status WiFi/MQTT:");
          Serial.printf("   Habilitado: %s\n", wifiMqttCfg.enabled ? "SIM" : "NÃO");
          Serial.printf("   SSID: %s\n", wifiMqttCfg.ssid);
          Serial.printf("   WiFi: %s", isWiFiConnected() ? "✓ Conectado" : "✗ Desconectado");
          if (isWiFiConnected()) {
            Serial.printf(" | IP: %s | RSSI: %d dBm\n", 
                         WiFi.localIP().toString().c_str(), WiFi.RSSI());
          } else {
            Serial.println();
          }
          Serial.printf("   MQTT Server: %s:%d\n", wifiMqttCfg.mqttServer, wifiMqttCfg.mqttPort);
          Serial.printf("   MQTT: %s\n", isMQTTConnected() ? "✓ Conectado" : "✗ Desconectado");
          Serial.printf("   Tópico Pub: %s\n", wifiMqttCfg.topicPub);
          Serial.printf("   Tópico Sub: %s\n", wifiMqttCfg.topicSub);

        } else if (cmdUpper == "AT+WIFIRESET") {
          Preferences p;
          p.begin("wifimqtt", false);
          p.clear();
          p.end();
          Serial.println("NVS WiFi/MQTT limpa! Use AT+RESET");

        } else if (cmdUpper == "AT+RESET") {
          Serial.println("Reiniciando...");
          delay(1000);
          ESP.restart();
          
        } else if (cmdUpper == "AT") {
          Serial.println("OK");
          
        } else {
          Serial.println("ERROR: Comando desconhecido");
          Serial.println("Digite AT+HELP");
        }
      }
      
      atBuffer = "";
    } else {
      atBuffer += c;
    }
  }
}
