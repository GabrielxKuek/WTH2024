// Pins for buttons
const int taskButtonPin = 38;

// Define a struct to represent a task
struct Task {
  String name;
  bool completed;
};

// Maximum number of tasks
const int maxTasks = 5;

// Array to store tasks
Task tasks[maxTasks];

// Button state tracking
int buttonPressCount = 0;
unsigned long lastPressTime = 0; // For debouncing

void setup() {
  pinMode(taskButtonPin, INPUT_PULLUP);
  Serial.begin(115200);

  // Initialize tasks
  for (int i = 0; i < maxTasks; i++) {
    tasks[i].name = "Task " + String(i + 1); 
    tasks[i].completed = false; 
  }

  Serial.println("Task Tracker Initialized!");
  displayTasks(); 
}

void loop() {
  handleTaskButton();
}

// Function to handle task button presses
void handleTaskButton() {
  if (digitalRead(taskButtonPin) == LOW) {
    unsigned long currentTime = millis();

    // Debounce button
    if (currentTime - lastPressTime > 200) {
      buttonPressCount++;
      lastPressTime = currentTime;

      // Check for three presses
      if (buttonPressCount == 3) {
        selectAndCompleteTask(); // Select and complete a task
        buttonPressCount = 0; // Reset count
      }
    }
  }
}

// Select and complete a task
void selectAndCompleteTask() {
  Serial.println("Select task to complete (enter task number 1-" + String(maxTasks) + "):");

  while (Serial.available() == 0) { 
    // Wait for user input
  }

  int selectedTask = Serial.parseInt(); 

  if (selectedTask >= 1 && selectedTask <= maxTasks) { 
    if (!tasks[selectedTask - 1].completed) { 
      tasks[selectedTask - 1].completed = true; 
      Serial.println(tasks[selectedTask - 1].name + " marked as completed!");
    } else {
      Serial.println(tasks[selectedTask - 1].name + " is already completed.");
    }
  } else {
    Serial.println("Invalid task number.");
  }

  displayTasks(); 
}

// Display task status
void displayTasks() {
  Serial.println("Task Status:");
  for (int i = 0; i < maxTasks; i++) {
    Serial.print(tasks[i].name);
    Serial.print(": ");
    Serial.println(tasks[i].completed ? "Completed" : "Not Started");
  }
}
