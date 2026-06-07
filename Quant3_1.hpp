#pragma once

#include <Wire.h>

#define Q3_1 301

#define MUX_IN1  7
#define MUX_IN2  8
#define MUX_IN3 A1
#define MUX_OUT A0

#define LS_COUNT 6
#define WS_COUNT 3

#define WS_ANALOG_TO_RELATIVE(pin) (pow(analogRead(pin) / 300.0, 0.5))

#define MPU_ADDR           0x68
#define MPU_CONFIG     0x1A
#define MPU_ACCEL_CONFIG   0x1C
#define MPU_ACCEL_START    0x3B
#define MPU_POWER_MGMT 0x6B

#define SCALE   16384.0
#define GRAVITY 9.80665

template <bool Debug>
class Quant<Q3_1, Debug> : public QuantBase {
private:
  float wallSensors[WS_COUNT];
  
public:
  
  Quant() : QuantBase(LS_COUNT, 3) {}

  void begin() override {
    pinMode(5,  OUTPUT);
    pinMode(6,  OUTPUT);
    pinMode(9,  OUTPUT);
    pinMode(10, OUTPUT);
    
    pinMode(MUX_IN1, OUTPUT);
    pinMode(MUX_IN2, OUTPUT);
    pinMode(MUX_IN3, OUTPUT);

    Wire.begin();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_POWER_MGMT);
    Wire.write(0);
    int err = Wire.endTransmission();
    
    if constexpr (Debug) {
      if (err) {
        Serial.print("Error: IMU initialization failed! Code: ");
        Serial.println(err);
      }
      Serial.println("Running version 3.1!");
    }

    if (err) return;

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_ACCEL_CONFIG);
    Wire.write(0);
    Wire.endTransmission();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_CONFIG);
    Wire.write(0x4);
    Wire.endTransmission();
  }

  void readLineSensors() override {
    lineSensors = 0;
    for (int i = LS_COUNT - 1; i >= 0; i--) {
      digitalWrite(MUX_IN1, (i / 1) % 2);
      digitalWrite(MUX_IN2, (i / 2) % 2);
      digitalWrite(MUX_IN3, (i / 4) % 2);
      
      lineSensors <<= 1;
      lineSensors |= digitalRead(MUX_OUT);
    }

    if constexpr (Debug) {
      Serial.print("Line sensors updated: [ ");
      for(int i = 0; i < LS_COUNT; i++) {
        Serial.print((lineSensors & (1 << i)) > 0);
        Serial.print(" ");
      }
      Serial.println("]");
    }
  }

  void setThreshold(uint16_t threshold) override {
    Serial.println("Quant 3.1 threshold is set physically, not via software!");
  }

  void readWallSensors() override {
    wallSensors[0] = WS_ANALOG_TO_RELATIVE(5);
    wallSensors[1] = WS_ANALOG_TO_RELATIVE(6);
    wallSensors[2] = WS_ANALOG_TO_RELATIVE(7);  
  }

  float getWallSensorValue(uint8_t sensor) override {
    return wallSensors[sensor];
  }

  void readIMU() override {
    // Wire.beginTransmission();
    // Wire.write(MP);
  }

  void updateMotors() override {
    analogWrite(5 , ((left  < 0) - (left  * speed * min(1.0 - balance, 1.0))) * 255);
    analogWrite(10, ((right < 0) - (right * speed * min(1.0 + balance, 1.0))) * 255);
    digitalWrite(6, left  < 0);
    digitalWrite(9, right < 0);
  }
};
