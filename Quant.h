#pragma once

#include <Arduino.h>

class QuantBase {

protected:

  uint8_t lineSensors; // Hope there will not be some version with more than 8 line sensors...
  uint8_t lineSensorCount, wallSensorCount;
  float left = 0.0, right = 0.0, speed = 0.8, balance = 0.0;
  float accX = 0.0, accY = 0.0, accZ = 0.0, roll = 0.0, pitch = 0.0, yaw = 0.0;
  
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

  // IMU
  virtual void readIMU();

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
    if (k) return direction / k;
    return 0;
  }

  uint8_t getWallSensorCount() {
    return wallSensorCount;
  }

  float getAccX() {
    return accX;
  }

  float getAccY() {
    return accY;
  }

  float getAccZ() {
    return accY;
  }

  float getRoll() {
    return roll;
  }

  float getPitch() {
    return pitch;
  }

  float getYaw() {
    return yaw;
  }
  
  void setLeft(float left) {
    this -> left = left;
  }
  
  void setRight(float right) {
    this -> right = right;
  }
  
  void setSpeed(float speed) {
    this -> speed = speed;
  }
  
  void setBalance(float balance) {
    this -> balance = balance;
  }
};


template <uint16_t Version, bool Debug = false>
class Quant;

#include "Quant2.hpp"
#include "Quant3.hpp"
#include "Quant3_1.hpp"
