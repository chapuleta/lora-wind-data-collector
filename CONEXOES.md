# Guia de Conexões - Sistema de Leitura Anemômetro e Biruta

## 📋 Visão Geral

Este documento descreve como conectar o **Heltec WiFi LoRa 32 V2** ao módulo **MAX485** e aos sensores **Anemômetro** (ID Modbus 1) e **Biruta** (ID Modbus 2) via RS-485.

---

## 🔌 Pinagem Utilizada no Código

| Função | Pino GPIO | Observação |
|--------|-----------|------------|
| Serial2 RX | GPIO 12 | Recebe dados do MAX485 (RO) |
| Serial2 TX | GPIO 13 | Envia dados para MAX485 (DI) |
| DE/RE Control | GPIO 2 | Controla modo TX/RX do MAX485 |
| Alimentação | 3.3V | VCC do MAX485 |
| Ground | GND | Referência comum |

---

## 🔧 Conexões: Heltec → MAX485

| Heltec ESP32 | MAX485 | Cor Sugerida | Descrição |
|--------------|--------|--------------|-----------|
| **3.3V** | **VCC** | 🔴 Vermelho | Alimentação do módulo MAX485 |
| **GND** | **GND** | ⚫ Preto | Ground comum (obrigatório!) |
| **GPIO 13 (TX2)** | **DI** | 🟡 Amarelo | Transmissão de dados (ESP → sensores) |
| **GPIO 12 (RX2)** | **RO** | 🟢 Verde | Recepção de dados (sensores → ESP) |
| **GPIO 2** | **DE + RE** | 🔵 Azul | Controle de direção (amarrar DE e RE juntos) |

### ⚠️ Importante sobre DE e RE:
- **DE** (Driver Enable) e **RE** (Receiver Enable) devem ser conectados **juntos** ao GPIO 2
- **HIGH** (1) = modo transmissão
- **LOW** (0) = modo recepção
- Se seu módulo MAX485 tem pinos separados para DE e RE, solde-os juntos ou ligue ambos ao GPIO 2

---

## 📡 Conexões: MAX485 → Sensores RS-485

### Barramento RS-485

O RS-485 usa um par diferencial de fios (A e B) em topologia de barramento:

| MAX485 | Barramento RS-485 | Cor Sugerida |
|--------|-------------------|--------------|
| **A** | **A (todos os sensores)** | ⚪ Branco / 🟠 Laranja |
| **B** | **B (todos os sensores)** | ⚪ Cinza / 🟤 Marrom |

### Conexão dos Sensores (Paralelo)

```
MAX485 A ─────┬──── Anemômetro A
              │
              └──── Biruta A

MAX485 B ─────┬──── Anemômetro B
              │
              └──── Biruta B
```

- Todos os dispositivos RS-485 compartilham os mesmos fios A e B
- Cada sensor tem seu próprio **ID Modbus** (Anemômetro = 1, Biruta = 2)
- O mestre (ESP32) endereça cada sensor pelo ID

---

## 📊 Diagrama Completo de Conexões

```
┌─────────────────────────────────────────────────┐
│         Heltec WiFi LoRa 32 V2                  │
│                                                 │
│  3.3V ──────────────────────────┐               │
│  GND ───────────────────────────┼───────┐       │
│  GPIO 13 (TX2) ─────────────────┼───────┼───┐   │
│  GPIO 12 (RX2) ─────────────────┼───────┼───┼─┐ │
│  GPIO 2 (DE/RE) ────────────────┼───────┼───┼─┼─┤
└─────────────────────────────────┼───────┼───┼─┼─┘
                                  │       │   │ │ │
                    ┌─────────────┘       │   │ │ │
                    │    ┌────────────────┘   │ │ │
                    │    │    ┌───────────────┘ │ │
                    │    │    │    ┌────────────┘ │
                    │    │    │    │    ┌─────────┘
                    ↓    ↓    ↓    ↓    ↓
              ┌─────────────────────────────┐
              │      Módulo MAX485          │
              │                             │
              │  VCC    GND   DI   RO  DE/RE│
              │                             │
              │   A ─────────────────────┐  │
              │   B ─────────────────────┼──┤
              └──────────────────────────┼──┘
                                         │  │
                    ┌────────────────────┘  │
                    │    ┌──────────────────┘
                    ↓    ↓
              ╔═══════════════╗      ╔═══════════════╗
              ║  ANEMÔMETRO   ║      ║    BIRUTA     ║
              ║  (Modbus ID 1)║      ║ (Modbus ID 2) ║
              ║               ║      ║               ║
              ║   A       B   ║      ║   A       B   ║
              ╚═══════════════╝      ╚═══════════════╝
```

---

## 🛠️ Terminação do Barramento RS-485

### Por que é necessária?

A terminação elimina reflexões de sinal em comunicações de alta velocidade e longas distâncias.

### Como fazer:

1. **Resistor de 120Ω** entre os fios **A** e **B** nas **duas extremidades** do barramento
2. Se o cabo for curto (< 1 metro) e houver poucos dispositivos, 1 terminação pode ser suficiente
3. Alguns módulos MAX485 têm **jumpers de terminação** — habilite se necessário

```
[MAX485] ────A────[Sensor1]────[Sensor2]────A────[120Ω]
             │                                │
             B───────────────────────────────B
```

### Bias (opcional mas recomendado):

Para manter a linha definida quando ninguém transmite:

