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
  I2Cleft.begin(20, 19);   // SDA = 13, SCK = 14 (left display)
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
  writeToDisplaysLineByLine(
    "homer simpson is a character from the simp",
    "word one two three four five six seven eightasdasdasdasda",
    "something",
    "poop man the first makes me happy",
    "lalala dog eat shit",
    "i am a god hurry up croissant"
  );

}

void loop() {
  // Main loop is empty as we only need to write once
  delay(1000);
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

void writeToDisplaysLineByLine(String line1, String line2, String line3, 
                             String line4, String line5, String line6) {
  const uint8_t SINGLE_DISPLAY_WIDTH = 21;  // Characters per display
  const uint8_t MAX_LINE_LENGTH = 42;       // Total width across both displays
  
  // Array to hold all lines for easier processing
  String lines[] = {line1, line2, line3, line4, line5, line6};
  
  // Validate and process each line
  for (int i = 0; i < 6; i++) {
    // Check if line exceeds maximum length
    if (lines[i].length() > MAX_LINE_LENGTH) {
      Serial.print("Line ");
      Serial.print(i + 1);
      Serial.println(" exceeds maximum length. Truncating to 42 characters.");
      lines[i] = lines[i].substring(0, MAX_LINE_LENGTH);
    }
  }
  
  // Clear both displays
  displayLeft.clearDisplay();
  displayRight.clearDisplay();
  
  // Set text properties for both displays
  displayLeft.setTextSize(1);
  displayLeft.setTextColor(SSD1306_WHITE);
  displayRight.setTextSize(1);
  displayRight.setTextColor(SSD1306_WHITE);
  
  // Reset cursors
  displayLeft.setCursor(0, 0);
  displayRight.setCursor(0, 0);
  
  // Write each line to displays
  for (int i = 0; i < 6; i++) {
    String currentLine = lines[i];
    uint8_t lineLength = currentLine.length();
    
    // Calculate vertical position (assuming 8 pixels per line with 1 pixel spacing)
    int yPosition = i * 9;
    
    // Split line between displays
    String leftPart, rightPart;
    
    if (lineLength <= SINGLE_DISPLAY_WIDTH) {
      leftPart = currentLine;
      rightPart = "";
    } else {
      leftPart = currentLine.substring(0, SINGLE_DISPLAY_WIDTH);
      rightPart = currentLine.substring(SINGLE_DISPLAY_WIDTH);
    }
    
    // Write to left display
    displayLeft.setCursor(0, yPosition);
    displayLeft.print(leftPart);
    
    // Write to right display
    displayRight.setCursor(0, yPosition);
    displayRight.print(rightPart);
  }
  
  // Update both displays
  displayLeft.display();
  displayRight.display();
}
