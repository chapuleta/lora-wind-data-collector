#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <WiFi.h>
#include "heltec.h"
#include "HT_SSD1306Wire.h"
#include <time.h>

#define BAND 915e6
#define SD_CS 12

SPIClass spi1;
static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

// Configurações de data e hora
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600; // Ajuste para o fuso horário (ex: -3 para Brasília)
const int daylightOffset_sec = 0;     // Ajuste para horário de verão (0 se não aplicável)

// Variáveis globais para data e hora
struct tm timeinfo;

void setup() {
    Serial.begin(115200);
    Heltec.begin(false, true, true, true, BAND);  // Inicializa o sistema Heltec com LoRa ativado
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);

    // Inicializa o display OLED
    VextON();
    delay(100);
    display.init();
    display.setFont(ArialMT_Plain_10);

    // Conecta ao Wi-Fi para obter a data e hora
    WiFi.begin("AndroidAP3958", "99154145"); // Substitua pelo seu SSID e senha
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado ao Wi-Fi");

    // Configura o NTP para obter a data e hora
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Falha ao obter a data e hora");
        return;
    }
    Serial.println("Data e hora obtidas com sucesso");

    // Inicializa o cartão SD
    spi1.begin(17, 13, 23, SD_CS);
    if (!SD.begin(SD_CS, spi1)) {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    Serial.println("SD Card inicializado.");

    // Cria o arquivo CSV com cabeçalhos se não existir
    if (!SD.exists("/temperatures.csv")) {
        File file = SD.open("/temperatures.csv", FILE_WRITE);
        if (file) {
            file.println("Dia, Mês, Ano, Temperatura"); // Cabeçalhos das colunas
            file.close();
            Serial.println("Arquivo CSV criado com cabeçalhos.");
        } else {
            Serial.println("Falha ao criar o arquivo CSV");
        }
    }

    Serial.println("Receptor LoRa iniciado!");
}

void loop() {
    // Verifica se há pacote e tenta ler
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        // Recebeu um pacote
        Serial.print("Pacote recebido: ");
        String received = "";
        while (LoRa.available()) {
            received += (char)LoRa.read();
        }
        Serial.println(received);

        // Obtém a data e hora atual
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Falha ao obter a data e hora");
            return;
        }

        // Formata a data e hora
        char dateStr[20];
        snprintf(dateStr, sizeof(dateStr), "%d, %d, %d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);

        // Exibe os dados recebidos no display OLED
        display.clear(); // Limpa a tela
        display.setColor(WHITE); // Define a cor do texto
        display.drawString(0, 0, "Recebido: " + received); // Exibe a temperatura recebida
        display.drawString(0, 20, "Data: " + String(dateStr)); // Exibe a data
        display.display(); // Atualiza o display

        // Salva a temperatura com a data e hora no arquivo CSV
        saveTemperatureToCSV(dateStr, received);
    }
}

void saveTemperatureToCSV(String dateStr, String temperature) {
    File file = SD.open("/temperatures.csv", FILE_APPEND);
    if (file) {
        file.print(dateStr); // Escreve a data
        file.print(", ");
        file.println(temperature); // Escreve a temperatura
        file.close();
        Serial.println("Dados salvos no CSV.");
    } else {
        Serial.println("Falha ao abrir o arquivo CSV para escrita");
    }
}

void VextON(void) {
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);
}

void VextOFF(void) {
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);
}