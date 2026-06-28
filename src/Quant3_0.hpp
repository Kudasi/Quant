#pragma once

#define Q3 300

#define MUX_IN1  7
#define MUX_IN2  8
#define MUX_IN3 A1
#define MUX_OUT A0

template <uint16_t Features>
class Quant<Q3, Features> : public QuantBase<8, 3, Features> {
public:

  Quant() : QuantBase<8, 3, Features>() {}

  void _begin() override {
    pinMode(5,  OUTPUT);
    pinMode(6,  OUTPUT);
    pinMode(9,  OUTPUT);
    pinMode(10, OUTPUT);
    
    if constexpr (Features & DEBUG) {
      Serial.println("Running version 3.0!");       
    }
  }

  void _readLineSensors() override {
    this->lineSensorBits = 0;
  }

  void _readIMU() override {
    
  }
  
};
