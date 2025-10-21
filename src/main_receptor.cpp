#include <Arduino.h>

// === OLED Display ===
#include "heltec.h"

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
  } else if (packetSize > 0) {
    Serial.printf("Pacote com tamanho incorreto: %d bytes (esperado: %d)\n", 
                  packetSize, sizeof(SensorData));
    // Limpa o buffer
    while (LoRa.available()) {
      LoRa.read();
    }
  }
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
}

void loop() {
  // Verifica se há dados LoRa para receber
  receiveLoRaData();
  
  // Atualiza o display OLED
  updateOLED();
  
  delay(100);
}
