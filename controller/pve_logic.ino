#include <EEPROM.h>
#include <math.h>

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

// Button pins
const int quickAttackButton = 1;
const int parryButton = 2;
const int heavyAttackButton = 42;

// Debounce variables
const unsigned long debounceDelay = 200;

// EEPROM Address for high score
const int highScoreAddress = 0;

int roundNumber = 1;
int highScore = 0; // Variable to store high score
int currentScore = 0; // Player's current score

Character player = {100, 20};
Enemy enemy;

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

Enemy generateEnemy(int round) {
  Enemy newEnemy;
  newEnemy.health = baseHealth * pow(healthMultiplier, round - 1);
  newEnemy.attack = baseAttack * pow(attackMultiplier, round - 1);
  return newEnemy;
}

void battle() {
  while (player.health > 0 && enemy.health > 0) {
    MoveType playerMove = getPlayerMove();
    MoveType enemyMove = enemyTurn();
    Serial.print("Enemy chose: ");
    Serial.println(enemyMove == QUICK_ATTACK ? "Quick Attack" : 
                  enemyMove == PARRY ? "Parry" : "Heavy Attack");
    resolveMoves(playerMove, enemyMove);
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
  pinMode(quickAttackButton, INPUT_PULLUP);
  pinMode(parryButton, INPUT_PULLUP);
  pinMode(heavyAttackButton, INPUT_PULLUP);

  // Load the high score from EEPROM
  highScore = EEPROM.read(highScoreAddress);

  Serial.println("Game Initialized!");
}

void loop() {
  // Game logic will be handled in the battle
  battle();
  delay(1000); // Add delay for each turn
}
