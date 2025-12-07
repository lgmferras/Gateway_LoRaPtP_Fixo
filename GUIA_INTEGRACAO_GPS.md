# 🛰️ Guia Completo - Integração GPS GY-GPS6MV2

Integração completa do módulo GPS NEO-6M ao Gateway LoRa P2P.

---

## 📦 1. Hardware - Conexões

### Módulo GY-GPS6MV2 (NEO-6M)

```
┌─────────────────────────────────────────────┐
│                                             │
│   ESP32                     GY-GPS6MV2      │
│                                             │
│   3.3V  ●──────────────●  VCC               │
│   GND   ●──────────────●  GND               │
│   GPIO16 (RX2)  ●──────●  TX  (GPS→ESP32)   │
│   GPIO17 (TX2)  ●──────●  RX  (ESP32→GPS)   │
│                        ●  PPS (não usar)    │
│                                             │
└─────────────────────────────────────────────┘
```

**⚠️ IMPORTANTE:**
- TX do GPS conecta no RX do ESP32 (GPIO 16)
- RX do GPS conecta no TX do ESP32 (GPIO 17)
- Usamos Serial2 do ESP32 (pinos 16/17)
- VCC pode ser 3.3V ou 5V (módulo tem regulador)

### Tabela de Pinos

| GY-GPS6MV2 | Cor Sugerida | ESP32 | Função |
|------------|--------------|-------|--------|
| VCC | 🔴 Vermelho | 3.3V ou 5V | Alimentação |
| GND | ⚫ Preto | GND | Terra |
| TX | 🟢 Verde | GPIO 16 (RX2) | Dados GPS → ESP32 |
| RX | 🔵 Azul | GPIO 17 (TX2) | Comandos ESP32 → GPS |
| PPS | - | NC | Não conectar |

### Antena

⚠️ **OBRIGATÓRIO:** O módulo GPS **PRECISA** de antena externa conectada!
- Conecte a antena cerâmica no conector IPEX
- Posicione com vista para o céu
- Evite ambientes fechados/cobertos

---

## 📚 2. Biblioteca Necessária

### Instalar TinyGPS++

**Arduino IDE:**
1. `Sketch → Include Library → Manage Libraries`
2. Buscar: `TinyGPSPlus`
3. Instalar: **TinyGPSPlus by Mikal Hart** (v1.0.3 ou superior)

**PlatformIO:**
Adicionar em `platformio.ini`:
```ini
lib_deps = 
    mikalhart/TinyGPSPlus@^1.0.3
    # ... outras bibliotecas
```

---

## 📁 3. Arquivos a Adicionar/Modificar

### Arquivos NOVOS:

1. **gps.h** - Interface do módulo GPS
2. **gps.cpp** - Implementação GPS

### Arquivos MODIFICADOS:

3. **config.h** - Adicionar defines GPS
4. **at_commands.cpp** - Adicionar comandos AT+GPS
5. **display_oled.cpp** - Adicionar página GPS
6. **lora_p2p.cpp** - Incluir GPS no MQTT
7. **Gateway_LoRaPtP_Fixo_v1_4.ino** - Incluir chamadas GPS

---

## 🔧 4. Passo a Passo da Integração

### Passo 1: Adicionar Arquivos Novos

Copie para a pasta do projeto:
```
Gateway_LoRaPtP_Fixo_v1_4/
├── gps.h               ← NOVO
├── gps.cpp             ← NOVO
└── ... (outros arquivos)
```

### Passo 2: Atualizar config.h

Adicione após as definições de pinos LoRa:

```cpp
// ===== PINOS GPS GY-GPS6MV2 =====
#define GPS_RX_PIN 16   // ESP32 RX2 ← GPS TX
#define GPS_TX_PIN 17   // ESP32 TX2 → GPS RX
#define GPS_BAUD 9600   // Padrão do NEO-6M

// Em INTERVALOS, adicione:
#define GPS_UPDATE_INTERVAL 1000     // 1s

// Em TIMEOUTS, adicione:
#define GPS_TIMEOUT 30000     // 30s sem atualização = sem fix

// Em DEBUG, adicione:
#define DEBUG_GPS 1
#define DEBUG_GPS_RAW 0      // 0=off, 1=mostrar NMEA bruto
```

### Passo 3: Atualizar Versão

Em `config.h`:
```cpp
#define FW_VERSION "1.4"
#define FW_DATE "2025-12-07"
```

### Passo 4: Modificar Arquivo Principal (.ino)

```cpp
// Adicionar include
#include "gps.h"

// Adicionar variável global
unsigned long lastGPSUpdate = 0;

// No setup(), adicionar:
initGPS();          // ← Logo após initSensors()

// No loop(), adicionar:
// Atualizar GPS
if (ms - lastGPSUpdate >= GPS_UPDATE_INTERVAL) {
  updateGPS();
  lastGPSUpdate = ms;
}

// Também adicionar no final do loop():
updateGPS();  // Continua processando em background
```

