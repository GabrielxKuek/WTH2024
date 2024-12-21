#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SINGLE_SCREEN_LENGTH 21
#define SINGLE_SCREEN_HEIGHT 6
#define TOTAL_SCREEN_CHARACTER_SIZE SINGLE_SCREEN_LENGTH * SINGLE_SCREEN_HEIGHT * 2
#define NUM_SCREENS 6

// Create two Wire instances for the two I2C buses
TwoWire I2Cleft = TwoWire(0);  // First I2C bus
TwoWire I2Cright = TwoWire(1); // Second I2C bus

// Initialize both displays
Adafruit_SSD1306 displayLeft(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cleft, OLED_RESET);
Adafruit_SSD1306 displayRight(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cright, OLED_RESET);

void setup() {
  // Initialize I2C buses
  I2Cleft.begin(13, 14);   // SDA = 13, SCK = 14 (left display)
  I2Cright.begin(4, 5);    // SDA = 4, SCK = 5 (right display)

  // Initialize serial monitor
  Serial.begin(9600);

  // Initialize left display
  if(!displayLeft.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Left SSD1306 allocation failed"));
    for(;;);
  }

  // Initialize right display
  if(!displayRight.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Right SSD1306 allocation failed"));
    for(;;);
  }

  // Clear both displays
  displayLeft.clearDisplay();
  displayRight.clearDisplay();

  ////////////////////
  // display formatted text
  ////////////////////
  writeToDisplays("something super long that i dont have the time to count the letters of that i hope works. lorem ipsum dolore something something something");
}

void loop() {
  // Main loop is empty as we only need to write once
  delay(100);
}

// Helper function to write to both displays
// void writeToDisplays(const char* leftText, const char* rightText) {
//   // Clear both displays
//   displayLeft.clearDisplay();
//   displayRight.clearDisplay();
  
//   // Write to left display
//   displayLeft.setCursor(0,0);
//   displayLeft.println(leftText);
//   displayLeft.display();
  
//   // Write to right display
//   displayRight.setCursor(0,0);
//   displayRight.println(rightText);
//   displayRight.display();
// }

// calculates what to display on all lcd displays
void writeToDisplays(String input_text) {
    // validation
    if (input_text.length() > TOTAL_SCREEN_CHARACTER_SIZE) {
        Serial.println("Character count of input too high. Maximum characters for both screens: " + String(TOTAL_SCREEN_CHARACTER_SIZE));
        Serial.println("Only using up to total character size.");
        input_text = input_text.substring(0, TOTAL_SCREEN_CHARACTER_SIZE);
        Serial.println("Trimmed input: " + input_text);
    }

    //////////////////
    // business logic
    //////////////////
    int num_chunks = (input_text.length() + SINGLE_SCREEN_LENGTH - 1) / SINGLE_SCREEN_LENGTH;
    String left_screen_chunk = "";
    String right_screen_chunk = "";

    // store chunks in left_screen_chunk and right_screen_chunk
    for (int i = 0; i < num_chunks; i++) {
        int start_pos = i * SINGLE_SCREEN_LENGTH;
        int remaining_chars = input_text.length() - start_pos;
        int chunk_length;
        
        if (remaining_chars < SINGLE_SCREEN_LENGTH) {
            chunk_length = remaining_chars;
        } else {
            chunk_length = SINGLE_SCREEN_LENGTH;
        }
        
        String chunk = input_text.substring(start_pos, start_pos + chunk_length);
        
        if (i % 2 == 0) {
            left_screen_chunk += chunk + "\n";
        } else {
            right_screen_chunk += chunk + "\n";
        }
    }

    /////////////////////
    // write to displays
    /////////////////////
    displayLeft.clearDisplay();
    displayRight.clearDisplay();

    // Write to left display
    displayLeft.setTextSize(1);
    displayLeft.setTextColor(SSD1306_WHITE);
    displayLeft.setCursor(0,0);
    displayLeft.println(left_screen_chunk);
    displayLeft.display();

    // Write to right display
    displayRight.setTextSize(1);
    displayRight.setTextColor(SSD1306_WHITE);
    displayRight.setCursor(0,0);
    displayRight.println(right_screen_chunk);
    displayRight.display();
}
