#include <Servo.h>

#define TRIG_PIN 6
#define ECHO_PIN 7
#define SERVO_PIN 5
#define BUZZER_PIN 8

Servo radarServo;

int angle = 20;
int direction = 1;

bool locked = false;
unsigned long lockTime = 0;
unsigned long beepTimer = 0;
bool beepState = false;

int zone = 0; 
// 0 = safe, 1 = soft, 2 = critical

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 15000);
  if (duration == 0) return 999;

  return (duration / 2) / 29.1;
}

void setup() {
  // Use 115200 to communicate with ESP32 Serial2
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  radarServo.attach(SERVO_PIN);
  radarServo.write(angle);
}

void loop() {

  long distance = getDistance();

  // ---------------- ZONE DETECTION ----------------
  if (distance > 40) zone = 0;
  else if (distance >= 20 && distance <= 40) zone = 1;
  else if (distance >= 10 && distance < 20) zone = 2;
  else zone = 0;

  // ---------------- CRITICAL LOCK ----------------
  if (zone == 2 && !locked) {
    locked = true;
    lockTime = millis();
  }

  // ---------------- LOCK MODE (RED ZONE) ----------------
  if (locked) {
    radarServo.write(angle);

    // 🔥 SYNC ADDITION: Send LOCK alert to ESP32
    Serial.print("<LOCK:");
    Serial.print(angle);
    Serial.print(",");
    Serial.print(distance);
    Serial.println(">");

    // 🔴 FAST BEEP (CRITICAL)
    if (millis() - beepTimer > 100) {
      beepTimer = millis();
      beepState = !beepState;
      digitalWrite(BUZZER_PIN, beepState);
    }

    // unlock after 1.5 sec
    if (millis() - lockTime > 1500) {
      locked = false;
      digitalWrite(BUZZER_PIN, LOW);
    }

    delay(30); // Prevent flooding ESP32 serial
    return;
  }

  // ---------------- SOFT ZONE (YELLOW) ----------------
  if (zone == 1) {
    // 🟡 SLOW BEEP
    if (millis() - beepTimer > 600) {
      beepTimer = millis();
      beepState = !beepState;
      digitalWrite(BUZZER_PIN, beepState);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ---------------- SCAN MODE ----------------
  // 🔥 SYNC ADDITION: Send normal SCAN data to ESP32
  Serial.print("<SCAN:");
  Serial.print(angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.println(">");

  angle += direction * 2;

  if (angle >= 160) {
    angle = 160;
    direction = -1;
  }
  else if (angle <= 20) {
    angle = 20;
    direction = 1;
  }

  radarServo.write(angle);

  delay(25);
}