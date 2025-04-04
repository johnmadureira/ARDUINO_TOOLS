#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10  // Pino SDA do MFRC522
#define RST_PIN 9  // Pino de Reset do MFRC522

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  // Chave de autenticação

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    
    // Inicializa a chave padrão (6 bytes de 0xFF)
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}

void loop() {
    // Aguarda um novo cartão
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    byte bloco = 1; // Bloco 1 do setor 0
    byte buffer[18]; // Buffer para armazenar os dados lidos
    byte tamanho = sizeof(buffer);

    // Autenticação antes da leitura
    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid)) != MFRC522::STATUS_OK) {
        return;
    }

    // Leitura do bloco
    if (mfrc522.MIFARE_Read(bloco, buffer, &tamanho) != MFRC522::STATUS_OK) {
        return;
    }

    // Exibir apenas os valores hexadecimais dos bytes não vazios
    for (byte i = 0; i < 16; i++) {
        if (buffer[i] != 0x00 && buffer[i] != 0xFF) { // Ignora bytes vazios (0x00) e preenchidos (0xFF)
            if (buffer[i] < 0x10) Serial.print("0"); // Garante dois dígitos por byte
            Serial.print(buffer[i], HEX);
        }
    }
    Serial.println();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(100);
}
