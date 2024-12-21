// Pins for buttons
const int levelUpButtonPin = 2;
const int deLevelButtonPin = 3;

// Variables for tracking tamagotchi stats
int level = 1;
int health = 100;
int attack = 10;

// Button state tracking
int levelUpPressCount = 0;
unsigned long lastPressTime = 0; // For debouncing

void setup() {
  pinMode(levelUpButtonPin, INPUT_PULLUP);
  pinMode(deLevelButtonPin, INPUT_PULLUP);
  Serial.begin(115200);

  Serial.println("Tamagotchi Initialized!");
  displayStats();
}

void loop() {
  handleLevelUp();
  handleDeLevel();
}

// Function to handle level-up button presses
void handleLevelUp() {
  if (digitalRead(levelUpButtonPin) == LOW) {
    unsigned long currentTime = millis();

    // Debounce button
    if (currentTime - lastPressTime > 200) {
      levelUpPressCount++;
      lastPressTime = currentTime;

      // Check for three presses
      if (levelUpPressCount == 3) {
        levelUp();
        levelUpPressCount = 0; // Reset count
      }
    }
  }
}

// Function to handle de-level button presses
void handleDeLevel() {
  if (digitalRead(deLevelButtonPin) == LOW) {
    unsigned long currentTime = millis();

    // Debounce button
    if (currentTime - lastPressTime > 200) {
      deLevel();
      lastPressTime = currentTime;
    }
  }
}

// Level up tamagotchi
void levelUp() {
  level++;
  health += 20;  // Increase health
  attack += 5;   // Increase attack
  Serial.println("Leveled up!");
  displayStats();
}

// De-level tamagotchi
void deLevel() {
  if (level > 1) {
    level--;
    health -= 20;  // Revert health
    attack -= 5;   // Revert attack
    Serial.println("De-leveled!");
    displayStats();
  } else {
    Serial.println("Already at the lowest level!");
  }
}

// Display tamagotchi stats
void displayStats() {
  Serial.print("Level: ");
  Serial.println(level);
  Serial.print("Health: ");
  Serial.println(health);
  Serial.print("Attack: ");
  Serial.println(attack);
}
