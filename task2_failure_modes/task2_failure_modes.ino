#include <Servo.h>

Servo myServo;
#define SERVO_PIN 9

float simulateVoltage(bool underLoad) {
  if (underLoad) return 4.1 + random(0, 30) / 100.0;
  return 4.9 + random(0, 20) / 100.0;
}

int addSignalNoise(int pulse, bool noisyLine) {
  if (noisyLine) return pulse + random(-120, 120);
  return pulse + random(-5, 5);
}

bool stallWatchdog(float current_mA) {
  return current_mA > 1800.0;
}

void demo1_normalOperation() {
  Serial.println("\n(1) Normal Operation");
  for (int i = 0; i < 4; i++) {
    int pulse = addSignalNoise(1500, false);
    float volt = simulateVoltage(false);
    myServo.writeMicroseconds(pulse);
    Serial.print("Pulse: "); Serial.print(pulse);
    Serial.print("us - Voltage: "); Serial.print(volt, 2);
    Serial.println("V - STATUS: OK");
    delay(600);
  }
}

void demo2_signalJitter() {
  Serial.println("\n(2) Noisy Signal Line - Servo Jitters");
  Serial.println("CAUSE: Long unshielded wire near power cables");
  Serial.println("FIX: 100ohm series resistor + shielded cable");
  for (int i = 0; i < 6; i++) {
    int pulse = addSignalNoise(1500, true);
    myServo.writeMicroseconds(pulse);
    Serial.print("Corrupted Pulse: "); Serial.print(pulse);
    Serial.println("us - STATUS: JITTER FAULT");
    delay(300);
  }
}

void demo3_brownout() {
  Serial.println("\n(3) Voltage Brownout Under Load");
  Serial.println("CAUSE: Undersized supply, thin wires");
  Serial.println("FIX: 1000uF capacitor on power rail, 18AWG wire");
  for (int i = 0; i < 5; i++) {
    float volt = simulateVoltage(true);
    Serial.print("Voltage: "); Serial.print(volt, 2); Serial.print("V");
    if (volt < 4.5) Serial.println(" WARNING: BROWNOUT - servo may reset!");
    else            Serial.println(" OK");
    delay(500);
  }
}

void demo4_stallWatchdog() {
  Serial.println("\n(4) Stall Detection & Watchdog Recovery");
  Serial.println("CAUSE: Mechanical blockage or overload");
  Serial.println("FIX: Current watchdog returns servo to safe position");
  float currents[] = {150, 400, 900, 1500, 1850, 2100};
  for (float curr : currents) {
    Serial.print("Current: "); Serial.print(curr); Serial.print("mA");
    if (stallWatchdog(curr)) {
      Serial.println(" STALL DETECTED - returning to neutral!");
      myServo.writeMicroseconds(1500);
      delay(1000);
      break;
    } else {
      Serial.println(" Normal");
    }
    delay(400);
  }
}

void demo5_thermalWarning() {
  Serial.println("\n(5) Thermal Overload Simulation");
  Serial.println("CAUSE: Running near stall torque continuously");
  Serial.println("FIX: Duty cycle limit, higher-rated servo, heatsink");
  int temp = 25;
  for (int i = 0; i < 6; i++) {
    temp += random(4, 9);
    Serial.print("Body Temp: "); Serial.print(temp); Serial.print("C");
    if (temp > 65) {
      Serial.println(" THERMAL SHUTDOWN - servo disabled!");
      myServo.writeMicroseconds(1500);
      break;
    } else if (temp > 50) {
      Serial.println(" WARNING: High temperature");
    } else {
      Serial.println(" OK");
    }
    delay(500);
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  myServo.attach(SERVO_PIN);
  Serial.println("  SERVO FAILURE MODE SIMULATOR  ");
}

void loop() {
  demo1_normalOperation();
  demo2_signalJitter();
  demo3_brownout();
  demo4_stallWatchdog();
  demo5_thermalWarning();
  Serial.println("\nCYCLE COMPLETE - restarting in 5s ");
  delay(5000);
}
