#include <Arduino.h>

// === OLED Display ===
#include "heltec.h"

// === SD Card ===
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// === RTC DS3231 ===
#include <Wire.h>
#include <RTClib.h>

// === Configurações ===
// Pinos SD Card (conforme CONEXOES_RECEPTOR.md)
#define SD_CS   12  // CS
#define SD_MOSI 23  // MOSI
#define SD_MISO 13  // MISO
#define SD_SCK  17  // SCK

// Pinos RTC DS3231 (I2C)
#define RTC_SDA 4   // SDA
#define RTC_SCL 15  // SCL

// === Estrutura de dados para recepção ===
struct SensorData {
  uint32_t timestamp;
  float windSpeed;
  uint16_t windDirection;
  bool anemometroOK;
  bool birutaOK;
};

// Variáveis para dados recebidos
SensorData receivedData;
bool newDataReceived = false;
unsigned long lastDataReceived = 0;

// Variáveis de status do sistema
bool sdCardOK = false;
bool rtcOK = false;
RTC_DS3231 rtc;
DateTime now;

// SPI para SD Card
SPIClass spi1;

// Protótipos de funções
void saveDataToSD();
bool initSDCard();
bool initRTC();

void updateOLED() {
  Heltec.display->clear();
  
  // Título
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "ESP2 - RECEPTOR");
  
  // Linha divisória
  Heltec.display->drawHorizontalLine(0, 12, 128);
  
  // Status da comunicação
  unsigned long timeSinceLastData = millis() - lastDataReceived;
  if (timeSinceLastData < 10000 && newDataReceived) {
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 16, "Status: CONECTADO");
    
    // Velocidade do vento
    Heltec.display->drawString(0, 28, "Velocidade:");
    if (receivedData.anemometroOK) {
      String speedStr = String(receivedData.windSpeed, 1) + " m/s";
      Heltec.display->drawString(0, 40, speedStr);
    } else {
      Heltec.display->drawString(0, 40, "Erro anemometro");
    }
    
    // Direção do vento
    Heltec.display->drawString(0, 52, "Direcao:");
    if (receivedData.birutaOK) {
      String dirStr = String(receivedData.windDirection) + " graus";
      Heltec.display->drawString(70, 52, dirStr);
    } else {
      Heltec.display->drawString(70, 52, "Erro");
    }
    
  } else {
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 16, "Status: AGUARDANDO");
    Heltec.display->drawString(0, 28, "Dados do ESP1...");
  }
  
  Heltec.display->display();
}

void receiveLoRaData() {
  int packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(SensorData)) {
    // Lê os dados recebidos
    LoRa.readBytes((uint8_t*)&receivedData, sizeof(receivedData));
    newDataReceived = true;
    lastDataReceived = millis();
    
    Serial.println("Dados recebidos via LoRa:");
    Serial.printf("Timestamp: %lu\n", receivedData.timestamp);
    Serial.printf("Velocidade: %.1f m/s\n", receivedData.windSpeed);
    Serial.printf("Direção: %d graus\n", receivedData.windDirection);
    Serial.printf("Status: Anem=%s, Biruta=%s\n", 
                  receivedData.anemometroOK ? "OK" : "ERRO",
                  receivedData.birutaOK ? "OK" : "ERRO");
    Serial.printf("RSSI: %d dBm\n", LoRa.packetRssi());
    Serial.println("---");
    
    // Salva no SD Card (se disponível)
    if (sdCardOK) {
      saveDataToSD();
    }
    
  } else if (packetSize > 0) {
    Serial.printf("Pacote com tamanho incorreto: %d bytes (esperado: %d)\n",
                  packetSize, sizeof(SensorData));
    // Limpa o buffer
    while (LoRa.available()) {
      LoRa.read();
    }
  }
}

