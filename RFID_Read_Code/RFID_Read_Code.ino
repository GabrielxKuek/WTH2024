#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10

// Define global variable
int skill_points = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    SPI.begin(11, 13, 12, 10);
    mfrc522.PCD_Init();
    
    // Initialize key (same as write code)
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    
    Serial.println("Ready to read card data...");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    byte blockAddr = 4;
    byte trailerBlock = 7;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid)
    );
    
    if (status != MFRC522::STATUS_OK) {
        Serial.println("Authentication failed");
        return;
    }

    // Read data
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status == MFRC522::STATUS_OK) {
      String card_data = "";
      Serial.print("Data: ");
      for (uint8_t i = 0; i < 16; i++) {
          // If there's actual data (printable character), show it
          if (buffer[i] >= 32 && buffer[i] <= 126) {
              Serial.print((char)buffer[i]);
              card_data += (char)buffer[i];
          } else {
              // If empty or non-printable, show *
              Serial.print("*");
          }
      }
      Serial.println();

      if(card_data == "1") {
        skill_points += 1;
      }
      else if(card_data == "2") {
        if(skill_points > 0) {
          skill_points -= 1;
        }
        else {
          Serial.println("There are no more skill points");
        }
        
      }

      Serial.print("Skill points: ");
      Serial.println(skill_points);
    }  

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
}