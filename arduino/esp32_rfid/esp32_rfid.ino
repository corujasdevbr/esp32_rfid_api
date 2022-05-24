#include <Arduino.h>
#include <MFRC522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI
#include <HTTPClient.h>
#include <WiFi.h>

HTTPClient http;

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

// Replace with your network credentials (STATION)
const char* ssid = "Corujasdev_2G";
const char* password = "Corujadev132";

//esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
//código de status de retorno da autenticação
MFRC522::StatusCode status;

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

int LED_BUILTIN = 2;

void setup() {
    Serial.begin(115200);
    delay(3000);

    WiFi.begin(ssid, password);
    Serial.println("Resetting due to Wifi not connecting...");
    
    // Wait for wifi to be connected
    uint32_t notConnectedCounter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Wifi connecting...");
        notConnectedCounter++;
        if(notConnectedCounter > 50) { // Reset board if not connected after 5s
            Serial.println("Resetting due to Wifi not connecting...");
            ESP.restart();
        }
    }
    Serial.println("Wifi connected, IP address: ");
    Serial.println(WiFi.localIP());
    
    SPI.begin(); // Init SPI bus
    pinMode (LED_BUILTIN, OUTPUT);

    Serial.println("Aproxime o seu cartao do leitor...");
    
    // Inicia MFRC522
    mfrc522.PCD_Init(); 
    // Mensagens iniciais no serial monitor
    
    Serial.println();
}
void loop() {
    // Aguarda a aproximacao do cartao
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Seleciona um dos cartoes
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
  
    String conteudo= "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       //Serial.print(mfrc522.uid.uidByte[i], HEX);
       conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    Serial.println("Tag : " + String(conteudo));

    Serial.println("Call Api: https://192.168.0.20:7267/api/tag/");
    http.begin("https://192.168.0.20:7267/api/tag/");
    http.addHeader("Content-Type", "application/json");
    String httpRequestData = "{\"tag\":\"" + String(conteudo)  + "\"}";
    int httpResponseCode = http.POST(httpRequestData);
  
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
    mfrc522.PICC_HaltA(); 
    // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
    mfrc522.PCD_StopCrypto1();
}
