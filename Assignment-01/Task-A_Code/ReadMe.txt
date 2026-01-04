1. Wokwi Link:
https://wokwi.com/projects/445058360114427905

2. Pin Map:

|   Component    |           Pin Number            |     Direction     |       Description / Function         |
| -------------- | ------------------------------- | ----------------- | ------------------------------------ |
| LED1           | GPIO 19                         | Output            | Turns ON/OFF or blinks alternately   |
| LED2           | GPIO 18                         | Output            | Turns ON/OFF or blinks alternately   |
| LED3           | GPIO 5                          | Output (PWM)      | Used for brightness fading using PWM |
| BTN_MODE       | GPIO 34                         | Input (Interrupt) | Changes the mode when pressed        |
| BTN_RESET      | GPIO 35                     	   | Input (Interrupt) | Resets mode back to 0 when pressed   |
| OLED SDA       | GPIO 21 (default I²C SDA)       | Output            | Sends display data to SSD1306        |
| OLED SCL       | GPIO 22 (default I²C SCL)       | Output            | Sends clock signal to SSD1306        |
| GND            | —                               | —                 | Common ground for all components     |
| VCC (OLED)     | 3.3V                            | —                 | Power for OLED display               |
