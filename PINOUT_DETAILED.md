# 🔌 Esquema Completo de Ligação - Gateway v1.4 + GPS

## 📐 Diagrama Geral

```
                           ┌──────────────────────────────┐
                           │                              │
                    ┌──────┤        ESP32 DevKit          │
                    │      │                              │
                    │      └──────────────────────────────┘
                    │             │ │ │ │ │ │ │
                    │             │ │ │ │ │ │ └─── Serial2 (GPS)
                    │             │ │ │ │ │ └───── I²C (SDA/SCL)
                    │             │ │ │ │ └─────── LoRa SPI
                    │             │ │ │ └───────── LoRa Control
                    │             │ │ └─────────── Power (3.3V)
                    │             │ └───────────── GND
                    │             └─────────────── LED
                    │
    ┌───────────────┴────────────────────────────────────────┐
    │                                                        │
    │                                                        │
┌───▼────┐  ┌────▼─────┐  ┌────▼─────┐  ┌────▼─────┐         │
│ LoRa   │  │ Sensors  │  │   OLED   │  │   GPS    │         │
│SX1276  │  │  I²C     │  │ SSD1306  │  │ NEO-6M   │         │
└────┬───┘  └──────────┘  └──────────┘  └──────────┘         │
     │      AHT20+BMP280   (0x3C)        GY-GPS6MV2          │
     │      (0x38, 0x76)                                     │
 Antena                                                      │
 915MHz                                                      │
                                                             │
                    ┌────────────────┐                       │
                    │  LED Indicador │ ← GPIO 2              │
                    │   + 220Ω       │                       │
                    └────────────────┘                       │
                                                             │
            Todos compartilham VCC (3.3V) e GND              │
            ═══════════════════════════════════              │
                                                             │
                                                             └──┐
                                                                │
                                                                ▼
                                                         ┌──────────┐
                                                         │ Fonte 5V │
                                                         │   USB    │
                                                         └──────────┘
```

---

## 📊 Tabela Completa de Conexões

### Barramento SPI (LoRa SX1276)

| Pino ESP32 | Nome SPI | Pino SX1276 | Cor Sugerida | Descrição |
|------------|----------|-------------|--------------|-----------|
| GPIO 12 | SCK | CLK/SCK | 🟡 Amarelo | Clock SPI |
| GPIO 13 | MISO | MISO | 🟠 Laranja | Master In Slave Out |
| GPIO 11 | MOSI | MOSI | 🟢 Verde | Master Out Slave In |
| GPIO 10 | SS | NSS | 🔵 Azul | Chip Select |
| GPIO 14 | - | RESET | 🟣 Roxo | Reset do módulo |
| GPIO 4 | - | DIO0 | ⚪ Branco | Interrupt (RX/TX done) |
| 3.3V | - | VCC | 🔴 Vermelho | Alimentação |
| GND | - | GND | ⚫ Preto | Terra |
| - | - | ANT | - | Antena 915 MHz ⚠️ |

---

### Barramento I²C (Sensores e Display)

| Pino ESP32 | Nome I²C | Dispositivos | Endereço I²C | Cor Sugerida |
|------------|----------|--------------|--------------|--------------|
| GPIO 21 | SDA | AHT20 + BMP280 + OLED | 0x38, 0x76/0x77, 0x3C | 🟢 Verde |
| GPIO 22 | SCL | AHT20 + BMP280 + OLED | (compartilhado) | 🟡 Amarelo |
| 3.3V | - | VCC (todos) | - | 🔴 Vermelho |
| GND | - | GND (todos) | - | ⚫ Preto |

**Dispositivos I²C:**
- **AHT20:** 0x38 (temperatura e umidade)
- **BMP280:** 0x76 ou 0x77 (pressão atmosférica)
- **OLED SSD1306:** 0x3C (display 128x64)

---

### Serial2 (GPS GY-GPS6MV2) ⭐ NOVO

| Pino ESP32 | Nome Serial | Pino GPS | Cor Sugerida | Descrição |
|------------|-------------|----------|--------------|-----------|
| GPIO 16 | RX2 | TX | 🟢 Verde | GPS envia → ESP32 recebe |
| GPIO 17 | TX2 | RX | 🔵 Azul | ESP32 envia → GPS recebe |
| 3.3V ou 5V | - | VCC | 🔴 Vermelho | Alimentação (módulo tem regulador) |
| GND | - | GND | ⚫ Preto | Terra |
| - | - | PPS | - | **NÃO CONECTAR** |

