#pragma once

#define Q2 200

#define LS_COUNT 5
#define WS_COUNT 3
#define WS_ANALOG_TO_RELATIVE(pin) (1.0 - pow(analogRead(pin) / 300.0, 0.5))

template <bool Debug>
class Quant<Q2, Debug> : public QuantBase {
private:
  float wallSensors[WS_COUNT];
  
public:

  Quant() : QuantBase(LS_COUNT, 3) {}

  void begin() override {
    for (int i = 5; i <= 10; i++) pinMode(i, OUTPUT);

    if constexpr (Debug) {
      Serial.println("Running version 2.0!");
    }
  }

  void readLineSensors() override {
    lineSensors = ~PORTC & 31;
    
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
    Serial.println("Quant 2.0 threshold is set physically, not via software!");
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
    Serial.println("Quant 2.0 have no IMU!");
  }

  void updateMotors() override {
    analogWrite(5 , fabs(left)  * speed * 255 * min(1.0 - balance, 1.0));
    analogWrite(10, fabs(right) * speed * 255 * min(1.0 + balance, 1.0));

    digitalWrite(6, left  < 0);
    digitalWrite(7, left  > 0);
    digitalWrite(8, right > 0);
    digitalWrite(9, right < 0);
  }

};
