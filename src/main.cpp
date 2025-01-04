#include <PS4Controller.h>

//Right motor
int pwmRightMotor = 12; // Pin do sterowania prędkością prawego silnika
int dirRightMotor = 13; // Pin do sterowania kierunkiem prawego silnika
//Left motor
int pwmLeftMotor = 14;  // Pin do sterowania prędkością lewego silnika
int dirLeftMotor = 27;  // Pin do sterowania kierunkiem lewego silnika

// LED pins
const int redLedPin = 2;    // Pin for red LED
const int greenLedPin = 4;  // Pin for green LED
bool redLedState = false;    // Track red LED state
bool greenLedState = false;  // Track green LED state

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

void notify()
{
  int rightMotorSpeed, leftMotorSpeed;

  // Mapowanie prędkości
  rightMotorSpeed = map(PS4.RStickY(), -127, 127, -255, 255); // Right stick - Y-axis
  leftMotorSpeed = map(PS4.LStickY(), -127, 127, -255, 255);  // Left stick - Y-axis

  // Dead zone to ignore small deviations from neutral position
  if (abs(rightMotorSpeed) < 30) rightMotorSpeed = 0;
  if (abs(leftMotorSpeed) < 30) leftMotorSpeed = 0;

  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);
  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);

  Serial.println(rightMotorSpeed);
  Serial.println(leftMotorSpeed);  

  rotateMotor(rightMotorSpeed, leftMotorSpeed);
}

void onConnect()
{
  Serial.println("Connected!");
}

void onDisConnect()
{
  rotateMotor(0, 0);
  // Turn off LEDs on disconnect
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  redLedState = false;
  greenLedState = false;
  Serial.println("Disconnected!");
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  // Prawy silnik
  if (rightMotorSpeed > 30)
  {
    digitalWrite(dirRightMotor, HIGH); // Kierunek do przodu
    ledcWrite(rightMotorPWMSpeedChannel, map(rightMotorSpeed, 30, 255, 255, 0)); 
  }
  else if (rightMotorSpeed < -30)
  {
    digitalWrite(dirRightMotor, LOW); // Kierunek wstecz
    ledcWrite(rightMotorPWMSpeedChannel, map(abs(rightMotorSpeed), 30, 255, 0, 255)); 
  }
  else
  {
    digitalWrite(dirRightMotor, LOW);
    ledcWrite(rightMotorPWMSpeedChannel, 0); // Całkowite zatrzymanie
  }

  // Lewy silnik
  if (leftMotorSpeed > 30)
  {
    digitalWrite(dirLeftMotor, HIGH); // Kierunek do przodu
    ledcWrite(leftMotorPWMSpeedChannel, map(leftMotorSpeed, 30, 255, 255, 0)); 
  }
  else if (leftMotorSpeed < -30)
  {
    digitalWrite(dirLeftMotor, LOW); // Kierunek wstecz
    ledcWrite(leftMotorPWMSpeedChannel, map(abs(leftMotorSpeed), 30, 255, 0, 255)); 
  }
  else
  {
    digitalWrite(dirLeftMotor, LOW);
    ledcWrite(leftMotorPWMSpeedChannel, 0); // Całkowite zatrzymanie
  }
}

void setUpPinModes()
{
  pinMode(pwmRightMotor, OUTPUT);
  pinMode(dirRightMotor, OUTPUT);
  pinMode(pwmLeftMotor, OUTPUT);
  pinMode(dirLeftMotor, OUTPUT);
  
  // Setup LED pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);    // Initially off
  digitalWrite(greenLedPin, LOW);  // Initially off

  // Konfiguracja PWM dla prędkości silników
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(pwmRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(pwmLeftMotor, leftMotorPWMSpeedChannel);

  rotateMotor(0, 0); // Zatrzymanie silników na starcie
}

void setup()
{
  setUpPinModes();
  Serial.begin(115200);
  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
  Serial.println("Ready.");
}

void loop()
{
  if (PS4.isConnected()) {
    static bool lastDownState = false;
    static bool lastUpState = false;
    
    // Handle Down button for red LED
    if (PS4.Down()) {
      if (!lastDownState) {  // Button just pressed
        redLedState = !redLedState;  // Toggle state
        digitalWrite(redLedPin, redLedState ? HIGH : LOW);
        Serial.println(redLedState ? "Red LED ON" : "Red LED OFF");
      }
      lastDownState = true;
    } else {
      lastDownState = false;
    }
    
    // Handle Up button for green LED
    if (PS4.Up()) {
      if (!lastUpState) {  // Button just pressed
        greenLedState = !greenLedState;  // Toggle state
        digitalWrite(greenLedPin, greenLedState ? HIGH : LOW);
        Serial.println(greenLedState ? "Green LED ON" : "Green LED OFF");
      }
      lastUpState = true;
    } else {
      lastUpState = false;
    }
  }
}