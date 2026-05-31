#pragma once

#include <Arduino.h>

class QuantBase {

protected:

  uint8_t lineSensors; // Hope there will not be some version with more than 8 line sensors...
  uint8_t lineSensorCount, wallSensorCount;
  float left, right, speed, balance;
  
  QuantBase(uint8_t lineSensorCount, uint8_t wallSensorCount) {
    this->lineSensorCount = lineSensorCount;
    this->wallSensorCount = wallSensorCount;
  };
  
public:

  // Initialisation
  virtual void begin();

  // Line sensors
  virtual void readLineSensors();
  virtual void setThreshold(uint16_t threshold);

  // Wall sensors
  virtual void readWallSensors();
  virtual float getWallSensorValue(uint8_t sensor);

  // Motors
  virtual void updateMotors();

  
  bool getLineSensorValue(uint8_t sensor) {
    return lineSensors & (1 << sensor);
  }

  uint8_t getLineSensorCount() {
    return lineSensorCount;
  }

  float getLineSensorsDirection() {
    float direction = 0.0;
    int k = 0;
    for (int i = 0, c = lineSensorCount; i < c; i++) {
      bool sensor = getLineSensorValue(i);
      direction += (i + 0.5 - c / 2.0) * sensor;
      k += sensor;
    }
    return direction / k;
  }

  uint8_t getWallSensorCount() {
    return wallSensorCount;
  }
  
  void setLeft(float left) {
    this -> left = left;
    updateMotors();
  }
  
  void setRight(float right) {
    this -> right = right;
    updateMotors();
  }
  
  void setSpeed(float speed) {
    this -> speed = speed;
    updateMotors();
  }
  
  void setBalance(float balance) {
    this -> balance = balance;
    updateMotors();
  }
};


template <uint16_t Version, bool Debug = false>
class Quant;

#include "Quant2.hpp"
#include "Quant3.hpp"
#include "Quant3_1.hpp"
