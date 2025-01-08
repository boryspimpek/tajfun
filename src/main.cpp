#include <PS4Controller.h>

//Right motor
int pwmRightMotor = 12; // Pin do sterowania prędkością prawego silnika
int dirRightMotor = 13; // Pin do sterowania kierunkiem prawego silnika
//Left motor
int pwmLeftMotor = 14;  // Pin do sterowania prędkością lewego silnika
int dirLeftMotor = 27;  // Pin do sterowania kierunkiem lewego silnika

// LED pins
const int turnSigLeft = 21;      
const int turnSigRight = 19;     
const int parkLight = 5;    
const int lowBeamLight = 18; 
const int highBeamLight = 17; 
const int brakeLight = 4; 
const int tailLight = 2; 
const int reversLight = 16; 

// Variables to track LED states
bool turnSigLeftState = false;    
bool turnSigRightState = false;  
bool parkLightState = false;
bool lowBeamLightState = false;
bool highBeamLightState = false;
bool brakeLightState = false;
bool tailLightState = false;
bool reversLightState = false;

// PWM setup
const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

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

void notify()
{
  int rightMotorSpeed, leftMotorSpeed;

  // Mapowanie prędkości i cud miud
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
  digitalWrite(turnSigLeft, LOW);
  digitalWrite(turnSigRight, LOW);
  digitalWrite(parkLight, LOW);
  digitalWrite(lowBeamLight, LOW);
  digitalWrite(highBeamLight, LOW);
  digitalWrite(brakeLight, LOW);
  digitalWrite(tailLight, LOW);
  digitalWrite(reversLight, LOW);

  turnSigLeftState = false;
  turnSigRightState = false;
  parkLightState = false;
  lowBeamLightState = false;
  highBeamLightState = false;
  brakeLightState = false;
  tailLightState = false;
  reversLightState = false;

  Serial.println("Disconnected!");
}

void setUpPinModes()
{
  pinMode(pwmRightMotor, OUTPUT);
  pinMode(dirRightMotor, OUTPUT);
  pinMode(pwmLeftMotor, OUTPUT);
  pinMode(dirLeftMotor, OUTPUT);
  
  // Setup LED pins
  pinMode(turnSigLeft, OUTPUT);
  pinMode(turnSigRight, OUTPUT);
  pinMode(parkLight, OUTPUT);
  pinMode(lowBeamLight, OUTPUT);
  pinMode(highBeamLight, OUTPUT);
  pinMode(brakeLight, OUTPUT);
  pinMode(tailLight, OUTPUT);
  pinMode(reversLight, OUTPUT);
  digitalWrite(turnSigLeft, LOW);    // Initially off
  digitalWrite(turnSigRight, LOW);  // Initially off
  digitalWrite(parkLight, LOW);    // Initially off
  digitalWrite(lowBeamLight, LOW);  // Initially off
  digitalWrite(highBeamLight, LOW); // Initially off
  digitalWrite(brakeLight, LOW);    // Initially off
  digitalWrite(tailLight, LOW);     // Initially off

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
    static bool lastLeftState = false;
    static bool lastRightState = false;
    static bool lastL1State = false;
    static bool lastR1State = false;
    static bool lastUpState = false;
    
    // Handle Left button for Left signal led
    if (PS4.Left()) {
      if (!lastLeftState) {  // Button just pressed
        turnSigLeftState = !turnSigLeftState;  // Toggle state
        digitalWrite(turnSigLeft, turnSigLeftState ? HIGH : LOW);
        Serial.println(turnSigLeftState ? "Left sig ON" : "Left sig OFF");
      }
      lastLeftState = true;
    } else {
      lastLeftState = false;
    }
    
    // Handle Right button Right signal led
    if (PS4.Right()) {
      if (!lastRightState) {  // Button just pressed
        turnSigRightState = !turnSigRightState;  // Toggle state
        digitalWrite(turnSigRight, turnSigRightState ? HIGH : LOW);
        Serial.println(turnSigRightState ? "Right sig ON" : "Right sig OFF");
      }
      lastRightState = true;
    } else {
      lastRightState = false;
    }

    // Handle L1 button for low beam light and tail light
    if (PS4.L1()) {
      if (!lastL1State) {  // Button just pressed
        lowBeamLightState = !lowBeamLightState;  // Toggle state
        tailLightState = !tailLightState;  // Toggle state
        digitalWrite(lowBeamLight, lowBeamLightState ? HIGH : LOW);
        digitalWrite(tailLight, tailLightState ? HIGH : LOW);
        Serial.println(lowBeamLightState ? "Low beam ON" : "Low beam OFF");
        Serial.println(tailLightState ? "Tail light ON" : "Tail light OFF");
      }
      lastL1State = true;
    }  else {
      lastL1State = false;
    }

    // Handle R1 button for high beam light
    if (PS4.R1()) {
      if (!lastR1State) {  // Button just pressed
        highBeamLightState = !highBeamLightState;  // Toggle state
        digitalWrite(highBeamLight, highBeamLightState ? HIGH : LOW);
        Serial.println(highBeamLightState ? "High beam ON" : "High beam OFF");
      }
      lastR1State = true;
    }  else {
      lastR1State = false;
    }

    // Handle Up button for park light
    if (PS4.Up()) {
      if (!lastUpState) {  // Button just pressed
        parkLightState = !parkLightState;  // Toggle state
        digitalWrite(parkLight, parkLightState ? HIGH : LOW);
        Serial.println(parkLightState ? "Park light ON" : "Park light OFF");
      }
      lastUpState = true;
    }  else {
      lastUpState = false;
    }

    // Handle reverse light
    if ((PS4.LStickY() < -30 || PS4.RStickY() < -30) && digitalRead(2) == HIGH) {
      digitalWrite(reversLight, HIGH);
      reversLightState = true;
    } else {
      digitalWrite(reversLight, LOW);
      reversLightState = false;
    }

    // Handle brake light
    if (abs(PS4.LStickY()) <= 30 && abs(PS4.RStickY()) <= 30 && digitalRead(2) == HIGH) {
      digitalWrite(brakeLight, HIGH);
      brakeLightState = true;
    } else {
      digitalWrite(brakeLight, LOW);
      brakeLightState = false;
    }
  }
}