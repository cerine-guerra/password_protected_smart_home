# Smart Gate & Water Pump Automation System

A multi-functional, password-protected automation system using **Arduino**, designed to demonstrate real-world control of gates, water pumps, and LEDs with secure access.

---

## Features
- **Password-protected access:** Only authorized users can operate the gate and water pump.  
- **Gate automation:** Opens when a person or object is detected using an ultrasonic sensor.  
- **Water pump control:** Activate manually via button, accessible only after correct password.  
- **Secret password reset:** Hidden button for secure password changes.  
- **Day/Night LED control:** Night LEDs turn on automatically using an LDR sensor.  
- **Dual Arduino system:** Master handles input & display; Slave controls outputs.  
- **Interactive feedback:** Red/green LEDs and buzzer indicate system status.  

---

## Components Used
- 2 × Arduino Uno  
- 4×4 Keypad  
- I²C LCD Display  
- Servo Motor (Gate)  
- Ultrasonic Sensor (Distance detection)  
- Relay Module + DC Water Pump  
- LDR Sensor  
- LEDs & Pushbuttons  
- Supporting resistors, transistors, and wiring  

---

## Technical Highlights
- **I²C communication** between master and slave Arduino  
- **Non-blocking timing** using `millis()`  
- **Efficient pin management** for multiple components  
- **Masked password input** and reset functionality  
- **Expandable modular design** for sensors/actuators  

---

## How it Works
1. Enter password on the keypad.  
2. If correct, the gate opens when distance is detected and water pump can be activated.  
3. Night LEDs automatically turn on in darkness.  
4. Hidden button allows secure password reset.  

---

## Images / Demo
- **Simulation View**  
![Simulation](images/simulation.jpn)

- **Implementation View**  
![Implementation](images/implementation.jpn)  

- **Real Circuit**  
![Real Project](images/real_project.jpg)  

> Replace the `images/...` paths with your actual image files in the repository.
