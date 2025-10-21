# 📊 Receptor com SD Card, WiFi e Tratamento de Erros

## 🎯 O que foi implementado

O código `main_receptor.cpp` agora inclui:

1. ✅ **Salvamento em SD Card** (banco de dados CSV)
2. ✅ **Sincronização de tempo** via WiFi/NTP
3. ✅ **Tratamento completo de erros** (SD, WiFi, LoRa)
4. ✅ **Display OLED com ícones de status**
5. ✅ **Logs detalhados no Serial Monitor**

---

## 🗂️ Banco de Dados CSV

### **Arquivo:** `/wind_data.csv`

### **Cabeçalhos:**
```csv
Data,Hora,Timestamp_ms,Velocidade_ms,Direcao_graus,Anem_OK,Biruta_OK,RSSI_dBm
```

### **Exemplo de dados:**
```csv
2025-10-21,14:35:22,15340,12.5,270,1,1,-45
2025-10-21,14:35:27,20450,13.2,265,1,1,-47
2025-10-21,14:35:32,25560,11.8,272,1,1,-43
```

### **Campos:**
| Campo | Tipo | Descrição | Exemplo |
|-------|------|-----------|---------|
| `Data` | String | Data no formato AAAA-MM-DD | `2025-10-21` |
| `Hora` | String | Hora no formato HH:MM:SS | `14:35:22` |
| `Timestamp_ms` | uint32 | Millis do ESP1 emissor | `15340` |
| `Velocidade_ms` | float | Velocidade do vento (m/s) | `12.5` |
| `Direcao_graus` | uint16 | Direção do vento (0-360°) | `270` |
| `Anem_OK` | bool | Status anemômetro (0=erro, 1=ok) | `1` |
| `Biruta_OK` | bool | Status biruta (0=erro, 1=ok) | `1` |
| `RSSI_dBm` | int | Potência do sinal LoRa | `-45` |

---

## 🔌 Pinagem do SD Card

### **Conexões SPI:**

| SD Card | ESP32 Heltec | Pino GPIO |
|---------|--------------|-----------|
| **MISO** | MISO | GPIO 19 |
| **MOSI** | MOSI | GPIO 27 |
| **SCK** | SCK | GPIO 5 |
| **CS** | CS | GPIO 17 |
| **VCC** | 3.3V | 3.3V |
| **GND** | GND | GND |

⚠️ **Importante:** Verifique se os pinos estão corretos para o seu módulo SD Card!

---

## 📡 WiFi e NTP

### **Configuração:**
```cpp
const char* ssid = "AndroidAP3958";      // ← Altere para sua rede
const char* password = "99154145";       // ← Altere sua senha

const char* ntpServer = "pool.ntp.org";  // Servidor NTP
const long gmtOffset_sec = -3 * 3600;     // UTC-3 (Brasília)
```

### **Como funciona:**
1. **Setup:** ESP2 conecta ao WiFi
2. **NTP Sync:** Obtém data/hora do servidor NTP
3. **Loop:** Atualiza hora a cada salvamento no SD

### **Se WiFi falhar:**
- ⚠️ Sistema continua funcionando
- ⚠️ Timestamps serão `N/A` no CSV
- ⚠️ Display mostra ícone "WiFi:X"

---

## 🛡️ Tratamento de Erros

### **1. Erro de SD Card**

#### **Possíveis causas:**
- Cartão não inserido
- Pinos SPI incorretos
- Cartão corrompido
- Alimentação insuficiente

#### **Como detectar:**
```cpp
if (!sdCardOK) {
  Serial.println("❌ SD Card indisponível");
  errorMsg = "Erro SD: Não montou";
}
```

#### **Comportamento:**
- ⚠️ Dados LoRa continuam sendo recebidos
- ⚠️ Dados NÃO são salvos
- ⚠️ Display mostra "SD:X"
- ⚠️ Serial mostra mensagem de erro

#### **Códigos de erro no Serial:**
```
❌ Falha ao montar SD Card
❌ Nenhum cartão SD detectado
❌ Erro ao criar arquivo CSV
❌ Erro ao abrir arquivo CSV
```

---

### **2. Erro de WiFi**

#### **Possíveis causas:**
- SSID/senha incorretos
- Rede WiFi fora de alcance
- Roteador desligado

#### **Como detectar:**
```cpp
if (!wifiConnected) {
  Serial.println("❌ Falha ao conectar WiFi");
  errorMsg = "Erro WiFi: Timeout";
}
```

#### **Comportamento:**
- ⚠️ Sistema funciona sem WiFi
- ⚠️ NTP não será sincronizado
- ⚠️ Timestamps no CSV serão `N/A`
- ⚠️ Display mostra "WiFi:X"

#### **Tentativas de reconexão:**
- 20 tentativas (10 segundos total)
- Se falhar, continua sem WiFi

---

### **3. Erro de NTP**

