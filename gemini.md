# 🌬️ Sistema de Monitoramento de Vento via LoRa

## 📋 Visão Geral do Projeto

Sistema de monitoramento remoto de velocidade e direção do vento usando dois **ESP32 Heltec WiFi LoRa 32 V2**, comunicação **Modbus RS-485** com sensores e transmissão de dados via **LoRa** (470 MHz).

---

## 🎯 Objetivo

Criar um sistema distribuído onde:
1. **ESP1 (Emissor)** lê sensores de vento via RS-485 e transmite os dados via LoRa
2. **ESP2 (Receptor)** recebe os dados via LoRa e exibe em tempo real no OLED
3. Futuramente: ESP2 salvará dados em SD Card e disponibilizará via interface web para download CSV

---

## 🏗️ Arquitetura do Sistema

```
┌─────────────────────────────────────┐
│         ESP1 - EMISSOR              │
│  Heltec WiFi LoRa 32 V2             │
│                                     │
│  ┌─────────────┐   ┌──────────┐    │
│  │   MAX485    │   │   OLED   │    │
│  │  (RS-485)   │   │ Display  │    │
│  └──────┬──────┘   └──────────┘    │
│         │                           │
│    ┌────┴────┐                      │
│    │         │                      │
│  Anem.    Biruta                    │
│  (ID 1)   (ID 2)                    │
│                                     │
│         LoRa TX (470 MHz)           │
└────────────┬────────────────────────┘
             │
             │  ~10 km alcance
             │
             ▼
┌─────────────────────────────────────┐
│         ESP2 - RECEPTOR             │
│  Heltec WiFi LoRa 32 V2             │
│                                     │
│  ┌──────────┐                       │
│  │   OLED   │  (futuro: SD Card)    │
│  │ Display  │  (futuro: WiFi AP)    │
│  └──────────┘  (futuro: Web Server) │
│                                     │
│         LoRa RX (470 MHz)           │
└─────────────────────────────────────┘
```

---

## 🔧 Hardware Utilizado

### ESP1 - Emissor
| Componente | Modelo/Especificação | Função |
|------------|---------------------|--------|
| Microcontrolador | Heltec WiFi LoRa 32 V2 | Controlador principal |
| Display | OLED 128x64 (integrado) | Mostra leituras locais |
| Módulo LoRa | SX1276/SX1278 470 MHz (integrado) | Transmite dados |
| Conversor RS-485 | MAX485 | Interface com sensores Modbus |
| Sensor 1 | Anemômetro (Modbus ID 1) | Mede velocidade do vento |
| Sensor 2 | Biruta (Modbus ID 2) | Mede direção do vento |

### ESP2 - Receptor
| Componente | Modelo/Especificação | Função |
|------------|---------------------|--------|
| Microcontrolador | Heltec WiFi LoRa 32 V2 | Controlador principal |
| Display | OLED 128x64 (integrado) | Mostra dados recebidos |
| Módulo LoRa | SX1276/SX1278 470 MHz (integrado) | Recebe dados |

---

## 📡 Comunicação

### 1. **Modbus RS-485** (ESP1 ↔ Sensores)
- **Protocolo:** Modbus RTU
- **Interface física:** RS-485 (par diferencial A/B)
- **Conversor:** MAX485
- **Baudrate:** 9600 bps
- **Pinos ESP32:**
  - TX (GPIO 13) → MAX485 DI
  - RX (GPIO 12) → MAX485 RO
  - DE/RE (GPIO 23) → MAX485 DE+RE (controle de direção)
- **IDs Modbus:**
  - Anemômetro: `0x01`
  - Biruta: `0x02`

### 2. **LoRa** (ESP1 → ESP2)
- **Frequência:** 470 MHz
- **Modulação:** LoRa (Chirp Spread Spectrum)
- **Alcance:** até 10 km (linha de visada)
- **Formato de dados:** Struct binária (`SensorData`)
- **Intervalo de transmissão:** 5 segundos

