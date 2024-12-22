#include <EEPROM.h>
#include <math.h>

#include <SPI.h>
#include <MFRC522.h>

#include <esp_now.h>
#include <WiFi.h>

//---------------------------------------------------------------------------

#define RST_PIN         9
#define SS_PIN          10


struct Character {
  int health;
  int attack;
};

struct Enemy {
  int health;
  int attack;
};

enum MoveType {
  QUICK_ATTACK,
  PARRY,
  HEAVY_ATTACK
};

struct MoveMessage {
  MoveType move;  // Player's move
  int health;     // Player's health
  int attack;     // Player's attack
  int round;      // Current round number
};

// Define global variables
float goodStreak = 0, badStreak = 0;
// float player.health = 100, player.attack = 25;
// float enemyHp = 50, enemtAtk = 10;
float userStatIncrementHp = 50, userStatIncrementAtk = 10;
float userStatDecrementHp = 60, userStatDecrementAtk = 12;
float streakIncrement = 0.05;

Character player = {100, 25};
Enemy enemy;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//-------------------------------------------------------------------------------------

// Button pins
const int quickAttackButton = 1;
const int parryButton = 2;
const int heavyAttackButton = 42;
const int startBattleButton = 41;

// Debounce variables
const unsigned long debounceDelay = 200;

// EEPROM Address for high score
const int highScoreAddress = 0;

int roundNumber = 1;
int highScore = 0; // Variable to store high score
int currentScore = 0; // Player's current score

void nextRound() {
  roundNumber++;
  enemy = generateEnemy(roundNumber); // Assign generated enemy
}

// Base stats for enemies
const int baseHealth = 50;
const int baseAttack = 10;

// Scaling multipliers
const float healthMultiplier = 1.2; // Health increases by 20% each round
const float attackMultiplier = 1.1; // Attack increases by 10% each round

void onDataSent(uint8_t *macAddr, uint8_t status) {
    Serial.println(status == 0 ? "Message Sent Successfully" : "Send Failed");
}

// In onDataRecv():
void onDataRecv(uint8_t *macAddr, uint8_t *data, uint8_t len) {
    MoveMessage receivedMove;
    memcpy(&receivedMove, data, sizeof(receivedMove));

    Serial.print("Received move: ");
    Serial.println(receivedMove.move);
    Serial.print("Enemy Health: ");
    Serial.println(receivedMove.health);

    // Sync enemy stats
    enemy.health = receivedMove.health;

    // Apply logic
    resolveMoves(getPlayerMove(), receivedMove.move);
}

Enemy generateEnemy(int round) {
  Enemy newEnemy;
  newEnemy.health = baseHealth * pow(healthMultiplier, round - 1);
  newEnemy.attack = baseAttack * pow(attackMultiplier, round - 1);
  return newEnemy;
}

void battle() {
  while (player.health > 0 && enemy.health > 0) {   
    // Collect player input
    MoveType playerMove = getPlayerMove();
    
    // Send the move to the other device
    MoveMessage myMove = {playerMove, player.health};
    esp_now_send(peerMac, (uint8_t *)&myMove, sizeof(myMove));
  }
  
  if (player.health <= 0) {
    Serial.println("Player is defeated!");
    // End the game
    endGame();
  } else if (enemy.health <= 0) {
    Serial.println("Enemy is defeated! Moving to next round.");
    currentScore++; // Increase score for defeating an enemy
    nextRound();
  }
}

void resolveMoves(MoveType playerMove, MoveType enemyMove) {
  if (playerMove == QUICK_ATTACK && enemyMove == HEAVY_ATTACK) {
    enemy.health -= player.attack; // Quick cancels Heavy
    Serial.println("Player used Quick Attack, cancelling Enemy's Heavy Attack!");
  } else if (playerMove == HEAVY_ATTACK && enemyMove == PARRY) {
    enemy.health -= 1.5 * player.attack; // Heavy bypasses Parry
    Serial.println("Player used Heavy Attack, bypassing Enemy's Parry!");
  } else if (playerMove == PARRY && enemyMove == QUICK_ATTACK) {
    enemy.health -= 0.5 * player.attack; // Parry blocks Quick partially
    Serial.println("Player used Parry, blocking Enemy's Quick Attack!");
  } else if (enemyMove == QUICK_ATTACK && playerMove == HEAVY_ATTACK) {
    player.health -= enemy.attack; // Quick cancels Heavy
    Serial.println("Enemy used Quick Attack, bypassing Player's Heavy Attack!");
  } else if (enemyMove == HEAVY_ATTACK && playerMove == PARRY) {
    player.health -= 1.5 * enemy.attack; // Heavy bypasses Parry
    Serial.println("Enemy used Heavy Attack, bypassing Player's Parry!");
  } else if (enemyMove == PARRY && playerMove == QUICK_ATTACK) {
    player.health -= 0.5 * enemy.attack; // Parry blocks Quick partially
    Serial.println("Enemy used Parry, blocking Player's Quick Attack!");
  } else {
    Serial.println("Both used the same move. No damage taken!");
  }
  displayStats();
}

