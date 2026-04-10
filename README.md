# Embedded Countdown Timer (ATmega328P)

## Overview
This project implements a fully functional countdown timer system using an ATmega328P microcontroller (Arduino Uno). The system allows users to configure time using potentiometers and control execution through buttons.

The timer displays time in **MM:SS format** on a 4-digit 7-segment display and triggers an audible alarm when the countdown reaches zero.

##  Features
- Time configuration using potentiometers (minutes & seconds)
- Finite State Machine (FSM) architecture
- Real-time countdown with 1-second resolution
- +30 seconds increment during runtime
- PWM-based buzzer alarm (30 seconds duration)
- Debounced button input handling
- Multiplexed 7-segment display control

##  System Architecture
The system is implemented using a **Finite State Machine**:

- **IDLE** – waiting for user input  
- **CONFIG_MIN** – set minutes  
- **CONFIG_SEC** – set seconds  
- **COUNTDOWN** – timer running  
- **ALARM** – buzzer active  


## Hardware Components
- Arduino Uno (ATmega328P)
- 4-digit 7-segment display (common cathode)
- 2x potentiometers
- 2x push buttons
- Buzzer


## Technologies Used
- Embedded C / Arduino
- AVR architecture
- PWM (for buzzer)
- ADC (for potentiometers)
- Digital I/O & timers


## Testing
The system was tested using:
- Branch Coverage (white-box testing)
- Functional testing (black-box)
- Boundary value testing
- Debounce validation for buttons


## Key Implementation Details
- Implemented time decrement logic with second-level precision using `millis()`
- Designed modular functions for time handling (`tickTime`, `add30Seconds`)
- Implemented button debouncing to ensure stable input
- Used multiplexing technique for efficient control of 4-digit display


## Note
This project was developed and tested in the **Wokwi simulator**, without physical hardware.


## How to Run
1. Open project in Arduino IDE or Wokwi
2. Upload the code to Arduino Uno
3. Use potentiometers to set time
4. Press button to start countdown


## Demo
![Circuit](circuit.png)


## Author
Lavinia-Monica Rotariu
