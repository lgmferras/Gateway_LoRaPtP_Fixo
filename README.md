cat > README.md << 'EOF'
# ESP32 LoRa Single Channel Gateway

Gateway LoRaWAN single channel usando ESP32 e módulo SX1276.

## Hardware
- ESP32 (Heltec WiFi LoRa 32 V2 ou similar)
- Módulo LoRa SX1276
- Antena 915 MHz

## Configuração
- Frequência: AU915
- Servidor: ChirpStack

## Como usar
[Instruções de configuração e uso]

EOF

git add README.md
git commit -m "Add README"
git push