MoveType enemyTurn() {
  return static_cast<MoveType>(random(0, 3)); // Random move: 0 to 2
}

MoveType getPlayerMove() {
  unsigned long startTime = millis();
  Serial.println("You have 10 seconds to choose your move...");
  
  while (millis() - startTime < 10000) { // Wait for 10 seconds
    if (digitalRead(quickAttackButton) == LOW) {
      delay(debounceDelay); // Debounce delay
      return QUICK_ATTACK;
    }

    if (digitalRead(parryButton) == LOW) {
      delay(debounceDelay); // Debounce delay
      return PARRY;
    }

    if (digitalRead(heavyAttackButton) == LOW) {
      delay(debounceDelay); // Debounce delay
      return HEAVY_ATTACK;
    }
  }

  // If no input within 10 seconds, return default QUICK_ATTACK
  Serial.println("No input received. Defaulting to Quick Attack.");
  return QUICK_ATTACK;
}

void displayStats() {
  Serial.println("=== Current Stats ===");
  Serial.print("Player Health: ");
  Serial.println(player.health);
  Serial.print("Enemy Health: ");
  Serial.println(enemy.health);
  Serial.println("=====================");
}

void endGame() {
  // Show the final score and the high score
  if (currentScore > highScore) {
    highScore = currentScore; // Update the high score
    EEPROM.write(highScoreAddress, highScore); // Save the high score to EEPROM
  }
  
  Serial.print("Game Over! Your score: ");
  Serial.println(currentScore);
  Serial.print("Your High Score: ");
  Serial.println(highScore);

  // Exit the game
  Serial.println("Exiting the arena...");
  while (true); // Infinite loop to end the program
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512); // Initialize EEPROM with 512 bytes
  WiFi.mode(WIFI_STA);

  while (!Serial);
    SPI.begin(11, 13, 12, 10);
    mfrc522.PCD_Init();
    
    // Initialize key (same as write code)
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

  pinMode(quickAttackButton, INPUT_PULLUP);
  pinMode(parryButton, INPUT_PULLUP);
  pinMode(heavyAttackButton, INPUT_PULLUP);
  pinMode(startBattleButton, INPUT_PULLUP);
  

  // Load the high score from EEPROM
  highScore = EEPROM.read(highScoreAddress);
  Serial.println("Ready to read card data...");
  Serial.println("Game Initialized!");


    // Initialize ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW Initialization Failed");
        return;
    }

    // Register callbacks
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    // Pair with the other device
    uint8_t peerMac[] = {0x24, 0x0A, 0xC4, 0xXX, 0xXX, 0xXX}; // Replace with your peer's MAC address
    esp_now_add_peer(peerMac, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    // Game logic will be handled in the battle
    if (digitalRead(startBattleButton) == LOW) {
        delay(debounceDelay); // Debounce delay
        battle();
        delay(1000); // Add delay for each turn
    }

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
          player.health += userStatIncrementHp * (1 + goodStreak * streakIncrement); 
          player.attack += userStatIncrementAtk*(1 + goodStreak * streakIncrement); 
        } else {
          player.health += userStatIncrementHp * (1 + 10 * streakIncrement); 
          player.attack += userStatIncrementAtk * (1+ 10 * streakIncrement); 
        }
        goodStreak += 1;
      }
      else if(card_data == "2") { // parent dislike, check if the user stats is at minimum range or not. end the win streak, start bad streak, 
        goodStreak = 0;
        player.health -= userStatIncrementHp * (1 + badStreak * streakIncrement); 
        player.attack -= userStatIncrementAtk * (1 + badStreak * streakIncrement);
        badStreak += 1;

        if(player.health < 100) player.health = 100;
        if(player.attack < 25) player.attack = 25;
      }

      Serial.print("User Hp: ");
      Serial.println(player.health);
      Serial.print("goodStreaK: ");
      Serial.println(goodStreak);
      Serial.print("badStreak: ");
      Serial.println(badStreak);
    }  

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
}
