////////////////////////////////////////////////
// code for just detecting joystick direction
////////////////////////////////////////////////

// define global variables for analog pins
#define PIN_ANALOG_X 0
#define PIN_ANALOG_Y 1

// Define threshold values adjusted for your joystick
#define THRESHOLD_LOW 300    // Changed from 493
#define THRESHOLD_HIGH 360   // Changed from 553

void setup() {
  // Start serial because we will observe values at serial monitor
  Serial.begin(9600);
}

void loop() {
  // Read the analog values
  int xValue = analogRead(PIN_ANALOG_X);
  int yValue = analogRead(PIN_ANALOG_Y);

  // For debugging - let's see the actual values
  Serial.print("Debug - X: ");
  Serial.print(xValue);
  Serial.print(" Y: ");
  Serial.println(yValue);

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

  delay(500);
}