**⚠️ ATENÇÃO:**
- TX do GPS vai no RX do ESP32 (comunicação **cruzada**)
- RX do GPS vai no TX do ESP32
- Antena GPS **obrigatória** (conector IPEX)

---

### Outros Pinos

| Pino ESP32 | Função | Conexão | Cor | Observação |
|------------|--------|---------|-----|------------|
| GPIO 2 | LED Indicador | LED + Resistor 220Ω → GND | 🔴 Vermelho | Pisca ao TX/RX LoRa |
| EN | Enable/Reset | Botão → GND | - | Reset manual |
| USB | Alimentação | Cabo USB 5V | - | Alimenta todo circuito |

---

## 🎨 Diagrama Detalhado - Vista Superior

```
                     ╔═══════════════════════════════════╗
                     ║         ESP32 DevKit              ║
                     ║                                   ║
  GY-GPS6MV2         ║  3V3 ●────┬────┬────┬────┬────●  ║
  ┌──────────┐       ║           │    │    │    │       ║
  │ VCC  ●───┼───────╫───────────┴────┤    │    │       ║
  │ GND  ●───┼───────╫───────────┬────┼────┼────┴───●  ║
  │ TX   ●───┼───────╫─── GPIO16 ●    │    │            ║
  │ RX   ●───┼───────╫─── GPIO17 ●    │    │            ║
  │ PPS  ●   │       ║                │    │            ║
  └──┬───────┘       ║  GPIO 21 (SDA) ●────┼────┬──────●║
     │ Antena        ║  GPIO 22 (SCL) ●────┼────┼──────●║
     │ GPS           ║                │    │    │       ║
     │               ║  GPIO 12 (SCK)  ●───┼────┼──────●║
     │               ║  GPIO 13 (MISO) ●───┼────┼──────●║
  ┌──▼─────┐         ║  GPIO 11 (MOSI) ●───┼────┼──────●║
  │ ┌───┐  │         ║  GPIO 10 (SS)   ●───┼────┼──────●║
  │ │ ⊕ │  │         ║  GPIO 14 (RST)  ●───┼────┼──────●║
  │ └───┘  │         ║  GPIO 4 (DIO0)  ●───┼────┼──────●║
  │Antena  │         ║                │    │    │       ║
  │Cerâmica│         ║  GPIO 2  ●──────┼────┼────┼──────●║
  └────────┘         ║                │    │    │       ║
                     ╚════════════════╪════╪════╪════════╝
                                      │    │    │
            ┌─────────────────────────┘    │    │
            │   ┌──────────────────────────┘    │
            │   │  ┌───────────────────────────┘
            │   │  │
      ┌─────▼───▼──▼─────┐         ┌─────────────────┐
      │   Sensores I²C   │         │   LoRa SX1276   │
      │  AHT20 + BMP280  │         │                 │
      └──────────────────┘         │  ┌──────┐       │
                                   │  │ ANT  │       │
      ┌─────────────────┐          │  └───┬──┘       │
      │  OLED SSD1306   │          └──────┼──────────┘
      │    128x64       │                 │
      └─────────────────┘          ┌──────▼───────┐
                                   │   Antena     │
      ┌─────────────────┐          │   915 MHz    │
      │  LED + 220Ω     │          └──────────────┘
      │  (GPIO 2)       │
      └─────────────────┘
```

---

## 🔧 Montagem Passo a Passo

### Etapa 1: Alimentação
```
ESP32 3.3V ──┬─→ LoRa VCC
             ├─→ AHT20 VCC
             ├─→ BMP280 VCC
             ├─→ OLED VCC
             └─→ GPS VCC (ou usar 5V)

ESP32 GND  ──┬─→ LoRa GND
             ├─→ AHT20 GND
             ├─→ BMP280 GND
             ├─→ OLED GND
             └─→ GPS GND
```

