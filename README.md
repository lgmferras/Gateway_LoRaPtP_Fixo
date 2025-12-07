# 📡 Gateway LoRa P2P - ESP32 Single Channel

[![Version](https://img.shields.io/badge/version-1.3.1-blue.svg)](https://github.com/seu-usuario/gateway-lora-p2p)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)](https://www.espressif.com/)

Gateway LoRa **single channel** para comunicação ponto-a-ponto (P2P) usando ESP32 e módulo SX1276. Desenvolvido para receber dados de estações meteorológicas remotas (end-nodes) e encaminhá-los via MQTT, com monitoramento local através de display OLED e interface de configuração via comandos AT+.

---

## 📋 Índice

- [Características](#-características)
- [Hardware Necessário](#-hardware-necessário)
- [Esquema de Ligação](#-esquema-de-ligação)
- [Estrutura do Projeto](#-estrutura-do-projeto)
- [Instalação](#-instalação)
- [Configuração](#-configuração)
- [Comandos AT+](#-comandos-at)
- [Protocolo de Dados](#-protocolo-de-dados)
- [Monitoramento](#-monitoramento)
- [WiFi e MQTT](#-wifi-e-mqtt)
- [Troubleshooting](#-troubleshooting)
- [Roadmap](#-roadmap)
- [Licença](#-licença)

---

## ✨ Características

### Funcionalidades Principais
- 📡 **Gateway LoRa Single Channel** - Recepção em 915.4 MHz (região AU915)
- 🌡️ **Sensores Locais** - AHT20 (temp/umidade) + BMP280 (pressão)
- 🖥️ **Display OLED** - Visualização em tempo real dos dados
- 📟 **Comandos AT+** - Configuração completa via serial
- 📶 **WiFi/MQTT** - Publicação automática dos dados recebidos
- 🔄 **ACK Automático** - Confirmação de recepção para end-nodes
- 💾 **Persistência** - Configurações salvas em NVS (Non-Volatile Storage)

### Especificações LoRa
| Parâmetro | Valor |
|-----------|-------|
| Frequência | 915.4 MHz (configurável 915-928 MHz) |
| Spreading Factor | 10 (configurável 7-12) |
| Bandwidth | 125 kHz (125/250/500 kHz) |
| Coding Rate | 4/5 |
| TX Power | 14 dBm (configurável 2-20 dBm) |
| Sync Word | 0x34 |

---

## 🔧 Hardware Necessário

### Componentes Principais
1. **ESP32** (qualquer modelo com pinos disponíveis)
2. **Módulo LoRa SX1276** (915 MHz)
3. **Sensor AHT20** (I²C - temperatura e umidade)
4. **Sensor BMP280** (I²C - pressão atmosférica)
5. **Display OLED SSD1306** 128x64 (I²C)
6. **Antena 915 MHz** (obrigatória para boa recepção!)

### Componentes Opcionais
- LED externo (pino 2) para indicação visual
- Fonte de alimentação 5V/1A
- Case para montagem

---

## 🔌 Esquema de Ligação

### Pinos do Módulo LoRa SX1276
```
ESP32          SX1276
-----          ------
GPIO 12  -->   SCK
GPIO 13  -->   MISO
GPIO 11  -->   MOSI
GPIO 10  -->   NSS (CS)
GPIO 14  -->   RESET
GPIO 4   -->   DIO0
3.3V     -->   VCC
GND      -->   GND
         -->   ANT (Antena 915MHz)
```

### Pinos I²C (Sensores e Display)
```
ESP32          Dispositivos I²C
-----          ----------------
GPIO 21  -->   SDA (AHT20, BMP280, OLED)
GPIO 22  -->   SCL (AHT20, BMP280, OLED)
3.3V     -->   VCC (todos)
GND      -->   GND (todos)
```

**Endereços I²C:**
- AHT20: 0x38
- BMP280: 0x76 ou 0x77
- OLED SSD1306: 0x3C

### Outros Pinos
```
ESP32          Componente
-----          ----------
GPIO 2   -->   LED (indicador TX/RX)
GND      -->   LED (cátodo via resistor 220Ω)
```

### Diagrama Visual

```
                           ┌──────────────────┐
                           │                  │
                    ┌──────┤    ESP32         │
                    │      │                  │
                    │      └──────────────────┘
                    │             │ │ │ │
                    │             │ │ │ └─── I²C (SDA/SCL)
                    │             │ │ └───── LoRa SPI
                    │             │ └─────── Power (3.3V)
                    │             └───────── GND
                    │
    ┌───────────────┼───────────────────────┐
    │               │                       │
┌───▼───┐      ┌────▼────┐              ┌───▼───┐
│ LoRa  │      │ Sensors │              │ OLED  │
│SX1276 │      │ I²C Bus │              │128x64 │
└───┬───┘      └─────────┘              └───────┘
    │          AHT20 + BMP280
    │
 Antena 915MHz
```

---

## 📁 Estrutura do Projeto

```
Gateway_LoRaPtP_Fixo/
│
├── Gateway_LoRaPtP_Fixo.ino   # Arquivo principal
├── config.h                         # Configurações e defines
├── protocol.h                       # Estrutura do payload LoRa
│
├── lora_p2p.h / .cpp               # Comunicação LoRa P2P
├── sensors.h / .cpp                # Leitura dos sensores
├── display_oled.h / .cpp           # Interface OLED
├── wifi_mqtt.h / .cpp              # Conectividade WiFi/MQTT
└── at_commands.h / .cpp            # Interface de comandos AT+
```

### Módulos e Responsabilidades

| Módulo | Responsabilidade |
|--------|------------------|
| **config.h** | Defines de pinos, parâmetros LoRa, intervalos |
| **protocol.h** | Estrutura do payload e estatísticas |
| **lora_p2p** | TX/RX LoRa, ACK, configuração dinâmica |
| **sensors** | Leitura AHT20 e BMP280 |
| **display_oled** | Atualização do display com dados |
| **wifi_mqtt** | Conexão WiFi, cliente MQTT, publicação |
| **at_commands** | Parser e executor de comandos AT+ |

---

## 🚀 Instalação

### 1. Pré-requisitos

**Arduino IDE** ou **PlatformIO** instalado.

### 2. Bibliotecas Necessárias

Instale as seguintes bibliotecas via Library Manager:

```
- LoRa by Sandeep Mistry (v0.8.0 ou superior)
- Adafruit AHTx0 (v2.0.0 ou superior)
- Adafruit BMP280 Library (v2.6.0 ou superior)
- Adafruit Unified Sensor (v1.1.0 ou superior)
- U8g2 by olikraus (v2.35.0 ou superior)
- PubSubClient by Nick O'Leary (v2.8.0 ou superior)
```

**No Arduino IDE:**
`Sketch → Include Library → Manage Libraries...`

**No PlatformIO** (platformio.ini):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

lib_deps = 
    sandeepmistry/LoRa@^0.8.0
    adafruit/Adafruit AHTX0@^2.0.0
    adafruit/Adafruit BMP280 Library@^2.6.0
    adafruit/Adafruit Unified Sensor@^1.1.0
    olikraus/U8g2@^2.35.0
    knolleary/PubSubClient@^2.8.0
```

### 3. Upload do Código

1. Conecte o ESP32 via USB
2. Selecione a placa: `Tools → Board → ESP32 Dev Module`
3. Selecione a porta: `Tools → Port → COMx` (Windows) ou `/dev/ttyUSBx` (Linux)
4. Configure: `Tools → Upload Speed → 115200`
5. Clique em **Upload** ⬆️

---

## ⚙️ Configuração

### Primeira Inicialização

Após o upload, abra o **Serial Monitor** (115200 baud):

```
╔════════════════════════════════════════╗
║   Gateway LoRa P2P v1.3.1              ║
║   Build: 2025-12-07                    ║
╚════════════════════════════════════════╝

📡 Inicializando LoRa...
   ✓ OK
   Config:
   Freq: 915.4 MHz
   SF: 10 | BW: 125 kHz
   CR: 4/5 | Sync: 0x34

🌡️  Inicializando sensores...
   AHT20... ✓
   BMP280... 0x76 ✓

📟 Comandos AT+ habilitados
   Digite 'AT+HELP' para lista completa

✅ Gateway pronto!
```

### Configuração WiFi/MQTT (Opcional)

Para habilitar o envio de dados via MQTT:

```
AT+WSSID=SeuWiFi
AT+WPASS=SuaSenha
AT+MQSRV=broker.hivemq.com
AT+MQPORT=1883
AT+MQPUB=gateway/lora/data
AT+WIFION
AT+RESET
```

---

## 📟 Comandos AT+

Digite `AT+HELP` no Serial Monitor para ver todos os comandos disponíveis.

### Comandos Básicos

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `AT+VER` | Versão do firmware | `AT+VER` |
| `AT+STATUS` | Status do gateway | `AT+STATUS` |
| `AT+SENSORS` | Ler sensores locais | `AT+SENSORS` |
| `AT+STATS` | Estatísticas LoRa | `AT+STATS` |
| `AT+RESET` | Reiniciar gateway | `AT+RESET` |

### Configuração LoRa

| Comando | Descrição | Faixa | Exemplo |
|---------|-----------|-------|---------|
| `AT+FREQ=<MHz>` | Mudar frequência | 915-928 | `AT+FREQ=916.8` |
| `AT+SF=<n>` | Spreading Factor | 7-12 | `AT+SF=12` |
| `AT+BW=<kHz>` | Bandwidth | 125/250/500 | `AT+BW=250` |
| `AT+PWR=<dBm>` | Potência TX | 2-20 | `AT+PWR=17` |

### Configuração WiFi/MQTT

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `AT+WSSID=<ssid>` | SSID WiFi | `AT+WSSID=MeuWiFi` |
| `AT+WPASS=<pass>` | Senha WiFi | `AT+WPASS=MinhaSenh@123` |
| `AT+MQSRV=<server>` | Servidor MQTT | `AT+MQSRV=192.168.1.100` |
| `AT+MQPORT=<port>` | Porta MQTT | `AT+MQPORT=1883` |
| `AT+MQUSER=<user>` | Usuário MQTT | `AT+MQUSER=admin` |
| `AT+MQPASS=<pass>` | Senha MQTT | `AT+MQPASS=secret` |
| `AT+MQPUB=<topic>` | Tópico publicação | `AT+MQPUB=gw/data` |
| `AT+MQSUB=<topic>` | Tópico inscrição | `AT+MQSUB=gw/cmd` |
| `AT+WIFION` | Habilitar WiFi/MQTT | `AT+WIFION` |
| `AT+WIFIOFF` | Desabilitar WiFi/MQTT | `AT+WIFIOFF` |
| `AT+WSTATUS` | Status WiFi/MQTT | `AT+WSTATUS` |

**Nota:** As configurações são salvas automaticamente na NVS (memória flash) e persistem após reinicializações.

---

## 📊 Protocolo de Dados

### Estrutura do Payload (20 bytes)

O gateway espera receber pacotes de **20 bytes** dos end-nodes:

```cpp
struct PayloadStruct {
  int16_t temp;    // Temperatura (°C * 100)
  uint16_t hum;    // Umidade (% * 100)
  uint16_t press;  // Pressão (hPa)
  int32_t lat;     // Latitude (* 1e6)
  int32_t lon;     // Longitude (* 1e6)
  uint16_t alt;    // Altitude (metros)
  uint8_t sats;    // Satélites GPS (255 = marcador de GW)
  uint16_t bat;    // Bateria (V * 100)
};
```

### Exemplo de Decodificação

Quando um pacote é recebido:

```
📦 RX: 20 bytes | RSSI:-45dBm | SNR:9.2dB
Hex: 09 C4 1B 58 0C 1C 00 2F A6 3C FF 69 D4 00 28 FF 01 2C 00 00

┌─ END-NODE DECODIFICADO ────────────────┐
│ Temp: 25.0°C | Hum: 70%
│ GPS: -23.550520, -46.633308 | Sats: 8
│ Bat: 3.6V | RSSI: -45dBm
└────────────────────────────────────────┘

📤 ACK→Node: ✓
```

### Payload MQTT (JSON)

Os dados recebidos são publicados via MQTT no formato JSON:

```json
{
  "temp": 25.0,
  "hum": 70.0,
  "lat": -23.550520,
  "lon": -46.633308,
  "sats": 8,
  "bat": 3.6,
  "rssi": -45,
  "snr": 9.2
}
```

---

## 📺 Monitoramento

### Display OLED

O display mostra em tempo real:

```
┌──────────────────────────┐
│ GW 915.4 Rx:142          │
│ 23.5C 65%                │
│ 1013 hPa                 │
│ Node:25.0C R:-45         │
│ 9.2dB 3.6V               │
└──────────────────────────┘
```

**Linha 1:** Frequência e contador de pacotes recebidos  
**Linhas 2-3:** Sensores locais do gateway  
**Linhas 4-5:** Dados do último end-node (ou "OFFLINE" se sem comunicação há >30s)

### Serial Monitor

Mensagens detalhadas de debug e estatísticas a cada 60 segundos:

```
📊 Stats: RX=142 TX=142 Uptime=12.3min

┌─ END-NODE DECODIFICADO ────────────────┐
│ Temp: 25.0°C | Hum: 70%
│ GPS: -23.550520, -46.633308 | Sats: 8
│ Bat: 3.6V | RSSI: -45dBm
└────────────────────────────────────────┘
```

---

## 📶 WiFi e MQTT

### Configuração Passo a Passo

1. **Configurar credenciais WiFi:**
```
AT+WSSID=SeuWiFi
AT+WPASS=SuaSenha123
```

2. **Configurar broker MQTT:**
```
AT+MQSRV=192.168.1.100
AT+MQPORT=1883
AT+MQUSER=usuario
AT+MQPASS=senha
```

3. **Configurar tópicos:**
```
AT+MQPUB=gateway/node/data
AT+MQSUB=gateway/node/cmd
```

4. **Habilitar e reiniciar:**
```
AT+WIFION
AT+RESET
```

### Verificar Conexão

```
AT+WSTATUS

📊 Status WiFi/MQTT:
   Habilitado: SIM
   SSID: MeuWiFi
   WiFi: ✓ Conectado | IP: 192.168.1.150 | RSSI: -65 dBm
   MQTT Server: 192.168.1.100:1883
   MQTT: ✓ Conectado
   Tópico Pub: gateway/node/data
   Tópico Sub: gateway/node/cmd
```

### Exemplo com Node-RED

Você pode usar Node-RED para processar os dados MQTT:

```javascript
// Node-RED function node
var data = JSON.parse(msg.payload);

msg.payload = {
    temperature: data.temp,
    humidity: data.hum,
    location: {
        lat: data.lat,
        lon: data.lon
    },
    signal: data.rssi
};

return msg;
```

---

## 🔍 Troubleshooting

### Gateway não inicia

**Sintoma:** Display em branco, sem mensagens no serial.

**Soluções:**
- Verificar alimentação (3.3V estável)
- Conferir ligação dos pinos I²C (SDA/SCL)
- Testar sensores individualmente

### Não recebe pacotes LoRa

**Sintoma:** `RX:0` no display, nenhum pacote detectado.

**Soluções:**
- ✅ **Antena conectada!** (obrigatório)
- Verificar frequência: `AT+STATUS` deve mostrar 915.4 MHz
- Conferir SF e BW compatíveis com end-node
- Testar diferentes frequências: `AT+FREQ=915.2` até `AT+FREQ=916.8`
- Verificar ligação dos pinos SPI do SX1276

### WiFi não conecta

**Sintoma:** `WiFi: ✗ Desconectado` no `AT+WSTATUS`

**Soluções:**
- Verificar SSID e senha: `AT+WSSID` e `AT+WPASS`
- Conferir sinal WiFi (precisa estar próximo ao AP)
- Resetar configuração: `AT+WIFIRESET` + `AT+RESET`
- Verificar se o WiFi está habilitado: `AT+WIFION`

### MQTT não publica

**Sintoma:** Conecta WiFi mas `MQTT: ✗ Desconectado`

**Soluções:**
- Testar broker com ferramenta externa (MQTT Explorer, mosquitto_pub)
- Verificar firewall/portas abertas no broker
- Conferir credenciais MQTT: `AT+MQUSER` e `AT+MQPASS`
- Aumentar buffer: alterar `MQTT_BUFFER_SIZE` em `config.h`

### Sensores retornam valores errados

**Sintoma:** Temperatura/pressão irreais

**Soluções:**
- Verificar endereço I²C: `i2cdetect` ou scanner I²C
- BMP280 pode estar em 0x76 ou 0x77
- Conferir alimentação 3.3V (não 5V!)
- Aguardar estabilização (~30s após ligar)

---

## 🗺️ Roadmap

### v1.4 (Próxima versão)
- [ ] Suporte a múltiplos end-nodes simultâneos
- [ ] Armazenamento local de dados (SD card)
- [ ] Interface web para configuração
- [ ] Suporte a downlink (comandos para end-nodes)

### v2.0 (Futuro)
- [ ] Modo multi-canal (8+ canais)
- [ ] Integração com ChirpStack/TTN
- [ ] GPS no gateway para timestamp preciso
- [ ] Modo sleep para baixo consumo

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para:

1. Fazer fork do projeto
2. Criar uma branch para sua feature (`git checkout -b feature/MinhaFeature`)
3. Commit suas mudanças (`git commit -m 'Adiciona MinhaFeature'`)
4. Push para a branch (`git push origin feature/MinhaFeature`)
5. Abrir um Pull Request

---

## 📄 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 👤 Autor

**Gustavo**  
Projeto desenvolvido para monitoramento de estações meteorológicas via LoRa P2P.

---

## 📞 Suporte

- **Issues:** Reporte bugs ou sugira features através das [GitHub Issues](https://github.com/seu-usuario/gateway-lora-p2p/issues)
- **Discussões:** Participe das [Discussions](https://github.com/seu-usuario/gateway-lora-p2p/discussions)

---

## 🙏 Agradecimentos

- [Sandeep Mistry](https://github.com/sandeepmistry) - Biblioteca LoRa
- [Adafruit](https://www.adafruit.com/) - Bibliotecas de sensores
- [Olikraus](https://github.com/olikraus) - Biblioteca U8g2
- Comunidade LoRa brasileira

---

## 📚 Referências

- [LoRa Alliance](https://lora-alliance.org/)
- [SX1276 Datasheet](https://www.semtech.com/products/wireless-rf/lora-core/sx1276)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [MQTT Protocol](https://mqtt.org/)

---

<div align="center">

**⭐ Se este projeto foi útil, considere dar uma estrela! ⭐**

Made with ❤️ and ☕ in Brazil 🇧🇷

</div>
