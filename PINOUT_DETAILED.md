# 🔌 Esquema de Ligação Detalhado - Gateway LoRa P2P

Este documento complementa o README.md com informações detalhadas sobre a pinagem e conexões.

---

## 📐 Diagrama Completo

```
                    ╔══════════════════════════════════════╗
                    ║          ESP32 DevKit                ║
                    ║                                      ║
                    ║   3.3V ●────┬────┬────┬────┬────●   ║
                    ║             │    │    │    │         ║
                    ║   GND  ●────┼────┼────┼────┼────●   ║
                    ║             │    │    │    │         ║
                    ║   GPIO 21 (SDA) ●─┬──┬──┬──────●    ║
                    ║   GPIO 22 (SCL) ●─┼──┼──┼──────●    ║
                    ║             │    │ │  │  │           ║
                    ║   GPIO 12 (SCK)  ●─┼──┼──┼──────●   ║
                    ║   GPIO 13 (MISO) ●─┼──┼──┼──────●   ║
                    ║   GPIO 11 (MOSI) ●─┼──┼──┼──────●   ║
                    ║   GPIO 10 (SS)   ●─┼──┼──┼──────●   ║
                    ║   GPIO 14 (RST)  ●─┼──┼──┼──────●   ║
                    ║   GPIO 4 (DIO0)  ●─┼──┼──┼──────●   ║
                    ║             │    │ │  │  │           ║
                    ║   GPIO 2  ●─────┼──┼──┼──┼──────●   ║
                    ║             │    │ │  │  │           ║
                    ╚═════════════╪════╪═╪══╪══╪═══════════╝
                                  │    │ │  │  │
                        ┌─────────┘    │ │  │  │
                        │   ┌──────────┘ │  │  │
                        │   │  ┌─────────┘  │  │
                        │   │  │  ┌─────────┘  │
                        │   │  │  │  ┌─────────┘
                        │   │  │  │  │
            ┌───────────▼───▼──▼──▼──▼────────────┐
            │                                      │
     ┌──────▼──────┐  ┌────▼─────┐  ┌────▼─────┐ │
     │   AHT20     │  │  BMP280  │  │   OLED   │ │
     │             │  │          │  │ SSD1306  │ │
     │  0x38       │  │  0x76    │  │  0x3C    │ │
     └─────────────┘  └──────────┘  └──────────┘ │
                                                   │
     ┌─────────────────────────────────────────┐  │
     │          Módulo LoRa SX1276             │  │
     │                                         │  │
     │  VCC ● ← 3.3V                           │  │
     │  GND ● ← GND                            │  │
     │  SCK ● ← GPIO 12                        │  │
     │ MISO ● ← GPIO 13                        │  │
     │ MOSI ● ← GPIO 11                        │  │
     │  NSS ● ← GPIO 10                        │  │
     │  RST ● ← GPIO 14                        │  │
     │ DIO0 ● ← GPIO 4                         │  │
     │  ANT ●────────● Antena 915 MHz          │  │
     └─────────────────────────────────────────┘  │
                                                   │
     ┌─────────────┐                              │
     │     LED     │ ← GPIO 2 (através resistor)  │
     │   + 220Ω    │                              │
     └─────────────┘                              │
                                                   │
            Todos compartilham VCC (3.3V) e GND   │
            ════════════════════════════════════   │
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

## 📊 Tabela de Pinagem Completa

### Barramento SPI (LoRa SX1276)

| Pino ESP32 | Nome SPI | Pino SX1276 | Descrição |
|------------|----------|-------------|-----------|
| GPIO 12 | SCK | CLK/SCK | Clock SPI |
| GPIO 13 | MISO | MISO | Master In Slave Out |
| GPIO 11 | MOSI | MOSI | Master Out Slave In |
| GPIO 10 | SS | NSS | Chip Select |
| GPIO 14 | - | RESET | Reset do módulo |
| GPIO 4 | - | DIO0 | Interrupt (RX/TX done) |
| 3.3V | - | VCC | Alimentação |
| GND | - | GND | Terra |
| - | - | ANT | Antena 915 MHz ⚠️ |

**⚠️ IMPORTANTE:** A antena é **obrigatória**! Nunca ligue o módulo LoRa sem antena conectada.

---

### Barramento I²C (Sensores e Display)

| Pino ESP32 | Nome I²C | Dispositivos | Endereço I²C |
|------------|----------|--------------|--------------|
| GPIO 21 | SDA | AHT20 + BMP280 + OLED | 0x38, 0x76/0x77, 0x3C |
| GPIO 22 | SCL | AHT20 + BMP280 + OLED | (compartilhado) |
| 3.3V | - | VCC (todos) | - |
| GND | - | GND (todos) | - |

**Dispositivos I²C:**
- **AHT20:** 0x38 (fixo)
- **BMP280:** 0x76 ou 0x77 (verificar com i2c_scanner)
- **OLED SSD1306:** 0x3C (padrão 128x64)

---

### Outros Pinos

| Pino ESP32 | Função | Conexão | Observação |
|------------|--------|---------|------------|
| GPIO 2 | LED Indicador | LED + Resistor 220Ω → GND | Pisca ao TX/RX |
| EN | Enable/Reset | Botão → GND | Reset manual |
| USB | Alimentação | Cabo USB 5V | Alimenta todo circuito |

---

## 🔧 Detalhes Técnicos

### Alimentação

```
Fonte USB 5V
    │
    ├─→ Regulador ESP32 (3.3V) ─┬─→ ESP32
    │                            ├─→ SX1276 (3.3V máx!)
    │                            ├─→ AHT20
    │                            ├─→ BMP280
    │                            └─→ OLED
    │
    └─→ GND comum para todos
