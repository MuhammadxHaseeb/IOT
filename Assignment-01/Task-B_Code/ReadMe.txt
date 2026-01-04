1. Wokwi Link:

https://wokwi.com/projects/445177228056823809

2. Pin Map:

| Component            | Pin Number                  | Direction       | Description / Function             |
| -------------------- | --------------------------- | --------------- | ---------------------------------- |
| LED                  | GPIO 18                     | Output          | Turns ON/OFF on short button press |
| Button (Push Button) | GPIO 34                     | Input (Pull-up) | Detects short or long press        |
| Buzzer               | GPIO 14                     | Output          | Emits a tone on long press         |
| OLED SDA             | GPIO 21 (default I²C SDA)   | Output          | Sends data to SSD1306 display      |
| OLED SCL             | GPIO 22 (default I²C SCL)   | Output          | Clock signal for SSD1306 display   |
| VCC (OLED)           | 3.3V                        | —               | Power supply for OLED              |
| GND                  | —                           | —               | Common ground for all components   |
