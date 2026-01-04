// Assignment-01_Task-A
// 3 Led and 2 Buttons with Different Cases
// Embedded IoT System Fall-2025

// Name: Muhammad Haseeb                 Reg#: 23-NTU-CS-1069
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LED1 19
#define LED2 18
#define LED3 5

#define BTN_MODE 34
#define BTN_RESET 35

// ----- Mode Variables -----
volatile int mode = 0;
volatile int alt_Mode = 1;
volatile bool modeChanged = false;
volatile bool resetPressed = false;

// ----- Debounce Control -----
hw_timer_t *debounceTimer = NULL;
volatile bool debounceActive = false;  // shared debounce flag
volatile int lastButton = 0;           // 1 = mode, 2 = reset

// ----- PWM Setup -----
#define PWM_CH 0
#define FREQ 5000
#define RES 8

int brightness = 0;
int fadeAmount = 15;
unsigned long previousMillis = 0;
unsigned long lastFade = 0;
bool ledState = LOW;

// ======== ISR SECTION ========

// Reset debounce after 200ms
void IRAM_ATTR resetDebounce() {
  debounceActive = false;
  timerAlarmDisable(debounceTimer);
  timerWrite(debounceTimer, 0);
}

// When MODE button pressed
void IRAM_ATTR BTN_Pressed_Mode() {
  if (debounceActive) return;
  debounceActive = true;
  lastButton = 1;
  timerWrite(debounceTimer, 0);
  timerAlarmWrite(debounceTimer, 200000, false);
  timerAlarmEnable(debounceTimer);
  modeChanged = true;
}

// When RESET button pressed
void IRAM_ATTR BTN_Pressed_Reset() {
  if (debounceActive) return;
  debounceActive = true;
  lastButton = 2;
  timerWrite(debounceTimer, 0);
  timerAlarmWrite(debounceTimer, 200000, false);
  timerAlarmEnable(debounceTimer);
  resetPressed = true;
}

// ======== DISPLAY FUNCTION ========
void showMode(const char* text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - strlen(text) * 12) / 2, (SCREEN_HEIGHT - 16) / 2);
  display.print(text);
  display.display();
}

// ======== SETUP ========
void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.println("Mode 0: OFF");
  display.display();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // Attach button interrupts
  attachInterrupt(digitalPinToInterrupt(BTN_MODE), BTN_Pressed_Mode, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_RESET), BTN_Pressed_Reset, FALLING);

  // Setup single hardware timer for debounce
  debounceTimer = timerBegin(0, 80, true);              // 1 tick = 1µs
  timerAttachInterrupt(debounceTimer, &resetDebounce, true);
  timerAlarmWrite(debounceTimer, 200000, false);        // 200ms debounce
  timerAlarmDisable(debounceTimer);

  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(LED3, PWM_CH);
}

// ======== LOOP ========
void loop() {
  // Reset button action
  if (resetPressed) {
    mode = 0;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    resetPressed = false;
    showMode("Default");
  }

  // Mode button action
  if (modeChanged) {
    mode++;
    if (mode > 4) mode = 0;
    modeChanged = false;
  }

  // ----- LED Modes -----
  switch (mode) {
    case 0:
      showMode("Default");
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      ledcWrite(PWM_CH, 0);
      break;

    case 1:
      showMode("Both OFF");
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      ledcWrite(PWM_CH, 0);
      break;

    case 2:
      showMode("Blinking");
      if (millis() - previousMillis >= 500) {
        previousMillis = millis();
        ledState = !ledState;
        digitalWrite(LED1, ledState);
        digitalWrite(LED2, !ledState);
      }
      break;

    case 3:
      showMode("Both ON");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      ledcWrite(PWM_CH, 0);
      break;

    case 4:
      showMode("PWM");
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      if (millis() - lastFade >= 2) {
        lastFade = millis();
        ledcWrite(PWM_CH, brightness);
        brightness += fadeAmount;
        if (brightness <= 0 || brightness >= 255)
          fadeAmount = -fadeAmount;
      }
      break;
  }
}
