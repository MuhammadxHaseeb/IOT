// Assignment-01_Task-b
// Led and Buzzer with Different Cases
// Embedded IoT System Fall-2025

// Name: Muhammad Haseeb                 Reg#: 23-NTU-CS-1069
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN 18
#define BTN_PIN 25
#define BUZZER_PIN 14
#define btn_PIN 35

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


unsigned long buttonPressTime = 0;
bool buttonPressed = false;
bool ledState = LOW;

const unsigned long longPressTime = 1500; // 1.5 seconds

volatile bool btnPressed = false;
volatile bool btnChanged = false;

void IRAM_ATTR handleButton() {
  btnPressed = digitalRead(BTN_PIN) == LOW;
  btnChanged = true;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, ledState);
  digitalWrite(BUZZER_PIN, LOW);
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Ready...");
  display.display();

  attachInterrupt(digitalPinToInterrupt(btn_PIN), handleButton, CHANGE);

}

void loop() {
  bool btnState = digitalRead(BTN_PIN) == LOW; // LOW = pressed

  if (btnState && !buttonPressed) {
    // Button just pressed
    buttonPressed = true;
    buttonPressTime = millis();
  }
  
  if (!btnState && buttonPressed) {
    // Button just released
    unsigned long pressDuration = millis() - buttonPressTime;

    if (pressDuration >= longPressTime) {
      // Long press action
      digitalWrite(LED_PIN, LOW);

      tone(BUZZER_PIN, 1000, 200); // play 1 kHz tone for 200 ms
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Long press detected!");
      display.display();
    } else {
      // Short press action
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Short press detected!");
      display.display();
    }

    buttonPressed = false; // reset for next press
  }
 }


















//                            // Interrupt 
// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_SSD1306.h>

// #define LED_PIN 18
// #define BTN_PIN 25
// #define BUZZER_PIN 14

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// volatile bool buttonChanged = false;
// volatile bool btnPressed = false;
// unsigned long buttonPressTime = 0;
// bool ledState = LOW;
// const unsigned long longPressTime = 1500; // 1.5 seconds

// void IRAM_ATTR handleButton() {
//   btnPressed = digitalRead(BTN_PIN) == LOW;
//   buttonChanged = true;
// }

// void setup() {
//   pinMode(LED_PIN, OUTPUT);
//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(BTN_PIN, INPUT_PULLUP);

//   attachInterrupt(digitalPinToInterrupt(BTN_PIN), handleButton, CHANGE);

//   digitalWrite(LED_PIN, ledState);
//   digitalWrite(BUZZER_PIN, LOW);

//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);
//   display.println("Ready...");
//   display.display();
// }

// void loop() {
//   if (buttonChanged) {
//     buttonChanged = false;

//     if (btnPressed) {
//       buttonPressTime = millis();  // store press time
//     } else {
//       unsigned long pressDuration = millis() - buttonPressTime;

//       if (pressDuration >= longPressTime) {
//         // Long press
//         digitalWrite(LED_PIN, LOW);
//         tone(BUZZER_PIN, 1000, 200);
//         display.clearDisplay();
//         display.setCursor(0, 0);
//         display.println("Long press detected!");
//         display.display();
//       } else {
//         // Short press
//         ledState = !ledState;
//         digitalWrite(LED_PIN, ledState);
//         display.clearDisplay();
//         display.setCursor(0, 0);
//         display.println("Short press detected!");
//         display.display();
//       }
//     }
//   }
// }
