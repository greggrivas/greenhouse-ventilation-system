#include <Servo.h>

// ---------- PIN DEFINITIONS ----------
const int TEMP_PIN = A0;      
const int FAN_PIN = 9;        
const int SERVO_PIN = 6;      

// ---------- FAN / SERVO SETTINGS ----------
const float TEMP_FAN_ON   = 28.0;  
const float TEMP_WINDOW_ON = 32.0; 
const float TEMP_RESET     = 26.0;

const int WINDOW_CLOSED = 300;
const int WINDOW_OPEN   = 10;

Servo ventServo;

// ---------- FSM STATES ----------
enum SystemState {
  IDLE,       // everything off
  COOLING,    // fan only
  VENTING     // fan + window
};

SystemState currentState = IDLE;

// ---------- SETUP ----------
void setup() {
  Serial.begin(9600);
  pinMode(FAN_PIN, OUTPUT);
  ventServo.attach(SERVO_PIN);
  ventServo.write(WINDOW_CLOSED);
}

// ---------- MAIN LOOP ----------
void loop() {
  float tempC;

  // ---------- SIMULATION (REMOVE WHEN REAL SENSOR CONNECTED) ----------
  static float sim = 25;
  tempC = sim;
  sim += 0.5;
  if(sim > 34) sim = 20;

  Serial.print("Temp: ");
  Serial.println(tempC);

  switch(currentState) {

    case IDLE:
      digitalWrite(FAN_PIN, LOW);
      ventServo.write(WINDOW_CLOSED);
      if(tempC >= TEMP_FAN_ON) {
        currentState = COOLING;
        Serial.println("Transition: IDLE → COOLING");
      }
      break;

    case COOLING:
      digitalWrite(FAN_PIN, HIGH);
      ventServo.write(WINDOW_CLOSED);
      if(tempC >= TEMP_WINDOW_ON) {
        currentState = VENTING;
        Serial.println("Transition: COOLING → VENTING");
      }
      if(tempC <= TEMP_RESET) {
        currentState = IDLE;
        Serial.println("Transition: COOLING → IDLE");
      }
      break;

    case VENTING:
      digitalWrite(FAN_PIN, HIGH);
      ventServo.write(WINDOW_OPEN);
      if(tempC <= TEMP_FAN_ON) {
        currentState = COOLING;
        Serial.println("Transition: VENTING → COOLING");
      }
      break;
  }

  delay(1000);
}
