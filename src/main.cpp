#include <Arduino.h>

// === DIREÇÃO DO VENTO E ANEMÔMETRO (Modbus RS485) ===
#include <ModbusMaster.h>

#define DE_RE_PIN 2  // Pino para controlar DE e RE do MAX485

ModbusMaster nodeAnemometro;   // Dispositivo Modbus ID 1 (velocidade do vento)
ModbusMaster nodeBiruta;       // Dispositivo Modbus ID 2 (direção do vento)

void preTransmission() {
  digitalWrite(DE_RE_PIN, HIGH);  // Habilita transmissão
}

void postTransmission() {
  digitalWrite(DE_RE_PIN, LOW);   // Habilita recepção
}

void setup() {
  Serial.begin(9600);

  // Configuração da comunicação RS485
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

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
}

void loop() {
  // === Velocidade do Vento (anemômetro - ID 1) ===
  uint8_t resultAnemometro = nodeAnemometro.readInputRegisters(0x0000, 1);
  if (resultAnemometro == nodeAnemometro.ku8MBSuccess) {
    uint16_t rawSpeed = nodeAnemometro.getResponseBuffer(0);
    float windSpeed = rawSpeed / 10.0; // Ajuste conforme o sensor
    Serial.print("Velocidade do vento: ");
    Serial.print(windSpeed);
    Serial.println(" m/s");
  } else {
    Serial.print("Erro na leitura do anemômetro: ");
    Serial.println(resultAnemometro, HEX);
  }

  delay(100); // Pequeno delay entre leituras

  // === Direção do Vento (biruta - ID 2) ===
  uint8_t resultBiruta = nodeBiruta.readInputRegisters(0x0001, 1);
  if (resultBiruta == nodeBiruta.ku8MBSuccess) {
    uint16_t windDirection = nodeBiruta.getResponseBuffer(0);
    Serial.print("Direção do vento (raw): ");
    Serial.println(windDirection);
  } else {
    Serial.print("Erro na leitura da biruta: ");
    Serial.println(resultBiruta, HEX);
  }

  Serial.println("---"); // Separador para facilitar leitura
  delay(1000);
}