#include <Servo.h>

// Pin declarations
const int relayPin = 8;      // Relay connected to pin 8
const int buttonPin = 7;     // Push button connected to pin 7
const int trigPin = 10;      // Ultrasonic trig pin
const int echoPin = 11;      // Ultrasonic echo pin
const int servoPin = 9;      // Servo pin
const int ledPins[] = {2, 3, 4, 5, 6};  // LED pins
const int numLeds = 5;       // Number of LEDs

// Variables
int buttonState = 0;         // Current button state
int lastButtonState = 0;     // Previous button state
bool pumpRunning = false;    // Track if pump is running
bool authorized = false;     // Track if master Arduino authorized access
unsigned long pumpStartTime = 0;  // When pump started
const int pumpDuration = 5000;    // 5 seconds

Servo gateServo;  // Create servo object

void setup() {
  // Initialize pins
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize LED pins as outputs and turn them ON
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);  // Turn ON all LEDs
  }
  
  // Attach servo
  gateServo.attach(servoPin);
  gateServo.write(0);  // Start with servo at 0 degrees (closed position)
  
  // Start with pump OFF and NOT authorized
  digitalWrite(relayPin, HIGH);      // Relay OFF (active LOW)
  authorized = false;                // Explicitly set to false for safety
  
  // Initialize serial communication with master Arduino
  Serial.begin(9600);
  
  Serial.println("Slave Arduino Ready - Waiting for authorization");
  Serial.println("Pump button and gate disabled until authorized");
  Serial.println("All LEDs are ON");
}

void loop() {
  // Check for messages from master Arduino
  if (Serial.available() > 0) {
    char command = Serial.read();
    Serial.print("Received command: ");
    Serial.println(command);
    handleMasterCommand(command);
  }
  
  // Only check gate and button if authorized
  if (authorized) {
    // Check ultrasonic and control gate
    controlGate();
    
    // Read the button state
    buttonState = digitalRead(buttonPin);
    
    // Check for button press (LOW because of pull-up)
    if (buttonState == LOW && lastButtonState == HIGH) {
      // Button just pressed
      if (!pumpRunning) {
        startPump();
        // Notify master that pump was activated
        Serial.write('P');  // Send 'P' for pump activated
      }
    }
    
    // Check if pump should be turned off
    if (pumpRunning && (millis() - pumpStartTime >= pumpDuration)) {
      stopPump();
      // Notify master that pump finished
      Serial.write('F');  // Send 'F' for pump finished
    }
    
    // Save current button state for next loop
    lastButtonState = buttonState;
    
  } else {
    // If not authorized, make sure pump is off
    if (pumpRunning) {
      emergencyStopPump();
      Serial.println("Emergency stop - Access revoked");
    }
  }
  
  delay(50);  // Small delay for stability
}

void controlGate() {
  // Measure distance
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  // If object within 20cm, open gate
  if (distance <= 10 && distance > 0) {
    gateServo.write(90); // Open gate
    Serial.println("Gate: OPEN - Object detected");
    delay(2000); // Wait 2 seconds
    gateServo.write(0); // Close gate
    Serial.println("Gate: CLOSED");
    delay(2000); // Prevent multiple triggers
  }
}

void handleMasterCommand(char command) {
  switch(command) {
    case 'G':  // Access Granted
      authorized = true;
      Serial.println("ACCESS_GRANTED - Pump button and gate enabled");
      break;
      
    case 'D':  // Access Denied (also sent when '*' is pressed on master)
      authorized = false;
      // Immediately turn off pump if running
      if (pumpRunning) {
        emergencyStopPump();
        Serial.println("PUMP_FORCED_OFF - System relocked");
      } else {
        Serial.println("ACCESS_REVOKED - Pump and gate disabled");
      }
      break;
      
    default:
      // Ignore unknown commands
      break;
  }
}

void startPump() {
  digitalWrite(relayPin, LOW);   // Turn ON relay (pump on)
  pumpRunning = true;
  pumpStartTime = millis();
  Serial.println("Pump: ON - Running for 5 seconds");
}

void stopPump() {
  digitalWrite(relayPin, HIGH);  // Turn OFF relay (pump off)
  pumpRunning = false;
  Serial.println("Pump: OFF - Normal stop");
}

void emergencyStopPump() {
  digitalWrite(relayPin, HIGH);  // Turn OFF relay (pump off)
  pumpRunning = false;
  Serial.println("Pump: EMERGENCY STOP - Access revoked");
}