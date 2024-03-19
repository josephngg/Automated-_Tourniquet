#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// declare variables for pins
const int button_pin = 2;
const int motor_pin = 9;

const int transducer = A0;
const int rcOscillation = A1;

int pressure;
int osc;

// variable for the button state
int button_state;
// variable to track the previous button state
int prev_button_state = HIGH;
// variable to track the current state of the system
int system_state = 0; // 0 represents off, 1 represents on
unsigned long motorStartTime = 0;

void displayVariable() {
  display.clearDisplay();

  // Print the variable value
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Value: ");
  display.println(pressure);

  display.display();
}

void setup() // code that only runs once
{
    // initialize serial communication
  Serial.begin(9600);

  pinMode(motor_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);

  pinMode(transducer,INPUT);
  pinMode(rcOscillation,INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
}

void loop()
{
  // Data Logging
  float time = micros() / 1e6;

  // Start Stop for system
  button_state = digitalRead(button_pin);

  if (button_state == LOW && prev_button_state == HIGH) {
    // Button is pressed
    delay(50); // Debounce
    if (button_state == LOW) {
      // Toggle system state when the button is pressed
      system_state = 1 - system_state; // Toggle between 0 and 1
      analogWrite(motor_pin, 0); // Ensure motor stops immediately
      delay(500); // Debounce
      Serial.print("Button State: ");
      Serial.println(system_state);
      motorStartTime = millis(); // Record the time when the motor starts
    }
  }

  if (system_state == 1) {
    // Motor is on
    analogWrite(motor_pin, 128);
  } else {
    // Motor is off
    analogWrite(motor_pin, 0);
  }

  // Update the previous button state
  prev_button_state = button_state;

  // transducer code
  pressure = analogRead(transducer);
  osc = analogRead(rcOscillation);

  Serial.print(time);
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.println(osc);

  displayVariable();
  
  delay(20); // data collection rate
}
