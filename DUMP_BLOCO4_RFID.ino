#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10  // Pino SDA do MFRC522
#define RST_PIN 9  // Pino RST do MFRC522

MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
    Serial.begin(9600); 
    SPI.begin();        
    mfrc522.PCD_Init(); 

    Serial.println("Aproxime um cartão RFID para ler os dados...");
}

void loop() {
    // Verifica se há um novo cartão presente
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Tenta ler o cartão
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("\n--- Cartão Detectado ---");
    Serial.print("UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Chave padrão para autenticação
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF; // Chave padrão FF FF FF FF FF FF
    }

    // Percorre todos os setores e blocos
    for (byte setor = 0; setor < 16; setor++) {
        Serial.print("\nSetor ");
        Serial.print(setor);
        Serial.println(":");

        for (byte bloco = 0; bloco < 4; bloco++) {
            byte blocoNum = setor * 4 + bloco;

            // Autentica antes de ler
            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blocoNum, &key, &(mfrc522.uid)) != MFRC522::STATUS_OK) {
                Serial.print("Falha ao autenticar bloco ");
                Serial.println(blocoNum);
                continue;
            }

            // Lê os dados do bloco
            byte buffer[18];
            byte tamanho = sizeof(buffer);
            if (mfrc522.MIFARE_Read(blocoNum, buffer, &tamanho) != MFRC522::STATUS_OK) {
                Serial.print("Erro ao ler bloco ");
                Serial.println(blocoNum);
                continue;
            }

            // Exibe os dados em HEX
            Serial.print("Bloco ");
            Serial.print(blocoNum);
            Serial.print(": ");
            for (byte i = 0; i < 16; i++) {
                Serial.print(buffer[i] < 0x10 ? " 0" : " ");
                Serial.print(buffer[i], HEX);
            }

            // Exibe dados em ASCII (se for legível)
            Serial.print("  |  ");
            for (byte i = 0; i < 16; i++) {
                if (buffer[i] >= 32 && buffer[i] <= 126) {
                    Serial.write(buffer[i]);
                } else {
                    Serial.print(".");
                }
            }
            Serial.println();
        }
    }

    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
}