void saveDataToSD() {
  // Atualiza hora do RTC (se disponível)
  if (rtcOK) {
    now = rtc.now();
  }
  
  // Abre arquivo CSV
  File file = SD.open("/wind_data.csv", FILE_APPEND);
  if (!file) {
    Serial.println("❌ Erro ao abrir CSV");
    sdCardOK = false;
    return;
  }
  
  // Formata linha CSV
  char csvLine[200];
  if (rtcOK) {
    snprintf(csvLine, sizeof(csvLine), 
             "%04d-%02d-%02d,%02d:%02d:%02d,%lu,%.1f,%d,%d,%d,%d\n",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second(),
             receivedData.timestamp, receivedData.windSpeed,
             receivedData.windDirection, receivedData.anemometroOK,
             receivedData.birutaOK, LoRa.packetRssi());
  } else {
    snprintf(csvLine, sizeof(csvLine), 
             "N/A,N/A,%lu,%.1f,%d,%d,%d,%d\n",
             receivedData.timestamp, receivedData.windSpeed,
             receivedData.windDirection, receivedData.anemometroOK,
             receivedData.birutaOK, LoRa.packetRssi());
  }
  
  file.print(csvLine);
  file.close();
  Serial.println("✅ Salvo no SD");
}

bool initSDCard() {
  Serial.println("Inicializando SD...");
  
  // Configuração SPI igual ao exemplo (SCK=17, MISO=13, MOSI=23, CS=12)
  spi1.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS, spi1)) {
    Serial.println("❌ SD falhou");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ Sem cartão SD");
    return false;
  }
  
  Serial.println("✅ SD OK");
  
  // Cria CSV com cabeçalhos se não existir
  if (!SD.exists("/wind_data.csv")) {
    File file = SD.open("/wind_data.csv", FILE_WRITE);
    if (file) {
      file.println("Data,Hora,Timestamp_ms,Velocidade_ms,Direcao_graus,Anem_OK,Biruta_OK,RSSI_dBm");
      file.close();
      Serial.println("✅ CSV criado");
    }
  }
  
  return true;
}

bool initRTC() {
  Serial.println("Inicializando RTC...");
  
  // Inicializa I2C com os pinos corretos
  Wire.begin(RTC_SDA, RTC_SCL);
  
  if (!rtc.begin()) {
    Serial.println("❌ RTC não encontrado");
    return false;
  }
  
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC perdeu energia, ajustando para data de compilação");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  now = rtc.now();
  Serial.printf("✅ RTC OK - %04d-%02d-%02d %02d:%02d:%02d\n",
                now.year(), now.month(), now.day(),
                now.hour(), now.minute(), now.second());
  
  return true;
}

void setup() {
  // Inicializa o Heltec (OLED habilitado, LoRa HABILITADO, Serial DESABILITADO)
  Heltec.begin(true /*DisplayEnable*/, true /*LoRa*/, false /*Serial*/, true /*PABOOST*/, 470E6 /*Band*/);
  
  // Configura o Serial manualmente
  Serial.begin(9600);
  
  Serial.println("ESP2 - RECEPTOR LoRa inicializado");
  Serial.printf("Frequência LoRa: %lu Hz\n", 470000000UL);
  Serial.printf("Tamanho esperado do pacote: %d bytes\n", sizeof(SensorData));
  
  // Tela inicial
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "ESP2 - RECEPTOR");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 15, "Inicializando...");
  Heltec.display->display();
  delay(2000);
  
  // Inicializa dados padrão
  receivedData.timestamp = 0;
  receivedData.windSpeed = 0.0;
  receivedData.windDirection = 0;
  receivedData.anemometroOK = false;
  receivedData.birutaOK = false;
  
  Serial.println("\n=== INICIALIZANDO RECURSOS ===");
  
  // === INICIALIZA RTC DS3231 ===
  rtcOK = initRTC();
  if (!rtcOK) {
    Serial.println("⚠️ Timestamps serão relativos");
  }
  
  // === INICIALIZA SD CARD ===
  sdCardOK = initSDCard();
  if (!sdCardOK) {
    Serial.println("⚠️ Sistema funcionará SEM armazenamento");
  }
  
  Serial.println("\n✅ Sistema pronto!\n");
}

void loop() {
  // Verifica se há dados LoRa para receber
  receiveLoRaData();
  
  // Atualiza o display OLED
  updateOLED();
  
  delay(100);
}
