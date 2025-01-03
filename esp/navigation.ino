////////////////////////////////////////////////
// Improved joystick and button detection code
////////////////////////////////////////////////
#define PIN_ANALOG_X 1
#define PIN_ANALOG_Y 2
#define PIN_B_BUTTON 41
#define PIN_C_BUTTON 42

#define THRESHOLD_LOW 2700
#define THRESHOLD_HIGH 2900

// Debouncing variables
#define DEBOUNCE_DELAY 50
unsigned long lastBButtonDebounceTime = 0;
unsigned long lastCButtonDebounceTime = 0;
int lastBButtonState = HIGH;
int lastCButtonState = HIGH;

void setup() {
  Serial.begin(115200);
 
  // Configure button pins as inputs with internal pull-up resistors
  pinMode(PIN_B_BUTTON, INPUT_PULLUP);
  pinMode(PIN_C_BUTTON, INPUT_PULLUP);
}

void loop() {
  //////////////////////////
  // Read the analog values
  //////////////////////////
  int xValue = analogRead(PIN_ANALOG_X);
  int yValue = analogRead(PIN_ANALOG_Y);
 
  // Debug output
  Serial.print("Raw Values -> X: ");
  Serial.print(xValue);
  Serial.print(" Y: ");
  Serial.print(yValue);
  Serial.print(" | Direction: ");
 
  // Check Y-axis direction first (prioritize up/down)
  if (yValue < THRESHOLD_LOW) {
    Serial.println("DOWN");
  }
  else if (yValue > THRESHOLD_HIGH) {
    Serial.println("UP");
  }
  // Check X-axis direction
  else if (xValue < THRESHOLD_LOW) {
    Serial.println("LEFT");
  }
  else if (xValue > THRESHOLD_HIGH) {
    Serial.println("RIGHT");
  }
  else {
    Serial.println("NEUTRAL");
  }
 
  /////////////////////////
  // Improved button handling
  /////////////////////////
  // Read current button states
  int currentBButtonState = digitalRead(PIN_B_BUTTON);
  int currentCButtonState = digitalRead(PIN_C_BUTTON);
  
  // Check B button with debouncing
  if (currentBButtonState != lastBButtonState) {
    lastBButtonDebounceTime = millis();
  }
  
  if ((millis() - lastBButtonDebounceTime) > DEBOUNCE_DELAY) {
    if (currentBButtonState == LOW) {  // Button is pressed (active LOW)
      Serial.println("Button B Pressed!");
    }
  }
  
  // Check C button with debouncing
  if (currentCButtonState != lastCButtonState) {
    lastCButtonDebounceTime = millis();
  }
  
  if ((millis() - lastCButtonDebounceTime) > DEBOUNCE_DELAY) {
    if (currentCButtonState == LOW) {  // Button is pressed (active LOW)
      Serial.println("Button C Pressed!");
    }
  }
  
  // Save the current button states
  lastBButtonState = currentBButtonState;
  lastCButtonState = currentCButtonState;
  
  // Shorter delay to improve responsiveness
  delay(100);  // Reduced from 500ms to 100ms
}
