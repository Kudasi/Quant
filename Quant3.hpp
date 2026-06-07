#pragma once

#define Q3 300

#define MUX_IN1  7
#define MUX_IN2  8
#define MUX_IN3 A1
#define MUX_OUT A0

#define LS_COUNT 8

template <bool Debug>
class Quant<Q3, Debug> : public QuantBase {
public:
  
  Quant() : QuantBase(LS_COUNT, 3) {}

  void begin() override {
    pinMode(5,  OUTPUT);
    pinMode(6,  OUTPUT);
    pinMode(9,  OUTPUT);
    pinMode(10, OUTPUT);
    
    if constexpr (Debug) {
      Serial.println("Running version 3.0!");       
    }
  }

  void readLineSensors() override {
    lineSensors = 0;
  }

  void readIMU() override {
    
  }
  
};
