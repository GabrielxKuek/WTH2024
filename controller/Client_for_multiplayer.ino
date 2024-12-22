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
}

void onDataSent(uint8_t *macAddr, uint8_t status) {
    Serial.println(status == 0 ? "Message Sent Successfully" : "Send Failed");
}

void onDataRecv(uint8_t *macAddr, uint8_t *data, uint8_t len) {
    MoveMessage receivedMove;
    memcpy(&receivedMove, data, sizeof(receivedMove));

    Serial.print("Received move: ");
    Serial.println(receivedMove.move);
    Serial.print("Player Health: ");
    Serial.println(receivedMove.health);

    // Sync player stats
    player.health = receivedMove.health;

    // Apply logic
    resolveMoves(receivedMove.move, getEnemyMove());
}

void battle() {
  while (player.health > 0 && enemy.health > 0) {   
    // Wait for the host's move
    delay(1000); // Add delay for synchronization
  }
  
  if (player.health <= 0) {
    Serial.println("Player is defeated!");
    // End the game
    endGame();
  } else if (enemy.health <= 0) {
    Serial.println("Enemy is defeated! Waiting for next round.");
    currentScore++; // Increase score for surviving a round
    nextRound();
  }
}

void resolveMoves(MoveType enemyMove, MoveType playerMove) {
  if (playerMove == QUICK_ATTACK && enemyMove == HEAVY_ATTACK) {
    enemy.health -= player.attack; // Quick cancels Heavy
    Serial.println("Enemy used Quick Attack, cancelling Player's Heavy Attack!");
  } else if (playerMove == HEAVY_ATTACK && enemyMove == PARRY) {
    enemy.health -= 1.5 * player.attack; // Heavy bypasses Parry
    Serial.println("Enemy used Heavy Attack, bypassing Player's Parry!");
  } else if (playerMove == PARRY && enemyMove == QUICK_ATTACK) {
    enemy.health -= 0.5 * player.attack; // Parry blocks Quick partially
    Serial.println("Enemy used Parry, blocking Player's Quick Attack!");
  } else if (enemyMove == QUICK_ATTACK && playerMove == HEAVY_ATTACK) {
    player.health -= enemy.attack; // Quick cancels Heavy
    Serial.println("Player used Quick Attack, bypassing Enemy's Heavy Attack!");
  } else if (enemyMove == HEAVY_ATTACK && playerMove == PARRY) {
    player.health -= 1.5 * enemy.attack; // Heavy bypasses Parry
    Serial.println("Player used Heavy Attack, bypassing Enemy's Parry!");
  } else if (enemyMove == PARRY && playerMove == QUICK_ATTACK) {
    player.health -= 0.5 * enemy.attack; // Parry blocks Quick partially
    Serial.println("Player used Parry, blocking Enemy's Quick Attack!");
  } else {
    Serial.println("Both used the same move. No damage taken!");
  }
  displayStats();
}

MoveType getEnemyMove() {
  return static_cast<MoveType>(random(0, 3)); // Random move: 0 to 2
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
  Serial.println("Ready to receive data...");
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

    // Pair with the host
    uint8_t peerMac[] = {0x24, 0x0A, 0xC4, 0xXX, 0xXX, 0xXX}; // Replace with host's MAC address
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

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
}