```

**⚠️ ATENÇÃO:**
- **SX1276 suporta APENAS 3.3V!** Não conecte em 5V!
- Corrente mínima recomendada: **500mA**
- Durante TX LoRa, pico de corrente pode chegar a **120mA**

---

### Barramento I²C - Múltiplos Dispositivos

```
ESP32 GPIO 21 (SDA) ──┬──● AHT20 (SDA)
                      │
                      ├──● BMP280 (SDA/SDI)
                      │
                      └──● OLED (SDA)

ESP32 GPIO 22 (SCL) ──┬──● AHT20 (SCL)
                      │
                      ├──● BMP280 (SCL/SCK)
                      │
                      └──● OLED (SCL)
```

**Observações:**
- Não precisa resistores pull-up externos (ESP32 tem internos)
- Distância máxima do cabo I²C: ~1 metro
- Se tiver problemas, adicione pull-ups de 4.7kΩ em SDA e SCL

---

### Configuração do LED

```
ESP32 GPIO 2 ──┬──→ Resistor 220Ω ──→ LED (Ânodo +)
               │
               └──────────────────────→ GND (Cátodo -)
```

**Comportamento:**
- **OFF:** Gateway em idle/RX
- **PISCA RÁPIDO:** Enviando ACK
- **PISCA A CADA RX:** Pacote recebido

---

## 🧪 Testando as Conexões

### 1. Verificar Alimentação

```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 inicializado!");
  Serial.printf("Tensão CPU: %.2fV\n", ESP.getVcc() / 1000.0);
}
```

### 2. Scanner I²C

Cole este código no Arduino IDE para detectar dispositivos I²C:

```cpp
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  
  Serial.println("Scanning...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("Device found at 0x%02X\n", address);
      nDevices++;
    }
  }
  
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.printf("\n%d devices found\n", nDevices);
  
  delay(5000);
}
```

**Saída esperada:**
```
Scanning...
Device found at 0x38  ← AHT20
Device found at 0x3C  ← OLED
Device found at 0x76  ← BMP280

3 devices found
```

### 3. Testar LoRa

```cpp
#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  SPI.begin(12, 13, 11, 10);
  LoRa.setPins(10, 14, 4);
  
  if (!LoRa.begin(915400000)) {
    Serial.println("LoRa FAIL!");
    while(1);
  }
  Serial.println("LoRa OK!");
}
```

---

## 🛠️ Montagem Recomendada

### Ordem de Montagem

1. **Primeiro:** Conectar barramentos de alimentação (3.3V e GND)
2. **Segundo:** Conectar I²C (SDA e SCL) - mais fácil de debugar
3. **Terceiro:** Conectar SPI do LoRa
4. **Último:** LED indicador

### Dicas de Soldagem

- Use **fios de cores diferentes** para cada barramento:
  - 🔴 Vermelho → 3.3V
  - ⚫ Preto → GND
  - 🔵 Azul → SDA
  - 🟢 Verde → SCL
  - 🟡 Amarelo → SPI (SCK, MISO, MOSI)

- Mantenha os fios **curtos** (< 15cm)
- Use **conectores JST ou Dupont** para facilitar manutenção
- Solde um **capacitor 100µF** próximo ao SX1276 (entre VCC e GND)

---

## ⚡ Problemas Comuns

### Display não acende

**Verificar:**
- [ ] VCC = 3.3V (não 5V!)
- [ ] Endereço correto (0x3C ou 0x3D)
- [ ] Cabos SDA/SCL não invertidos

### LoRa não inicializa

**Verificar:**
- [ ] Antena conectada!
- [ ] Pinos SPI corretos (SCK, MISO, MOSI, SS)
- [ ] VCC = 3.3V estável
- [ ] DIO0 conectado no GPIO 4

### Sensores não respondem

**Verificar:**
- [ ] Pull-ups habilitados (automático no ESP32)
- [ ] Endereços I²C corretos com scanner
- [ ] Alimentação 3.3V estável
- [ ] Fios curtos (< 20cm)

---

## 📷 Fotos Recomendadas

Para documentar seu projeto no GitHub, tire fotos de:

1. **Vista superior** do circuito montado
2. **Close-up** das conexões SPI (LoRa)
3. **Close-up** das conexões I²C (sensores)
4. **Display OLED** em funcionamento
5. **Antena** conectada
6. **Setup completo** em caixa/case

---

## 🔗 Recursos Adicionais

- [Pinout ESP32](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [Datasheet SX1276](https://www.semtech.com/products/wireless-rf/lora-core/sx1276)
- [AHT20 Datasheet](http://www.aosong.com/userfiles/files/media/AHT20-datasheet-2020-4-16.pdf)
- [BMP280 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf)

---

## ✅ Checklist Antes de Ligar

- [ ] Todas conexões conferidas 2x
- [ ] Nenhum curto-circuito entre VCC e GND
- [ ] Antena LoRa conectada
- [ ] Alimentação = 5V USB ou 3.3V regulado
- [ ] Código compilado sem erros
- [ ] Serial Monitor aberto (115200 baud)

---

**Última atualização:** 2025-12-04  
**Versão do Firmware:** 1.3
