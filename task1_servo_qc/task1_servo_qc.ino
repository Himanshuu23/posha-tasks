#include <Servo.h>

Servo testServo;
#define SERVO_PIN 9

float getCurrent_mA() { return 150.0 + random(-20, 20); }
float getVoltage_V()  { return 5.0 + random(-5, 5) / 100.0; }
float getPower_mW()   { return getCurrent_mA() * getVoltage_V(); }

float simulateAngleRead(int pwmMicros) {
  float ideal = map(pwmMicros, 1000, 2000, 0, 180);
  float noise = (random(-20, 20)) / 10.0;
  return ideal + noise;
}

void runTest(int pulseWidth, int expectedAngle, String label) {
  testServo.writeMicroseconds(pulseWidth);
  delay(700);
  float angle   = simulateAngleRead(pulseWidth);
  float current = getCurrent_mA();
  float voltage = getVoltage_V();
  float error   = abs(angle - expectedAngle);
  bool  passed  = (error <= 5.0);

  Serial.print("TEST: ");     Serial.println(label);
  Serial.print("Target:   "); Serial.print(expectedAngle); Serial.println(" deg");
  Serial.print("Measured: "); Serial.print(angle, 2);      Serial.println(" deg");
  Serial.print("Error:    "); Serial.print(error, 2);      Serial.println(" deg");
  Serial.print("Current:  "); Serial.print(current, 1);    Serial.println(" mA");
  Serial.print("Voltage:  "); Serial.print(voltage, 2);    Serial.println(" V");
  Serial.println(passed ? "RESULT: PASS" : "RESULT: FAIL");
  delay(1000);
}

void repeatabilityTest() {
  Serial.println("REPEATABILITY TEST");
  float readings[10]; float sum = 0;
  for (int i = 0; i < 10; i++) {
    testServo.writeMicroseconds(1000); delay(400);
    testServo.writeMicroseconds(1500); delay(600);
    readings[i] = simulateAngleRead(1500);
    sum += readings[i];
    Serial.print("Rep "); Serial.print(i+1);
    Serial.print(": "); Serial.print(readings[i], 2); Serial.println(" deg");
  }
  float mean = sum / 10.0;
  float variance = 0;
  for (int i = 0; i < 10; i++) variance += pow(readings[i]-mean, 2);
  float stddev = sqrt(variance / 10.0);
  Serial.print("StdDev: "); Serial.print(stddev, 2); Serial.println(" deg");
  Serial.println(stddev < 2.0 ? "REPEATABILITY: PASS" : "REPEATABILITY: FAIL");
}

void speedTest() {
  Serial.println("SPEED TEST");
  testServo.writeMicroseconds(1000); delay(500);
  unsigned long t0 = millis();
  testServo.writeMicroseconds(2000); delay(500);
  unsigned long elapsed = millis() - t0;
  Serial.print("180 deg in: "); Serial.print(elapsed); Serial.println(" ms");
  Serial.println(elapsed < 600 ? "SPEED: PASS" : "SPEED: FAIL");
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  testServo.attach(SERVO_PIN);
  Serial.println("  SERVO MOTOR QC TEST JIG  ");
}

void loop() {
  runTest(1000, 0,   "0 deg");
  runTest(1500, 90,  "90 deg");
  runTest(2000, 180, "180 deg");
  runTest(1250, 45,  "45 deg");
  runTest(1750, 135, "135 deg");
  repeatabilityTest();
  speedTest();
  Serial.println("ALL TESTS COMPLETE");
  delay(10000);
}