- **Pull-up em A**: resistor de 4.7kΩ a 10kΩ entre **A** e **VCC**
- **Pull-down em B**: resistor de 4.7kΩ a 10kΩ entre **B** e **GND**

> ⚠️ Verifique se seu módulo MAX485 já possui resistores de bias integrados antes de adicionar externos.

---

## ✅ Checklist de Montagem

- [ ] 1. **Desligue tudo** antes de conectar os fios
- [ ] 2. Conecte **GND** do Heltec ao **GND** do MAX485
- [ ] 3. Conecte **3.3V** do Heltec ao **VCC** do MAX485
- [ ] 4. Conecte **GPIO 13** ao **DI** do MAX485
- [ ] 5. Conecte **GPIO 12** ao **RO** do MAX485
- [ ] 6. Conecte **GPIO 2** aos pinos **DE** e **RE** do MAX485 (amarrados juntos)
- [ ] 7. Conecte **A** do MAX485 ao fio **A** dos sensores
- [ ] 8. Conecte **B** do MAX485 ao fio **B** dos sensores
- [ ] 9. Conecte os sensores em paralelo: **A↔A** e **B↔B**
- [ ] 10. Adicione resistor de **120Ω** entre **A** e **B** nas extremidades (se aplicável)
- [ ] 11. Verifique se os IDs Modbus dos sensores estão corretos (Anemômetro=1, Biruta=2)
- [ ] 12. Ligue a alimentação e teste com o monitor serial

---

## 🚨 Cuidados Importantes

### ⚡ Alimentação
- **NUNCA** alimente o MAX485 através de um pino GPIO
- Use sempre **3.3V** do regulador da placa
- Se o MAX485 for de 5V, use um conversor de nível lógico ou escolha um módulo 3.3V

### 🔗 Ground Comum
- **Obrigatório**: todos os dispositivos devem compartilhar o mesmo GND
- Sem ground comum, a comunicação RS-485 não funcionará

### 🔄 Polaridade A/B
- Mantenha consistência: **A↔A** e **B↔B** em todos os dispositivos
- Se um sensor não responder, tente inverter **A↔B** apenas naquele sensor

### 📏 Cabo RS-485
- Para distâncias > 1 metro: use **cabo par trançado** (twisted pair)
- Para ambientes ruidosos: use cabo **blindado** (STP)
- Evite passar cabos RS-485 perto de fontes de alta tensão

### 🔧 Controle DE/RE
- Verifique no código que o pino DE/RE está configurado corretamente:
  - **HIGH** antes de transmitir
  - **LOW** após transmitir (para receber respostas)
- O código usa `preTransmission()` e `postTransmission()` para isso

---

## 🧪 Teste de Funcionamento

Após conectar tudo:

1. **Compile e envie o código emissor**:
   ```bash
   pio run -e emissor --target upload
   pio device monitor -e emissor
   ```

2. **Verifique o monitor serial**:
   - Deve aparecer: "ESP1 - EMISSOR LoRa inicializado"
   - A cada 1 segundo: leituras de velocidade e direção do vento
   - Se aparecer "Erro na leitura": verifique conexões A/B e IDs Modbus

3. **Verifique o display OLED**:
   - Deve mostrar "ESP1 - EMISSOR"
   - Valores de velocidade (m/s) e direção (graus)
   - Se aparecer "Erro anemometro" ou "Erro biruta": problema na comunicação RS-485

---

## 🐛 Troubleshooting

### Problema: "Erro na leitura do anemômetro" ou "Erro na leitura da biruta"

**Possíveis causas:**
- Fios A e B invertidos
- Falta de terminação no barramento
- ID Modbus incorreto nos sensores
- Baudrate diferente (deve ser 9600)
- Ground não compartilhado
- Alimentação insuficiente

**Soluções:**
1. Verifique a polaridade A/B
2. Adicione resistor de 120Ω entre A e B
3. Confirme os IDs Modbus dos sensores (use software de teste Modbus)
4. Meça a tensão: 3.3V no VCC do MAX485
5. Conecte todos os GNDs

### Problema: Leituras erráticas ou valores aleatórios

**Possíveis causas:**
- Interferência eletromagnética
- Cabo muito longo sem terminação
- Falta de resistores de bias

**Soluções:**
1. Use cabo blindado (STP) e aterrar a malha
2. Adicione terminação de 120Ω nas extremidades
3. Adicione resistores de bias (pull-up em A, pull-down em B)
4. Reduza o comprimento do cabo se possível

### Problema: Monitor serial mostra caracteres estranhos

**Possíveis causas:**
- Baudrate incorreto no monitor serial
- Problema no cabo USB

**Soluções:**
1. Configure o monitor serial para **9600 baud**
2. Troque o cabo USB
3. Reinicie o ESP32

---

## 📚 Referências

- [Datasheet MAX485](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf)
- [Modbus RTU Protocol](https://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf)
- [RS-485 Application Note](https://www.ti.com/lit/an/slla070d/slla070d.pdf)
- [Heltec WiFi LoRa 32 V2 Pinout](https://resource.heltec.cn/download/WiFi_LoRa_32/WIFI_LoRa_32_V2.pdf)

---

## 📝 Notas Finais

- Este guia assume que você está usando o código nos arquivos `main_emissor.cpp` e `main_receptor.cpp`
- As configurações de pinagem estão definidas no início do código (DE_RE_PIN = 2, Serial2 nos pinos 12 e 13)
- Para alterar os pinos, edite as definições no código e atualize este documento

**Data de criação**: Outubro 2025  
**Versão**: 1.0
