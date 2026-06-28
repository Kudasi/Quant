#pragma once

#define Q2_0 200
#define Q2 Q2_0

#define WS_ANALOG_TO_RELATIVE(pin) (1.0 - pow(analogRead(pin) / 300.0, 0.6))

template <uint16_t Features>
class Quant<Q2_0, Features> : public QuantBase<5, 3, Features> {
  
public:

  Quant() : QuantBase<5, 3, Features>() {}

  void _begin() override {
    for (int i = 5; i <= 10; i++) pinMode(i, OUTPUT);

    if constexpr (Features & DEBUG) {
      Serial.println("Running version 2.0!");
    }
  }

  void _readLineSensors() override {
    this->lineSensorBits = ~PINC & 31;
    
    if constexpr (Features & DEBUG) lineSensorsDebug();
  }

  void _setThreshold(uint16_t threshold) override {
    Serial.println("Quant 2.0 threshold is set physically, not via software!");
  }

  void _readWallSensors() override {
    this->wallSensorValues[0] = WS_ANALOG_TO_RELATIVE(5);
    this->wallSensorValues[1] = WS_ANALOG_TO_RELATIVE(6);
    this->wallSensorValues[2] = WS_ANALOG_TO_RELATIVE(7);
    
    if constexpr (Features & DEBUG) wallSensorsDebug();
  }

  void _readIMU() override {
    Serial.println("Quant 2.0 have no IMU!");
  }

  void _updateMotors() override {
    analogWrite(5 , fabs(this->left)  * this->speed * 255 * min(1.0 - this->balance, 1.0));
    analogWrite(10, fabs(this->right) * this->speed * 255 * min(1.0 + this->balance, 1.0));

    digitalWrite(6, this->left  < 0);
    digitalWrite(7, this->left  > 0);
    digitalWrite(8, this->right > 0);
    digitalWrite(9, this->right < 0);
  }

};
