# Leitura Anemômetro e Biruta

Sistema de monitoramento de vento usando ESP32 (Heltec WiFi LoRa 32 V2) com comunicação Modbus RS485.

## 📋 Descrição

Este projeto realiza a leitura de:
- **Velocidade do vento** (Anemômetro - Modbus ID 1)
- **Direção do vento** (Biruta - Modbus ID 2)
- **Display OLED** integrado para visualização dos dados em tempo real

## 🔧 Hardware

- **Microcontrolador**: Heltec WiFi LoRa 32 V2 (ESP32)
- **Comunicação**: RS485 via MAX485
- **Sensores**: Anemômetro e Biruta com protocolo Modbus RTU

### Pinagem

| Função | Pino ESP32 | Descrição |
|--------|------------|-----------|
| RX | 12 | Recepção RS485 |
| TX | 13 | Transmissão RS485 |
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

**Serial Monitor:**
```
Velocidade do vento: 5.2 m/s
Direção do vento (raw): 180
---
```

**Display OLED:**
- Título: "Monitor Vento"
- Velocidade do vento em m/s
- Direção do vento em graus
- Indicação de erro para cada sensor

## 🛠️ Dependências

- ModbusMaster@^2.0.1
- Heltec ESP32 Dev-Boards@^1.1.1

## 📝 Autor

Desenvolvido para monitoramento meteorológico.
