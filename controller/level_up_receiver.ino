#include <esp_now.h>
#include <WiFi.h>

// Variables for tracking Tamagotchi stats
int level = 1;
int health = 100;
int attack = 10;

// Structure to receive data
typedef struct struct_message {
  char command[10];  // Command (e.g., "LEVEL_UP" or "DE_LEVEL")
} struct_message;

// Create a struct_message to hold incoming data
struct_message incomingData;

// Callback function when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  Serial.print("Received command: ");
  Serial.println(incomingData.command);

  if (strcmp(incomingData.command, "LEVEL_UP") == 0) {
    levelUp();
  } else if (strcmp(incomingData.command, "DE_LEVEL") == 0) {
    deLevel();
  }
}

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Tamagotchi Receiver Initialized!");
  displayStats();
}

void loop() {
  // The receiver continuously listens for incoming data.
}

// Level up Tamagotchi
void levelUp() {
  level++;
  health += 20;
  attack += 5;
  Serial.println("Leveled up!");
  displayStats();
}

// De-level Tamagotchi
void deLevel() {
  if (level > 1) {
    level--;
    health -= 20;
    attack -= 5;
    Serial.println("De-leveled!");
    displayStats();
  } else {
    Serial.println("Already at the lowest level!");
  }
}

// Display Tamagotchi stats
void displayStats() {
  Serial.print("Level: ");
  Serial.println(level);
  Serial.print("Health: ");
  Serial.println(health);
  Serial.print("Attack: ");
  Serial.println(attack);
}