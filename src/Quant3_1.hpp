#pragma once

#include <Wire.h>

#define Q3_1 301

#define MUX_IN1 7
#define MUX_IN2 8
#define MUX_IN3 A1
#define MUX_OUT A0

#define WS_ANALOG_TO_RELATIVE(pin) (pow(analogRead(pin) / 400.0, 0.6))

#define MPU_ADDR 0x68
#define MPU_CONFIG 0x1A
#define MPU_ACCEL_CONFIG 0x1C
#define MPU_ACCEL_START 0x3B
#define MPU_POWER_MGMT 0x6B

#define SCALE 16384.0
#define GRAVITY 9.80665

template <uint16_t Features>
class Quant<Q3_1, Features> : public QuantBase<6, 3, Features>
{

public:

  Quant() : QuantBase<6, 3, Features>() {}

  void _begin() override
  {
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    pinMode(MUX_IN1, OUTPUT);
    pinMode(MUX_IN2, OUTPUT);
    pinMode(MUX_IN3, OUTPUT);

    Wire.begin();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_POWER_MGMT);
    Wire.write(0);
    int err = Wire.endTransmission();

    if constexpr (Features & DEBUG)
    {
      if (err)
      {
        Serial.print("Error: IMU initialization failed! Code: ");
        Serial.println(err);
      }
      Serial.println("Running version 3.1!");
    }

    if (err)
      return;

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_ACCEL_CONFIG);
    Wire.write(0);
    Wire.endTransmission();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_CONFIG);
    Wire.write(0x4);
    Wire.endTransmission();
  }

  void _readLineSensors() override
  {
    this->lineSensorBits = 0;
    for (int i = 5; i >= 0; i--)
    {
      digitalWrite(MUX_IN1, (i / 1) % 2);
      digitalWrite(MUX_IN2, (i / 2) % 2);
      digitalWrite(MUX_IN3, (i / 4) % 2);

      this->lineSensorBits <<= 1;
      this->lineSensorBits |= digitalRead(MUX_OUT);
    }

    if constexpr (Features & DEBUG) lineSensorsDebug();
  }

  void _setThreshold(uint16_t threshold) override
  {
    Serial.println("Quant 3.1 threshold is set physically, not via software!");
  }

  void _readWallSensors() override
  {
    this->wallSensorValues[0] = WS_ANALOG_TO_RELATIVE(5);
    this->wallSensorValues[1] = WS_ANALOG_TO_RELATIVE(6);
    this->wallSensorValues[2] = WS_ANALOG_TO_RELATIVE(7);

    if constexpr (Features & DEBUG) wallSensorsDebug();
  }

  void _readIMU() override
  {
    // Wire.beginTransmission();
    // Wire.write(MP);
  }

  void _updateMotors() override
  {
    analogWrite(5, ((this->left < 0) - (this->left * this->speed * min(1.0 - this->balance, 1.0))) * 255);
    analogWrite(10, ((this->right < 0) - (this->right * this->speed * min(1.0 + this->balance, 1.0))) * 255);
    digitalWrite(6, this->left < 0);
    digitalWrite(9, this->right < 0);
  }
};
