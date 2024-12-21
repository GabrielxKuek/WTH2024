#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10

// Define global variables
float goodStreak = 0, badStreak = 0;
float userHp = 100, userAtk = 25;
float enemyHp = 50, enemtAtk = 10;
float userStatIncrementHp = 50, userStatIncrementAtk = 10;
float userStatDecrementHp = 60, userStatDecrementAtk = 12;
float streakIncrement = 0.05;

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

      if(card_data == "1") { // parent likes, Increase the stats, then increase the streak multiplier
        badStreak = 0;
        if (goodStreak < 10) {
          userHp += userStatIncrementHp * (1 + goodStreak * streakIncrement); 
          userAtk += userStatIncrementAtk*(1 + goodStreak * streakIncrement); 
        } else {
          userHp += userStatIncrementHp * (1 + 10 * streakIncrement); 
          userAtk += userStatIncrementAtk * (1+ 10 * streakIncrement); 
        }
        goodStreak += 1;
      }
      else if(card_data == "2") { // parent dislike, check if the user stats is at minimum range or not. end the win streak, start bad streak, 
        goodStreak = 0;
        userHp -= userStatIncrementHp * (1 + badStreak * streakIncrement); 
        userAtk -= userStatIncrementAtk * (1 + badStreak * streakIncrement);
        badStreak += 1;

        if(userHp < 100) userHp = 100;
        if(userAtk < 25) userAtk = 100;
      }

      Serial.print("User Hp: ");
      Serial.println(userHp);
      Serial.print("goodStreaK: ");
      Serial.println(goodStreak);
      Serial.print("badStreak: ");
      Serial.println(badStreak);
    }  

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
}