---

## 📦 Estrutura de Dados Transmitida

```cpp
struct SensorData {
  uint32_t timestamp;          // Tempo desde boot (ms)
  float windSpeed;             // Velocidade do vento (m/s)
  uint16_t windDirection;      // Direção do vento (graus, 0-360)
  bool anemometroOK;          // Status do sensor de velocidade
  bool birutaOK;              // Status do sensor de direção
};
// Tamanho total: 13 bytes
```

---

## 📁 Estrutura do Projeto

```
leitura_anem_biruta/
├── platformio.ini              # Configuração do PlatformIO (2 ambientes)
├── CONEXOES.md                 # Guia de conexões de hardware
├── gemini.md                   # Este arquivo (documentação do projeto)
├── main.cpp                    # Código legado (backup na raiz)
├── src/
│   ├── main_emissor.cpp        # ESP1: Lê sensores + Transmite LoRa
│   ├── main_receptor.cpp       # ESP2: Recebe LoRa + Exibe dados
│   └── main_backup.cpp         # Backup do código original (sem LoRa)
├── lib/                        # Bibliotecas customizadas (vazio)
├── include/                    # Headers customizados (vazio)
└── test/                       # Testes unitários (vazio)
```

---

## 🚀 Como Compilar e Enviar

### Pré-requisitos
- **PlatformIO Core** ou **VS Code + extensão PlatformIO**
- **Cabo USB** para cada ESP32
- **Sensores Modbus** configurados (IDs 1 e 2)
- **Dois ESP32 Heltec WiFi LoRa 32 V2**

### Programar ESP1 (Emissor)

1. **Conecte o ESP1** via USB
2. **Selecione o ambiente `emissor`:**
   - No VS Code: `Ctrl+Shift+P` → `PlatformIO: Switch Project Environment` → `emissor`
   - Ou no terminal:
     ```bash
     pio run -e emissor --target upload
     ```
3. **Monitor serial** (opcional):
   ```bash
   pio device monitor -e emissor
   ```

### Programar ESP2 (Receptor)

1. **Conecte o ESP2** via USB
2. **Selecione o ambiente `receptor`:**
   - No VS Code: `Ctrl+Shift+P` → `PlatformIO: Switch Project Environment` → `receptor`
   - Ou no terminal:
     ```bash
     pio run -e receptor --target upload
     ```
3. **Monitor serial** (opcional):
   ```bash
   pio device monitor -e receptor
   ```

---

## 🔍 Funcionamento Detalhado

### **ESP1 - Emissor** (`main_emissor.cpp`)

#### 1. **Inicialização** (`setup()`)
```cpp
- Inicializa OLED (128x64)
- Inicializa LoRa (470 MHz, PABOOST ativado)
- Configura Serial2 (RS-485): 9600 bps, pinos 12/13
- Configura pino DE/RE (GPIO 23) para controle MAX485
- Inicializa Modbus Master para anemômetro (ID 1) e biruta (ID 2)
- Exibe tela de boot no OLED
```

#### 2. **Loop Principal** (`loop()`)
```cpp
A cada 1 segundo:
  1. Lê anemômetro (registro 0x0000)
     └─ Converte valor bruto para m/s (rawSpeed / 10.0)
  
  2. Lê biruta (registro 0x0001)
     └─ Obtém direção em graus (0-360)
  
  3. Atualiza OLED com valores lidos
  
  4. A cada 5 segundos:
     └─ Cria struct SensorData
     └─ Envia via LoRa (13 bytes)
     └─ Imprime log no Serial
```

#### 3. **Controle MAX485**
```cpp
preTransmission():  GPIO 23 = HIGH (modo transmissão)
postTransmission(): GPIO 23 = LOW  (modo recepção)
```

---

### **ESP2 - Receptor** (`main_receptor.cpp`)

#### 1. **Inicialização** (`setup()`)
```cpp
- Inicializa OLED (128x64)
- Inicializa LoRa (470 MHz, PABOOST ativado)
- Configura Serial (9600 bps) para debug
- Exibe tela de boot no OLED
```

