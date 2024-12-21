#include <esp_now.h>
#include <WiFi.h>

int levelUpButton = 38;   // Button for level up
int deLevelButton = 39;   // Button for de-level

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x64, 0xE8, 0x33, 0x5D, 0x0F, 0x84};

// Structure example to send data
typedef struct struct_message {
  char command[10];  // Command (e.g., "LEVEL_UP" or "DE_LEVEL")
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  // Set button pins
  pinMode(levelUpButton, INPUT_PULLUP);
  pinMode(deLevelButton, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register send callback
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  if (digitalRead(levelUpButton) == LOW) {
    strcpy(myData.command, "LEVEL_UP");
    sendCommand();
  }

  if (digitalRead(deLevelButton) == LOW) {
    strcpy(myData.command, "DE_LEVEL");
    sendCommand();
  }

  delay(200);  // Debounce
}

void sendCommand() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Command sent successfully");
  } else {
    Serial.println("Error sending command");
  }
}