### Passo 5: Atualizar AT Commands

Adicionar novos comandos na função `printATHelp()`:
```cpp
Serial.println("║ AT+GPS          - Dados GPS do gateway             ║");
Serial.println("║ AT+GPSRAW       - NMEA sentences (debug)           ║");
```

E implementar na função `processATCommands()`:
```cpp
} else if (cmdUpper == "AT+GPS") {
  printGPSData();
  
} else if (cmdUpper == "AT+GPSRAW") {
  Serial.println("\n📡 NMEA Raw Data (10 segundos):");
  // ... código para mostrar NMEA
```

### Passo 6: Atualizar Display

O display agora terá **3 páginas** rotativas (5s cada):
- **Página 0:** Status geral + Node
- **Página 1:** GPS do Gateway ← NOVO
- **Página 2:** Estatísticas

### Passo 7: Atualizar MQTT

O payload MQTT agora inclui seção GPS do gateway:
```json
{
  ...
  "gateway": {
    "temp": 23.5,
    "hum": 65.0,
    "press": 1013.2,
    "gps": {                    ← NOVO
      "lat": -23.186982,
      "lon": -45.884621,
      "alt": 650.5,
      "sats": 8,
      "hdop": 1.2,
      "fix": true
    },
    "wifi_rssi": -65
  },
  ...
}
```

---

## 🧪 5. Teste e Validação

### Teste 1: Verificar Conexão

Upload do código e abra Serial Monitor (115200 baud):

```
🛰️  Inicializando GPS...
   Porta: Serial2 (RX:16, TX:17)
   Baud: 9600
   Aguardando fix GPS...
   (pode levar 30s-2min em ambiente aberto)
```

### Teste 2: NMEA Raw

Digite no Serial Monitor:
```
AT+GPSRAW
```

**Saída esperada:**
```
📡 NMEA Raw Data (10 segundos):
Press any key to stop...

$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
...

--- Fim NMEA Raw ---
```

Se não aparecer nada:
- ❌ Verificar ligação TX/RX
- ❌ Conferir se Serial2 está correto
- ❌ Testar trocar TX com RX (inversão comum)

### Teste 3: Aguardar Fix

**IMPORTANTE:** GPS precisa de:
- ✅ Vista para o céu (janela aberta ou área externa)
- ✅ Antena conectada
- ✅ 30 segundos a 2 minutos em cold start

Digite:
```
AT+GPS
```

**Sem fix:**
```
📡 GPS Gateway:
   ❌ Sem fix GPS
   Satélites: 3
   Chars processados: 1234
   ⚠️  Nenhum satélite - mover para área aberta
```

**Com fix:**
```
📡 GPS Gateway:
   ✓ Fix GPS obtido!
   Lat: -23.186982°
   Lon: -45.884621°
   Alt: 650.5 m
   Sats: 8 | HDOP: 1.2
   Última atualização: 234 ms atrás
   Sentenças com fix: 45
   Google Maps: https://maps.google.com/?q=-23.186982,-45.884621
```

### Teste 4: Display

O display deve rotacionar mostrando em uma das páginas:
```
GPS Gateway:
-23.186982
-45.884621
Alt:650m S:8
HDOP:1.2 GOOD
```

### Teste 5: MQTT

Quando receber um pacote LoRa, verificar payload MQTT:
```bash
mosquitto_sub -h SEU_BROKER -t gateway/data | jq '.gateway.gps'
```

Deve retornar:
```json
{
  "lat": -23.186982,
  "lon": -45.884621,
  "alt": 650.5,
  "sats": 8,
  "hdop": 1.2,
  "fix": true
}
```

---

## 🔍 6. Troubleshooting

### GPS não recebe dados (chars processados = 0)

**Causa:** Pinos invertidos ou solda ruim

**Solução:**
```cpp
// Tente inverter em config.h:
#define GPS_RX_PIN 17   // ← INVERTIDO
#define GPS_TX_PIN 16   // ← INVERTIDO
```

### GPS recebe dados mas sem satélites

**Causa:** Sem vista para o céu ou antena mal conectada

**Solução:**
- Mover para janela ou área externa
- Verificar antena no conector IPEX
- Aguardar mais tempo (até 5 min em cold start)

### Fix GPS depois perde

**Causa:** GPS timeout ou interferência

**Solução:**
```cpp
// Aumentar timeout em config.h:
#define GPS_TIMEOUT 60000  // 60s
```

### Display só mostra "Sem dados"

**Causa:** GPS não inicializou

**Solução:**
- Verificar se `initGPS()` está no setup()
- Conferir includes no .ino

### MQTT sem GPS do gateway

**Causa:** Código não atualizado

**Solução:**
- Usar `lora_p2p_COM_GPS.cpp` fornecido
- Verificar se tem seção `"gps"` dentro de `"gateway"`

---

## 📊 7. Comandos AT+ GPS

