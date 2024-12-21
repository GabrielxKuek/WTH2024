///////////////////////////////
// code to display on mini lcd
///////////////////////////////

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  // Change to 64 if using 128x64 display
#define OLED_RESET -1    

// Custom I2C pins
#define I2C_SDA 21  // Keep default SDA
#define I2C_SCL 16  // Using GPIO16 instead of 22

#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);  // Initialize I2C with custom pins
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Hello World!"));
  display.display();
}

void loop() {
  // Your main code here
}
