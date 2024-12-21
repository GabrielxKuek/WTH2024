#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN 21
#define SCL_PIN 19

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Start Serial with 9600 baud rate
  Serial.begin(9600);  // Changed to 9600
  delay(1000);
  
  Serial.println("Starting I2C Scanner...");
  
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Scan for I2C devices
  byte error, address;
  int nDevices = 0;
  
  Serial.println("Scanning for I2C devices...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }
  
  Serial.println("Initializing LCD...");
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
  
  lcd.setCursor(0,1);
  lcd.print("LCD 1602A");
}

void loop() {
  delay(1000);
}