#### 2. **Loop Principal** (`loop()`)
```cpp
A cada 100 ms:
  1. Verifica se há pacote LoRa disponível
  
  2. Se recebeu pacote:
     └─ Valida tamanho (deve ser 13 bytes)
     └─ Deserializa struct SensorData
     └─ Atualiza timestamp da última recepção
     └─ Imprime dados + RSSI no Serial
  
  3. Atualiza OLED:
     └─ Se recebeu dados recentemente (<10s):
        └─ Mostra "CONECTADO"
        └─ Exibe velocidade, direção e status
     └─ Senão:
        └─ Mostra "AGUARDANDO dados do ESP1"
```

---

## 📊 Informações Exibidas

### **OLED ESP1 (Emissor)**
```
┌────────────────────┐
│ ESP1 - EMISSOR     │
├────────────────────┤
│ Velocidade:        │
│ 12.5 m/s           │
│ Direcao:           │
│ 270 graus          │
└────────────────────┘
```

### **OLED ESP2 (Receptor)**
```
┌────────────────────┐
│ ESP2 - RECEPTOR    │
├────────────────────┤
│ Status: CONECTADO  │
│ Velocidade:        │
│ 12.5 m/s           │
│ Direcao: 270 graus │
└────────────────────┘
```

### **Monitor Serial ESP1**
```
Velocidade do vento: 12.5 m/s
Direção do vento (raw): 270
Dados enviados via LoRa:
Timestamp: 15340
Velocidade: 12.5 m/s
Direção: 270 graus
Status: Anem=OK, Biruta=OK
---
```

### **Monitor Serial ESP2**
```
Dados recebidos via LoRa:
Timestamp: 15340
Velocidade: 12.5 m/s
Direção: 270 graus
Status: Anem=OK, Biruta=OK
RSSI: -45 dBm
---
```

---

## 🛠️ Configuração do PlatformIO

### **`platformio.ini`**

```ini
[env:emissor]
platform = espressif32
board = heltec_wifi_lora_32_V2
framework = arduino
monitor_speed = 9600
build_src_filter = +<main_emissor.cpp> -<main.cpp> -<main_receptor.cpp> -<main_backup.cpp>
lib_deps = 
    4-20ma/ModbusMaster@^2.0.1
    heltecautomation/Heltec ESP32 Dev-Boards@^1.1.1

[env:receptor]
platform = espressif32
board = heltec_wifi_lora_32_V2
framework = arduino
monitor_speed = 9600
build_src_filter = +<main_receptor.cpp> -<main.cpp> -<main_emissor.cpp> -<main_backup.cpp>
lib_deps = 
    4-20ma/ModbusMaster@^2.0.1
    heltecautomation/Heltec ESP32 Dev-Boards@^1.1.1
```

**Explicação:**
- `build_src_filter`: Garante que apenas **um** arquivo `.cpp` seja compilado por ambiente
- `lib_deps`: Bibliotecas necessárias (Modbus e Heltec LoRa/OLED)

---

## 📚 Bibliotecas Utilizadas

| Biblioteca | Versão | Uso |
|------------|--------|-----|
| **ModbusMaster** | 2.0.1 | Comunicação Modbus RTU com sensores |
| **Heltec ESP32 Dev-Boards** | 1.1.1 | Controle OLED + LoRa |
| **Arduino Framework** | 3.20017+ | Base do firmware ESP32 |

---

## 🔧 Troubleshooting

### ❌ **Erro: "multiple definition of..."**
**Causa:** PlatformIO está compilando vários arquivos `.cpp` ao mesmo tempo.

**Solução:** Verifique o `build_src_filter` no `platformio.ini`. Cada ambiente deve compilar **apenas um** arquivo principal.

---

### ❌ **Erro: "Could not open COM4"**
**Causa:** Porta serial em uso (monitor serial aberto) ou ESP desconectado.

