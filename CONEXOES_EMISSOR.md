# CONEXÕES DO EMISSOR - Lista Simples

## ESP32 LoRa Heltec V2 → MAX485

| Pino ESP32 | → | Pino MAX485 | Cor Jumper |
|------------|---|-------------|------------|
| GPIO 13    | → | DI          | 🟠 Laranja |
| GPIO 12    | → | RO          | 🟤 Marrom  |
| GPIO 23     | → | DE e RE (juntos) | 🟡 Amarelo |
| 3.3V       | → | VCC         | 🔴 Vermelho |
| GND        | → | GND         | ⚫ Preto   |

---

## MAX485 → Sensores (barramento RS485)

| Pino MAX485 | → | Sensores | Cor Jumper |
|-------------|---|----------|------------|
| A           | → | A+ do Anemômetro e A+ da Biruta (em paralelo) | 🟡 Amarelo |
| B           | → | B- do Anemômetro e B- da Biruta (em paralelo) | 🔵 Azul |

**Resistor de terminação:** 120Ω entre A+ e B- no último sensor (Biruta)

---

## Fonte Externa 12V/24V → Sensores

| Fonte Externa | → | Sensores | Cor Fio |
|---------------|---|----------|---------|
| + (positivo)  | → | VCC do Anemômetro e VCC da Biruta | 🔴 Vermelho (grosso) |
| - (negativo)  | → | GND do Anemômetro e GND da Biruta | ⚫ Preto (grosso) |
| - (negativo)  | → | **TAMBÉM** conectar ao GND do ESP32 | ⚫ Preto (grosso) |

---

## Alimentação do ESP32

- **USB-C** conectado ao ESP32 (5V)

---

## Resumo Rápido:

```
ESP32 Emissor:
├─ GPIO 13 ──→ MAX485 DI
├─ GPIO 12 ──→ MAX485 RO
├─ GPIO 2  ──→ MAX485 DE+RE
├─ 3.3V    ──→ MAX485 VCC
└─ GND     ──→ MAX485 GND
              └─ Fonte Externa GND (aterramento comum)

MAX485:
├─ A ──→ Anemômetro A+ e Biruta A+ (paralelo)
└─ B ──→ Anemômetro B- e Biruta B- (paralelo)
            └─ 120Ω entre A+ e B- na Biruta

Fonte 12V/24V:
├─ (+) ──→ Anemômetro VCC e Biruta VCC
└─ (-) ──→ Anemômetro GND, Biruta GND e ESP32 GND
```