### Etapa 2: I²C (Sensores + Display)
```
ESP32 GPIO 21 (SDA) ──┬─→ AHT20 SDA
                      ├─→ BMP280 SDA
                      └─→ OLED SDA

ESP32 GPIO 22 (SCL) ──┬─→ AHT20 SCL
                      ├─→ BMP280 SCL
                      └─→ OLED SCL
```

### Etapa 3: SPI (LoRa)
```
ESP32 GPIO 12 ─→ LoRa SCK
ESP32 GPIO 13 ─→ LoRa MISO
ESP32 GPIO 11 ─→ LoRa MOSI
ESP32 GPIO 10 ─→ LoRa NSS
ESP32 GPIO 14 ─→ LoRa RESET
ESP32 GPIO 4  ─→ LoRa DIO0
```

### Etapa 4: Serial (GPS) ⭐ NOVO
```
ESP32 GPIO 16 (RX2) ─→ GPS TX  (cruzado!)
ESP32 GPIO 17 (TX2) ─→ GPS RX  (cruzado!)
```

### Etapa 5: LED
```
ESP32 GPIO 2 ─→ Resistor 220Ω ─→ LED (+) ─→ GND (-)
```

### Etapa 6: Antenas
```
LoRa SX1276 ANT  ←─ Antena 915 MHz
GPS NEO-6M IPEX  ←─ Antena GPS cerâmica
```

---

## 📏 Lista de Materiais Atualizada

| Item | Quantidade | Observação |
|------|------------|------------|
| ESP32 DevKit | 1 | Qualquer modelo |
| LoRa SX1276 | 1 | 915 MHz |
| AHT20 | 1 | I²C temp/umidade |
| BMP280 | 1 | I²C pressão |
| OLED SSD1306 | 1 | 128x64 I²C |
| **GY-GPS6MV2** | **1** | **⭐ NEO-6M** |
| **Antena GPS** | **1** | **⭐ Cerâmica com IPEX** |
| Antena LoRa | 1 | 915 MHz |
| LED | 1 | Qualquer cor |
| Resistor 220Ω | 1 | Para LED |
| Jumpers | ~25 | Macho-macho ou fêmea-macho |
| Protoboard | 1 | Opcional (para testes) |

---

## 🧪 Teste de Continuidade

Antes de ligar, verificar com multímetro:

### Teste 1: Curto-circuitos
- [ ] Não há curto entre VCC e GND
- [ ] Pinos GPIO não estão em curto

### Teste 2: Alimentação
- [ ] VCC = 3.3V (ou 5V para GPS)
- [ ] GND comum para todos

### Teste 3: I²C
- [ ] SDA conectado em todos I²C
- [ ] SCL conectado em todos I²C

### Teste 4: SPI
- [ ] Todos 6 pinos LoRa conectados
- [ ] Antena LoRa conectada

### Teste 5: Serial GPS ⭐
- [ ] TX GPS → RX ESP32 (GPIO 16)
- [ ] RX GPS → TX ESP32 (GPIO 17)
- [ ] Antena GPS conectada

---

## ⚡ Consumo de Corrente

| Componente | Corrente | Observação |
|------------|----------|------------|
| ESP32 | ~80-160 mA | Varia com WiFi |
| LoRa TX | ~120 mA | Pico durante transmissão |
| LoRa RX | ~12 mA | Modo recepção |
| AHT20 | ~0.5 mA | Muito baixo |
| BMP280 | ~2.5 µA | Ultra baixo |
| OLED | ~20 mA | Display ligado |
| **GPS** | **~40 mA** | **Procurando satélites** |
| **GPS** | **~25 mA** | **Com fix** |
| LED | ~10 mA | Com resistor 220Ω |
| **TOTAL** | **~280 mA** | **Pico máximo** |

**Fonte recomendada:** 5V / 500mA (USB padrão)

---

## 🎯 Resultado Final

Sistema completo com:
- ✅ 4 sensores (temp, umidade, pressão × 2)
- ✅ GPS com posição exata
- ✅ Display OLED (3 páginas)
- ✅ LoRa 915 MHz
- ✅ WiFi/MQTT
- ✅ Comandos AT+

**Firmware:** v1.4  
**Data:** 2025-12-07

---

**Pronto para montar! 🛠️**
