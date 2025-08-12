# Leitura Anemômetro e Biruta

Sistema de monitoramento de vento usando ESP32 (Heltec WiFi LoRa 32 V2) com comunicação Modbus RS485.

## 📋 Descrição

Este projeto realiza a leitura de:
- **Velocidade do vento** (Anemômetro - Modbus ID 1)
- **Direção do vento** (Biruta - Modbus ID 2)

## 🔧 Hardware

- **Microcontrolador**: Heltec WiFi LoRa 32 V2 (ESP32)
- **Comunicação**: RS485 via MAX485
- **Sensores**: Anemômetro e Biruta com protocolo Modbus RTU

### Pinagem

| Função | Pino ESP32 | Descrição |
|--------|------------|-----------|
| RX | 16 | Recepção RS485 |
| TX | 17 | Transmissão RS485 |
| DE/RE | 2 | Controle de direção MAX485 |

## ⚙️ Configuração

- **Baudrate**: 9600 bps
- **Protocolo**: Modbus RTU
- **IDs dos dispositivos**:
  - Anemômetro: ID 1 (registrador 0x0000)
  - Biruta: ID 2 (registrador 0x0001)

## 🚀 Como usar

1. Clone o repositório
2. Abra no PlatformIO
3. Conecte o hardware conforme a pinagem
4. Compile e faça upload:
   ```bash
   pio run -t upload -t monitor
   ```

## 📊 Saída

O sistema exibe no Serial Monitor:
```
Velocidade do vento: 5.2 m/s
Direção do vento (raw): 180
---
```

## 🛠️ Dependências

- ModbusMaster@^2.0.1

## 📝 Autor

Desenvolvido para monitoramento meteorológico.
