#pragma once

#include <Wire.h>

#define Q3_2 302

#define MUX_IN1 7
#define MUX_IN2 8
#define MUX_IN3 A1
#define MUX_OUT A0

#define WS_ANALOG_TO_RELATIVE(pin) (1.0 - pow(analogRead(pin) / 1000.0, 1.0))


template <uint16_t Features>
class Quant<Q3_2, Features> : public QuantBase<6, 3, Features>
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


    if constexpr (Features & DEBUG)
    {
      Serial.println("Running version 3.2!");
    }
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
      this->lineSensorBits |= !digitalRead(MUX_OUT);
    }

    if constexpr (Features & DEBUG) lineSensorsDebug();
  }

  void _setThreshold(uint16_t threshold) override
  {
    Serial.println("Quant 3.2 threshold is set physically, not via software!");
  }

  void _readWallSensors() override
  {
    this->wallSensorValues[0] = WS_ANALOG_TO_RELATIVE(3);
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
    analogWrite(5, ((this->left > 0) - (this->left * this->speed * min(1.0 - this->balance, 1.0))) * 255);
    analogWrite(10, ((this->right > 0) - (this->right * this->speed * min(1.0 + this->balance, 1.0))) * 255);
    digitalWrite(6, this->left > 0);
    digitalWrite(9, this->right > 0);
  }
};
