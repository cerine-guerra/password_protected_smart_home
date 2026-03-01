#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad Setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin Definitions
const int buzzerPin = 13;
const int redLedPin = 10;
const int greenLedPin = 12;
const int resetButtonPin = 11;

// Password Variables
String password = "1234"; // Default password
String inputPassword = "";
bool passwordResetMode = false;
bool accessGranted = false;

// Reset button timing variables
unsigned long lastResetPress = 0;
const unsigned long resetDebounce = 500; // 500ms debounce

void setup() {
  // Initialize pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  
  // Initialize LEDs to OFF
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize Serial Communication
  Serial.begin(9600);
  
  // Show initial screen
  showMainScreen();
}

void loop() {
  // Check reset button with debouncing
  if (digitalRead(resetButtonPin) == LOW) {
    if (millis() - lastResetPress > resetDebounce) {
      lastResetPress = millis();
      enterPasswordResetMode();
    }
  }
  
  // Get key input
  char key = keypad.getKey();
  
  if (key) {
    if (passwordResetMode) {
      handlePasswordReset(key);
    } else {
      handlePasswordInput(key);
    }
  }
  
 // Add to master's loop() function - REPLACE THE EXISTING SECTION
if (Serial.available() > 0) {
  char pumpStatus = Serial.read();
  if (pumpStatus == 'P') {
    lcd.clear();
    lcd.print("Pump Activated");
    lcd.setCursor(0, 1);
    lcd.print("Running...");
  } else if (pumpStatus == 'F') {
    lcd.clear();
    lcd.print("Pump Finished");
    lcd.setCursor(0, 1);
    lcd.print("Ready for use");
    delay(2000);
    showAccessScreen(); // Return to access screen
  }
}// Add to master's loop() function - REPLACE THE EXISTING SECTION
if (Serial.available() > 0) {
  char pumpStatus = Serial.read();
  if (pumpStatus == 'P') {
    lcd.clear();
    lcd.print("Pump Activated");
    lcd.setCursor(0, 1);
    lcd.print("Running...");
  } else if (pumpStatus == 'F') {
    lcd.clear();
    lcd.print("Pump Finished");
    lcd.setCursor(0, 1);
    lcd.print("Ready for use");
    delay(2000);
    showAccessScreen(); // Return to access screen
  }
}
}

void showMainScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  lcd.print(">");
  
  // Reset input password when showing main screen
  inputPassword = "";
}

void handlePasswordInput(char key) {
  if (key == '#') {
    // Check password
    if (inputPassword == password) {
      // Correct password
      accessGranted = true;
      lcd.clear();
      lcd.print("Access Granted!");
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH); // Turn on green LED
      
      // Send access granted signal to Arduino 2
      Serial.write('G');
      
      delay(2000);
      showAccessScreen();
    } else {
      // Wrong password
      accessGranted = false;
      lcd.clear();
      lcd.print("Wrong Password!");
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW); // Ensure green LED is off
      digitalWrite(buzzerPin, HIGH);
      delay(3000);
      digitalWrite(buzzerPin,LOW);
      
      // Send access denied signal
      Serial.write('D');
      
      delay(3000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(redLedPin, LOW);
      showMainScreen();
    }
    inputPassword = "";
    } else if (key == '*') {
    // Clear input and RELOCK the system
    inputPassword = "";
    accessGranted = false;
    digitalWrite(greenLedPin, LOW); // Turn off green LED when clearing
    
    // Send access denied signal to Arduino 2 to revoke authorization
    Serial.write('D');
    
    Serial.println("System Relocked - Authorization revoked");
    showMainScreen();
  } else {
    // Add digit to password
    if (inputPassword.length() < 4) {
      inputPassword += key;
      lcd.setCursor(inputPassword.length(), 1);
      lcd.print('*');
    }
  }
}

void showAccessScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted!");
  lcd.setCursor(0, 1);
  lcd.print("Gate & Pump OK");
}

void enterPasswordResetMode() {
  passwordResetMode = true;
  inputPassword = "";
  
  lcd.clear();
  lcd.print("Password Reset");
  lcd.setCursor(0, 1);
  lcd.print("Mode Activated");
  delay(1500);
  
  lcd.clear();
  lcd.print("Enter NEW 4-digit");
  lcd.setCursor(0, 1);
  lcd.print("password:");
  delay(2000);
  
  lcd.clear();
  lcd.print("New Password:");
  lcd.setCursor(0, 1);
  lcd.print(">");
}

void handlePasswordReset(char key) {
  if (key == '#') {
    if (inputPassword.length() == 4) {
      // Set new password
      password = inputPassword;
      
      lcd.clear();
      lcd.print("Password Reset!");
      lcd.setCursor(0, 1);
      lcd.print("New: " + password);
      
      // Blink green LED to confirm reset
      for (int i = 0; i < 3; i++) {
        digitalWrite(greenLedPin, HIGH);
        delay(300);
        digitalWrite(greenLedPin, LOW);
        delay(300);
      }
      
      delay(2000);
      passwordResetMode = false;
      accessGranted = false;
      showMainScreen();
    } else {
      lcd.clear();
      lcd.print("Must be 4 digits!");
      delay(2000);
      lcd.clear();
      lcd.print("New Password:");
      lcd.setCursor(0, 1);
      lcd.print(">");
    }
    inputPassword = "";
  } else if (key == '*') {
    // Cancel password reset
    inputPassword = "";
    passwordResetMode = false;
    accessGranted = false;
    digitalWrite(greenLedPin, LOW);
    
    lcd.clear();
    lcd.print("Reset Cancelled");
    delay(1500);
    showMainScreen();
  } else {
    if (inputPassword.length() < 4) {
      inputPassword += key;
      lcd.setCursor(inputPassword.length(), 1);
      lcd.print('*');
    }
  }
}