#### **Possíveis causas:**
- WiFi desconectado
- Servidor NTP inacessível
- Timeout na resposta

#### **Como detectar:**
```cpp
if (!ntpSynced) {
  Serial.println("❌ Falha ao sincronizar NTP");
  errorMsg = "Erro NTP: Timeout";
}
```

#### **Comportamento:**
- ⚠️ Timestamps relativos (millis do ESP1)
- ⚠️ Data/Hora serão `N/A` no CSV
- ⚠️ Display mostra "NTP:X"

---

### **4. Erro de LoRa**

#### **Possíveis causas:**
- Pacote corrompido
- Tamanho incorreto
- Interferência

#### **Como detectar:**
```cpp
if (packetSize != sizeof(SensorData)) {
  Serial.printf("❌ Pacote inválido: %d bytes (esperado: %d)\n", 
                packetSize, sizeof(SensorData));
  errorMsg = "Erro: Pacote corrompido";
}
```

#### **Comportamento:**
- ⚠️ Pacote é descartado
- ⚠️ Buffer LoRa é limpo
- ⚠️ Display mostra última leitura válida
- ⚠️ Serial mostra mensagem de erro

#### **Indicadores de qualidade:**
- **RSSI** (Received Signal Strength Indicator)
  - > -50 dBm: Excelente
  - -50 a -70 dBm: Bom
  - -70 a -90 dBm: Regular
  - < -90 dBm: Ruim

- **SNR** (Signal-to-Noise Ratio)
  - > 10 dB: Excelente
  - 5 a 10 dB: Bom
  - 0 a 5 dB: Regular
  - < 0 dB: Ruim

---

## 🖥️ Display OLED

### **Layout:**

```
┌──────────────────────────┐
│ ESP2 - RECEPTOR          │  ← Título
├──────────────────────────┤
│ Vel: 12.5 m/s            │  ← Velocidade
│ Dir: 270 graus           │  ← Direção
│ 14:35:22                 │  ← Hora (se NTP OK)
│                          │
├──────────────────────────┤
│ WiFi:OK  SD:OK  -45dB    │  ← Status
└──────────────────────────┘
```

### **Estados possíveis:**

#### **1. Aguardando dados**
```
ESP2 - RECEPTOR
─────────────────
AGUARDANDO...

─────────────────
WiFi:OK  SD:OK
```

#### **2. Recebendo dados**
```
ESP2 - RECEPTOR
─────────────────
Vel: 12.5 m/s
Dir: 270 graus
14:35:22
─────────────────
WiFi:OK  SD:OK  -45dB
```

#### **3. Sem WiFi/SD**
```
ESP2 - RECEPTOR
─────────────────
Vel: 12.5 m/s
Dir: 270 graus

─────────────────
WiFi:X  SD:X  -45dB
```

---

## 📋 Serial Monitor

### **Exemplo de log completo:**

```
=================================
   ESP2 - RECEPTOR LoRa v2.0
=================================

Inicializando SD Card...
✅ SD Card detectado: SDHC
Tamanho: 8192 MB
✅ Arquivo CSV criado com cabeçalhos

Conectando ao WiFi...
.........
✅ WiFi conectado
IP: 192.168.1.100

Sincronizando NTP...
...
✅ NTP sincronizado
Hora atual: 2025-10-21 14:35:15

--- Configuração LoRa ---
Frequência: 470000000 Hz
Tamanho do pacote: 13 bytes
-------------------------

✅ Sistema iniciado e pronto para receber dados!

=== DADOS RECEBIDOS ===
Timestamp: 15340 ms
Velocidade: 12.5 m/s (OK)
Direção: 270 graus (OK)
RSSI: -45 dBm | SNR: 8.5 dB
=======================
✅ Dados salvos no SD Card

=== DADOS RECEBIDOS ===
Timestamp: 20450 ms
Velocidade: 13.2 m/s (OK)
Direção: 265 graus (OK)
RSSI: -47 dBm | SNR: 7.2 dB
=======================
✅ Dados salvos no SD Card
```

### **Com erros:**

```
Inicializando SD Card...
❌ Falha ao montar SD Card
⚠️ Sistema funcionará SEM armazenamento

Conectando ao WiFi...
......................
❌ Falha ao conectar WiFi
⚠️ Sistema funcionará SEM WiFi
⚠️ Timestamps não serão precisos

=== DADOS RECEBIDOS ===
Timestamp: 15340 ms
Velocidade: 12.5 m/s (OK)
Direção: 270 graus (OK)
RSSI: -45 dBm | SNR: 8.5 dB
=======================
⚠️ SD Card indisponível - dados não salvos!
```

---

## 🔧 Funções Principais

### **1. `initSDCard()`**
```cpp
bool initSDCard()
```
- Inicializa SPI e monta o SD Card
- Detecta tipo de cartão (MMC, SD, SDHC)
- Cria arquivo CSV com cabeçalhos
- Retorna `true` se sucesso, `false` se falha

