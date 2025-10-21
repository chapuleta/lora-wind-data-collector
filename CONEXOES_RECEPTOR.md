# CONEXÕES DO RECEPTOR - Lista Simples

## ESP32 LoRa Heltec V2 → RTC DS3231

| Pino ESP32 | → | Pino DS3231 | Cor Jumper |
|------------|---|-------------|------------|
| GPIO 4     | → | SDA         | 🟢 Verde   |
| GPIO 15    | → | SCL         | 🟡 Amarelo |
| 3.3V       | → | VCC         | 🔴 Vermelho |
| GND        | → | GND         | ⚫ Preto   |

**Bateria:** CR2032 inserida no suporte do módulo DS3231

---

## ESP32 LoRa Heltec V2 → Módulo SD Card

| Pino ESP32 | → | Pino SD Card | Cor Jumper |
|------------|---|--------------|------------|
| GPIO 12    | → | CS           | 🔵 Azul    |
| GPIO 23    | → | MOSI         | 🟢 Verde   |
| GPIO 13    | → | MISO         | 🟤 Marrom  |
| GPIO 17    | → | SCK          | 🟡 Amarelo |
| 3.3V       | → | VCC          | 🔴 Vermelho |
| GND        | → | GND          | ⚫ Preto   |

**Cartão:** microSD formatado FAT32 inserido no módulo

---

## Alimentação do ESP32

- **USB-C** conectado ao ESP32 (5V)

---

## Resumo Rápido:

```
ESP32 Receptor:
├─ GPIO 4  ──→ DS3231 SDA (I2C)
├─ GPIO 15 ──→ DS3231 SCL (I2C)
├─ GPIO 12 ──→ SD Card CS
├─ GPIO 23 ──→ SD Card MOSI
├─ GPIO 13 ──→ SD Card MISO
├─ GPIO 17 ──→ SD Card SCK
├─ 3.3V    ──→ DS3231 VCC e SD Card VCC
└─ GND     ──→ DS3231 GND e SD Card GND

DS3231:
└─ Bateria CR2032 no suporte

SD Card:
└─ microSD formatado FAT32
```

---

## Observações:

- **Display OLED** já vem conectado internamente na placa (não precisa ligar nada)
- **Antena LoRa** conectada no conector U.FL da placa
- **WiFi** usa antena interna da placa (não precisa ligar nada)