### AT+GPS
Mostra dados completos do GPS:
```
AT+GPS

📡 GPS Gateway:
   ✓ Fix GPS obtido!
   Lat: -23.186982°
   Lon: -45.884621°
   Alt: 650.5 m
   Sats: 8 | HDOP: 1.2
   Última atualização: 234 ms atrás
   Google Maps: https://maps.google.com/?q=-23.186982,-45.884621
```

### AT+GPSRAW
Mostra NMEA sentences brutas (10s):
```
AT+GPSRAW

$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M...
```

### AT+STATUS
Inclui status GPS:
```
AT+STATUS

📊 Status Gateway:
   Uptime: 1234 s
   Packets RX: 56 | TX: 56
   Last RSSI: -45 dBm | SNR: 9.2 dB
   GPS: EXCELLENT | Sats: 8         ← NOVO
```

---

## 💡 8. Dicas e Otimizações

### Cold Start vs Hot Start

**Cold Start** (primeira vez ou sem bateria backup):
- Pode levar 30s a 5 minutos
- GPS precisa baixar almanaque dos satélites

**Hot Start** (já teve fix recentemente):
- 5-30 segundos
- Mais rápido se tiver bateria no módulo

### Melhorar Precisão

```cpp
// Em gps.cpp, após obter fix, você pode filtrar por HDOP:
if (gps.hdop.isValid() && gps.hdop.hdop() < 2.0) {
  // HDOP < 2 = excelente precisão
  gwGPS.latitude = gps.location.lat();
  gwGPS.longitude = gps.location.lng();
}
```

### Economia de Energia

Se não precisar de GPS o tempo todo:
```cpp
// Desligar GPS temporariamente
GPS_Serial.end();

// Religar depois
GPS_Serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
```

### Debug Avançado

Ativar debug NMEA raw em `config.h`:
```cpp
#define DEBUG_GPS_RAW 1  // Ver todas sentenças NMEA
```

---

## ✅ Checklist de Integração

- [ ] Biblioteca TinyGPS++ instalada
- [ ] Pinos conectados (VCC, GND, TX, RX)
- [ ] Antena conectada no GPS
- [ ] Arquivos gps.h e gps.cpp adicionados
- [ ] config.h atualizado com pinos GPS
- [ ] .ino principal atualizado (include + init + update)
- [ ] at_commands.cpp com AT+GPS e AT+GPSRAW
- [ ] display_oled.cpp com página GPS
- [ ] lora_p2p.cpp com GPS no MQTT
- [ ] Compilado sem erros
- [ ] Upload realizado
- [ ] AT+GPSRAW mostra dados NMEA
- [ ] AT+GPS mostra fix obtido
- [ ] Display mostra página GPS
- [ ] MQTT inclui gateway.gps

---

## 📈 Exemplo Uso MQTT com GPS

### Node-RED - Plotar no Mapa

```javascript
// Function node
var data = msg.payload;

// Extrair GPS do gateway
msg.payload = {
    name: "Gateway LoRa",
    lat: data.gateway.gps.lat,
    lon: data.gateway.gps.lon,
    icon: "wifi",
    label: `Gateway - ${data.gateway.gps.sats} sats`,
    accuracy: data.gateway.gps.hdop * 10  // HDOP em metros
};

// Conectar em worldmap node
return msg;
```

### Calcular Distância Node ↔ Gateway

```javascript
// Function node - Haversine formula
function distance(lat1, lon1, lat2, lon2) {
    const R = 6371; // Raio da Terra em km
    const dLat = (lat2 - lat1) * Math.PI / 180;
    const dLon = (lon2 - lon1) * Math.PI / 180;
    const a = Math.sin(dLat/2) * Math.sin(dLat/2) +
              Math.cos(lat1 * Math.PI / 180) * Math.cos(lat2 * Math.PI / 180) *
              Math.sin(dLon/2) * Math.sin(dLon/2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
    return R * c;
}

var data = msg.payload;
var dist = distance(
    data.node.gps.lat,
    data.node.gps.lon,
    data.gateway.gps.lat,
    data.gateway.gps.lon
);

msg.distance_km = dist.toFixed(2);
msg.distance_m = (dist * 1000).toFixed(0);

return msg;
```

---

## 🎯 Resultado Final

Com GPS integrado, seu gateway agora:

✅ Sabe sua posição exata  
✅ Mostra dados GPS no display (3 páginas rotativas)  
✅ Publica GPS no MQTT junto com sensores  
✅ Envia GPS no ACK para o end-node  
✅ Comandos AT+GPS e AT+GPSRAW disponíveis  
✅ Link Google Maps no serial  

**Versão do firmware:** 1.4  
**Data:** 2025-12-07

---

**Próximos passos sugeridos:**
1. Mapear cobertura LoRa (wardriving GPS)
2. Calcular distância node ↔ gateway
3. Criar dashboard com mapa
4. Análise RSSI vs distância