**Solução:**
1. Feche todos os monitores seriais
2. Desconecte e reconecte o cabo USB
3. Verifique a porta correta com `pio device list`

---

### ❌ **ESP2 mostra "AGUARDANDO" o tempo todo**
**Causa:** Comunicação LoRa não está funcionando.

**Solução:**
1. Verifique se **ambos** os ESPs estão configurados para **470 MHz**
2. Verifique se as **antenas** estão conectadas
3. Aproxime os ESPs para teste (distância < 5 metros)
4. Verifique o **RSSI** no monitor serial do ESP2 (deve ser > -100 dBm)

---

### ❌ **Leituras Modbus falham (Erro 0xE2)**
**Causa:** Comunicação RS-485 com problema.

**Solução:**
1. Verifique **polaridade A/B** (troque se necessário)
2. Adicione **resistor de terminação** 120Ω entre A e B
3. Confirme **IDs Modbus** dos sensores (1 e 2)
4. Verifique **baudrate** (9600 bps)
5. Meça **tensão no MAX485** (deve ser 3.3V)

---

## 🚧 Próximas Etapas (Roadmap)

### **Fase 2: Armazenamento de Dados**
- [ ] Adicionar módulo **SD Card** no ESP2
- [ ] Salvar leituras em arquivo **CSV** com timestamps
- [ ] Implementar **RTC DS3231** para timestamps precisos

### **Fase 3: Interface Web**
- [ ] Criar **WiFi AP** no ESP2
- [ ] Servidor web para **visualização em tempo real**
- [ ] Botão de **download CSV** via browser
- [ ] Gráficos de **histórico** de velocidade/direção

### **Fase 4: Otimizações**
- [ ] Implementar **deep sleep** no ESP1 para economizar bateria
- [ ] Adicionar **ACK** (confirmação de recebimento) no protocolo LoRa
- [ ] Compressão de dados para **maior alcance**
- [ ] **OTA** (Over-The-Air) updates via WiFi

---

## 📝 Notas Técnicas

### **Por que GPIO 23 para DE/RE?**
Inicialmente era GPIO 2, mas foi alterado para GPIO 23 para evitar conflitos com boot mode do ESP32 (GPIO 2 tem pull-up interno e pode interferir).

### **Por que 470 MHz?**
- Módulo LoRa do Heltec suporta 410-525 MHz
- 470 MHz tem boa propagação e menos interferência no Brasil
- Para uso comercial, considere **915 MHz** (frequência liberada pela Anatel)

### **Por que struct binária ao invés de JSON?**
- **Menor tamanho:** 13 bytes vs ~80 bytes (JSON)
- **Menor consumo de energia:** menos tempo de transmissão
- **Maior alcance:** pacotes menores têm melhor SNR

### **Limitações Atuais**
- Sem autenticação/criptografia na comunicação LoRa
- Sem sincronização de tempo entre ESP1 e ESP2
- Sem recuperação de falhas (se perder pacote, perde dado)
- Sem persistência de dados (tudo é volátil)

---

## 📞 Suporte e Documentação

- **Heltec WiFi LoRa 32 V2:** [Docs oficiais](https://heltec.org/project/wifi-lora-32/)
- **LoRa Library:** [Sandeep Mistry LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- **ModbusMaster:** [4-20ma/ModbusMaster](https://github.com/4-20ma/ModbusMaster)
- **RS-485:** [TI Application Note](https://www.ti.com/lit/an/slla070d/slla070d.pdf)

---

## 📄 Licença

Este projeto é de código aberto para fins educacionais e experimentais.

---

## 🏆 Status do Projeto

✅ **Comunicação LoRa:** Funcionando  
✅ **Leitura Modbus:** Implementada  
✅ **Display OLED:** Funcionando  
⏳ **Armazenamento SD:** Planejado  
⏳ **Interface Web:** Planejado  

**Última atualização:** Outubro 2025  
**Versão:** 1.0 (MVP)