### **2. `initWiFi()`**
```cpp
bool initWiFi()
```
- Conecta à rede WiFi configurada
- Timeout de 10 segundos (20 tentativas)
- Retorna `true` se conectado, `false` se falha

### **3. `syncNTP()`**
```cpp
bool syncNTP()
```
- Sincroniza data/hora com servidor NTP
- Timeout de 5 segundos (10 tentativas)
- Retorna `true` se sincronizado, `false` se falha

### **4. `saveDataToSD()`**
```cpp
void saveDataToSD()
```
- Formata linha CSV com todos os campos
- Usa timestamp real (se NTP OK) ou relativo
- Append no arquivo `/wind_data.csv`
- Atualiza flags de erro se falhar

### **5. `receiveLoRaData()`**
```cpp
void receiveLoRaData()
```
- Verifica se há pacote LoRa
- Valida tamanho do pacote
- Deserializa struct `SensorData`
- Chama `saveDataToSD()` se SD OK
- Limpa buffer se pacote inválido

### **6. `updateOLED()`**
```cpp
void updateOLED()
```
- Atualiza display com dados mais recentes
- Mostra ícones de status (WiFi, SD, RSSI)
- Formata velocidade e direção
- Exibe hora atual (se NTP OK)

---

## 🧪 Como Testar

### **1. Teste sem SD Card**
```bash
# Não insira o cartão SD
# Resultado esperado:
❌ Nenhum cartão SD detectado
⚠️ Sistema funcionará SEM armazenamento
WiFi:OK  SD:X
```

### **2. Teste sem WiFi**
```cpp
// Altere para SSID inexistente
const char* ssid = "RedeQueNaoExiste";

// Resultado esperado:
❌ Falha ao conectar WiFi
⚠️ Timestamps serão relativos
WiFi:X  SD:OK
```

### **3. Teste com tudo OK**
```bash
# SD Card inserido, WiFi correto
# Resultado esperado:
✅ SD Card detectado
✅ WiFi conectado
✅ NTP sincronizado
WiFi:OK  SD:OK  -45dB
```

### **4. Teste de pacote corrompido**
```cpp
// Simule enviando pacote de tamanho diferente do ESP1
// Resultado esperado no ESP2:
❌ Pacote inválido: 10 bytes (esperado: 13)
Erro: Pacote corrompido
```

---

## 📊 Análise de Dados (CSV)

### **Importar no Excel/Google Sheets:**
1. Abra o arquivo `wind_data.csv` do SD Card
2. As colunas já estarão separadas por vírgula
3. Use filtros para análise

### **Análise Python (exemplo):**
```python
import pandas as pd

# Lê o CSV
df = pd.read_csv('wind_data.csv')

# Estatísticas
print(df['Velocidade_ms'].describe())
print(f"Direção média: {df['Direcao_graus'].mean()}°")
print(f"RSSI médio: {df['RSSI_dBm'].mean()} dBm")

# Gráfico
df.plot(x='Hora', y='Velocidade_ms', kind='line')
```

---

## 🚀 Melhorias Futuras

### **Fase 3: Interface Web**
- [ ] Criar servidor HTTP no ESP2
- [ ] Página web para visualização em tempo real
- [ ] Botão de download do CSV
- [ ] Gráficos interativos

### **Fase 4: Backup e Redundância**
- [ ] Rotação automática de arquivos CSV (por dia/semana)
- [ ] Compressão de arquivos antigos
- [ ] Upload automático para nuvem (se WiFi OK)
- [ ] Buffer de dados em RAM (se SD falhar temporariamente)

### **Fase 5: Alertas**
- [ ] Notificações por email/telegram
- [ ] Alarme sonoro se velocidade > X m/s
- [ ] LED indicador de status

---

## 🔍 Troubleshooting Avançado

### **SD Card não detectado mas está inserido**
1. Verifique pinagem SPI (MISO, MOSI, SCK, CS)
2. Formate o cartão como **FAT32**
3. Use cartão de até **32GB** (SDHC)
4. Verifique alimentação (3.3V estável)

### **WiFi conecta mas NTP falha**
1. Teste com outro servidor NTP: `time.nist.gov`
2. Verifique firewall do roteador
3. Aumente timeout de 5s para 10s

### **Dados salvos mas CSV corrompido**
1. Sempre use `file.close()` após escrever
2. Não remova SD Card com sistema ligado
3. Use `SD.end()` antes de desligar

---

## 📝 Notas Importantes

1. **Sempre use `file.close()`** após operações de escrita
2. **SD Card deve ser FAT32** (máximo 32GB)
3. **WiFi é opcional** - sistema funciona sem
4. **NTP melhora precisão** mas não é obrigatório
5. **RSSI < -90 dBm** indica comunicação instável

---

**Última atualização:** Outubro 2025  
**Versão:** 2.0 (com SD Card e WiFi/NTP)
