# Arduino Uno Blind Project

An ultrasonic proximity aid built on the Arduino Uno. A distance sensor measures how far away the nearest object is, and a buzzer plays a sound with variable frequency: **the closer the object, the higher the pitch**.

Author: Nate Newhart and Colin Whitty, 2026

## How it works

1. Press the button to turn the system **on**. Press it again to turn it **off**.
2. While on, the sketch pings the ultrasonic sensor every 20 ms.
3. Each reading is stored in a rolling buffer of the last 5 distances, and the **median** of those 5 is used. [noise reduction]
4. If the smoothed distance is within range, it is mapped linearly to a tone frequency:

## Hardware

| Part | Notes |
|------|-------|
| Arduino Uno | Any compatible board should work |
| HC-SR04 ultrasonic sensor | Or a similar 5 V trigger/echo module |
| Passive buzzer | Must be passive (driven by `tone()`) |
| Momentary push button | Uses the internal pull-up resistor |
| Breadboard and jumper wires | |

## Wiring

| Component | Arduino pin |
|-----------|-------------|
| Button (one leg) | D9 |
| Button (other leg) | GND |
| Buzzer (+) | D10 |
| Buzzer (-) | GND |
| Ultrasonic TRIG | A0 (used as a digital output) |
| Ultrasonic ECHO | D11 |
| Ultrasonic VCC | 5V |
| Ultrasonic GND | GND |

The button uses `INPUT_PULLUP`, so no external resistor is needed. The pin reads LOW when pressed.

## Getting started

1. Wire the circuit as shown above.
2. Open `ArduinoUnoBlindProject.ino` in the Arduino IDE (the file must live in a folder with the same name).
3. Select **Arduino Uno** under Tools > Board and choose the correct port.
4. Click **Upload**.
5. Press the button and move your hand toward the sensor. The pitch should rise as you get closer.

## Serial output

Open the Serial Monitor at **9600 baud** to see each raw distance reading in meters (4 decimal places). This is useful for checking the sensor and tuning the range.

## Code overview

- `setup()` configures the pins, starts Serial, and fills the buffer with 1.0 m so the system starts quiet.
- `loop()` handles button debouncing and toggling, then triggers the sensor, filters the reading, and drives the buzzer.
- `median5()` copies the buffer, insertion-sorts it, and returns the middle value.
- `lerp()` linearly interpolates between two integer values.
