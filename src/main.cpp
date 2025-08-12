#include <Arduino.h>

// === OLED Display ===
#include "heltec.h"

// === DIREÇÃO DO VENTO E ANEMÔMETRO (Modbus RS485) ===
#include <ModbusMaster.h>

#define DE_RE_PIN 2  // Pino para controlar DE e RE do MAX485

ModbusMaster nodeAnemometro;   // Dispositivo Modbus ID 1 (velocidade do vento)
ModbusMaster nodeBiruta;       // Dispositivo Modbus ID 2 (direção do vento)

// Variáveis para armazenar os últimos valores lidos
float lastWindSpeed = 0.0;
uint16_t lastWindDirection = 0;
bool anemometroOK = false;
bool birutaOK = false;

void preTransmission() {
  digitalWrite(DE_RE_PIN, HIGH);  // Habilita transmissão
}

void postTransmission() {
  digitalWrite(DE_RE_PIN, LOW);   // Habilita recepção
}

void updateOLED() {
  Heltec.display->clear();
  
  // Título menor
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Monitor Vento");
  
  // Linha divisória
  Heltec.display->drawHorizontalLine(0, 12, 128);
  
  // Velocidade do vento
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 16, "Velocidade:");
  if (anemometroOK) {
    String speedStr = String(lastWindSpeed, 1) + " m/s";
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 28, speedStr);
  } else {
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 28, "Erro anemometro");
  }
  
  // Direção do vento
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 42, "Direcao:");
  if (birutaOK) {
    String dirStr = String(lastWindDirection) + " graus";
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 54, dirStr);
  } else {
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 54, "Erro biruta");
  }
  
  Heltec.display->display();
}

void setup() {
  // Inicializa o Heltec (OLED habilitado, LoRa desabilitado, Serial DESABILITADO)
  Heltec.begin(true /*DisplayEnable*/, false /*LoRa*/, false /*Serial*/, true /*PABOOST*/, 470E6 /*Band*/);
  
  // Configura o Serial manualmente
  Serial.begin(9600);
  
  // Configuração da comunicação RS485
  Serial2.begin(9600, SERIAL_8N1, 12, 13); // RX=12, TX=13

  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);  // Inicia em modo recepção

  // Inicializa anemômetro (ID 1)
  nodeAnemometro.begin(1, Serial2);
  nodeAnemometro.preTransmission(preTransmission);
  nodeAnemometro.postTransmission(postTransmission);

  // Inicializa biruta (ID 2)
  nodeBiruta.begin(2, Serial2);
  nodeBiruta.preTransmission(preTransmission);
  nodeBiruta.postTransmission(postTransmission);

  Serial.println("Sistema de leitura Biruta e Anemômetro inicializado");
  
  // Tela inicial
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Monitor Vento");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 15, "Inicializando...");
  Heltec.display->display();
  delay(2000);
}

void loop() {
  // === Velocidade do Vento (anemômetro - ID 1) ===
  uint8_t resultAnemometro = nodeAnemometro.readInputRegisters(0x0000, 1);
  if (resultAnemometro == nodeAnemometro.ku8MBSuccess) {
    uint16_t rawSpeed = nodeAnemometro.getResponseBuffer(0);
    lastWindSpeed = rawSpeed / 10.0; // Ajuste conforme o sensor
    anemometroOK = true;
    Serial.print("Velocidade do vento: ");
    Serial.print(lastWindSpeed);
    Serial.println(" m/s");
  } else {
    anemometroOK = false;
    Serial.print("Erro na leitura do anemômetro: ");
    Serial.println(resultAnemometro, HEX);
  }

  delay(100); // Pequeno delay entre leituras

  // === Direção do Vento (biruta - ID 2) ===
  uint8_t resultBiruta = nodeBiruta.readInputRegisters(0x0001, 1);
  if (resultBiruta == nodeBiruta.ku8MBSuccess) {
    lastWindDirection = nodeBiruta.getResponseBuffer(0);
    birutaOK = true;
    Serial.print("Direção do vento (raw): ");
    Serial.println(lastWindDirection);
  } else {
    birutaOK = false;
    Serial.print("Erro na leitura da biruta: ");
    Serial.println(resultBiruta, HEX);
  }

  // Atualiza o display OLED
  updateOLED();

  Serial.println("---"); // Separador para facilitar leitura
  delay(1000);
}