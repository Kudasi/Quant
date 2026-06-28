#pragma once

#include <Arduino.h>

// Features

/**
 * @brief Enables debug logging into Serial port.
 **/ 
#define DEBUG 1


/**
 * @brief Bluetooth Inline Control. Allows to control car via bluetooth even if it driven by external code.
 * To enter BTIC mode Quant need to receive any bluetooth command. It will automatically return to default mode
 * after 5 seconds of inactivity. Commands processing runs inside updateMotors() function, so you need to call it
 * in your code every loop() cycle for this feature to work properly. 
 *  
 * Bluetooth command list can be found at https://github.com/Kudasi/Quant.
 **/
#define BTIC 2


#define DEFAULT_FEATURES 0


/**
 * @brief Main class base defining main functionality and methods, providing one API for all Quant versions.
 * 
 * @param LineSensors Number of line sensors for Quant version.
 * @param WallSensors Number of wall sensors for Quant version (who knows, maybe there will be version with more than 3 wall sensors).
 */
template <uint8_t LineSensors, uint8_t WallSensors, uint16_t Features>
class QuantBase
{

private:
  /**
   * @brief Helper function for counting line sensors.
   */
  uint8_t countBits(uint8_t bits)
  {
    int count = 0;
    while (bits)
    {
      bits &= (bits - 1);
      count++;
    }
    return count;
  }

protected:
  /**
   * @brief Bitset containing line sensors states. 0 - no line, 1 - line.
   */
  uint16_t lineSensorBits;

  /**
   * @brief Array containing values from wall sensors in range [0.0; 1.0] (if properly tuned).
   */
  float wallSensorValues[WallSensors];

  /**
   * @brief Used for controlling motors throttle/steering.
   */
  float left = 0.0, right = 0.0, speed = 0.8, balance = 0.0;

  /**
   * IMU related variables.
   */
  float accX = 0.0, accY = 0.0, accZ = 0.0, roll = 0.0, pitch = 0.0, yaw = 0.0;


  void lineSensorsDebug()
  {
    if constexpr (Features & DEBUG) {
      Serial.print("Line sensors updated: [ ");
      for(int i = 0; i < LineSensors; i++) {
        Serial.print((lineSensorBits & (1 << i)) > 0);
        Serial.print(" ");
      }
      Serial.print("]    Direction: \t");
      Serial.print(getLineSensorsDirection());
      Serial.print("   Triggered: \t");
      Serial.println(getLineSensorsTriggered());
    }
  }

  void wallSensorsDebug()
  {
    if constexpr (Features & DEBUG)
    {  
      Serial.print("Wall sensors updated: [");
      for (int i = 0; i < WallSensors; i++) 
      {
        Serial.print(wallSensorValues[i]);
        Serial.print("\t");
      }
      Serial.println("]");
    }
  }

  virtual void _begin();
  virtual void _setThreshold(uint16_t threshold);
  virtual void _readLineSensors();
  virtual void _readWallSensors();
  virtual void _readIMU();
  virtual void _updateMotors();

public:

  QuantBase() {}
  
  /**
   * @brief Initialization function. Sets up pin modes, peripherals, e.t.c.
   */
  void begin() 
  {
    _begin();
  }

  /**
   * @brief Updates stored line sensors states.
   */
  void readLineSensors()
  {
    _readLineSensors();
  }

  /**
   * @brief Sets analog level threshold for line sensor counting as "triggered".
   * 
   * @param threshold Analog level threshold.
   */
  void setThreshold(uint16_t threshold)
  {
    _setThreshold(threshold);
  }

  /**
   * @brief Updates stored wall sensors values;
   */
  void readWallSensors()
  {
    _readWallSensors();
  }

  /**
   * @brief Updates stored IMU sensor values.
   */
  void readIMU()
  {
    _readIMU();
  }

  /**
   * @brief Updates signals on motor driver(s) based on throttle, speed and balance values.
   */
  void updateMotors()
  {
    _updateMotors();
  }

  /**
   * @param sensor Index of wall sensor (beginning from left) to obtain it's value.
   * @returns Wall sensor value in range [0.0; 1.0]. Call readWallSensors() first.
   */
  float getWallSensorValue(uint8_t sensor)
  {
    return wallSensorValues[sensor];
  }

  uint8_t getWallSensorCount()
  {
    return WallSensors;
  }


  /**
   * @returns Line sensor state stored in bitset. Call readLineSensors() first.
   */
  bool getLineSensorValue(uint8_t sensor)
  {
    return lineSensorBits & (1 << sensor);
  }

  uint8_t getLineSensorCount()
  {
    return LineSensors;
  }

  /**
   * @brief Line sensor direction is a value in range [-1.0; 1.0] which describes in which direction and how much Quant
   * should rotate to align with the line. In that case, 0 - can mean three things: car is perfetly centered, car is not 
   * seeing line (held in hands), car is on crossing (all sensors are triggered).
   * 
   * This value primarily supposed to be used in pair with stabilization algorithms (such as PID).
   * 
   * If you prefer to use branching based line following (via if/else) you may refer to getLineSensorValue().
   */
  float getLineSensorsDirection()
  {
    int k = countBits(lineSensorBits);
    if (!k)
      return 0;
    float direction = 0.0;
    for (int i = 0, c = LineSensors; i < c; i++)
    {
      bool sensor = getLineSensorValue(i);
      direction += (i + 0.5 - c / 2.0) / (c - 1) * 2.0 * sensor;
    }
    return direction / k;
  }

  /**
   * @returns Amount of line sensors which "seeing" line. May be used for detecting takeoffs (0 sensors)
   * or crossings (all sensors).
   */
  uint8_t getLineSensorsTriggered()
  {
    return countBits(lineSensorBits);
  }

  float getAccX()
  {
    return accX;
  }

  float getAccY()
  {
    return accY;
  }

  float getAccZ()
  {
    return accY;
  }

  float getRoll()
  {
    return roll;
  }

  float getPitch()
  {
    return pitch;
  }

  float getYaw()
  {
    return yaw;
  }

  /**
   * @brief Sets both motors throttle speed and direction: -1 - backward, 1 - forward, 0 - stop.
   * @param left Left motor throttle [-1.0; 1.0].
   * @param right Right motor throttle [-1.0; 1.0].
   */
  void throttle(float left, float right)
  {
    this->left = constrain(left, -1.0, 1.0);
    this->right = constrain(right, -1.0, 1.0);
  }

  /**
   * @brief Left motor throttle.
   */
  void throttleLeft(float left)
  {
    this->left = constrain(left, -1.0, 1.0);
  }

  /**
   * @brief Right motor throttle.
   */
  void throttleRight(float right)
  {
    this->right = constrain(right, -1.0, 1.0);
  }

  /**
   * @brief Set overall speed multiplier.
   * 
   * @param speed in [0.0, 1.0] range.
   */
  void setSpeed(float speed)
  {
    this->speed = constrain(speed, 0.0, 1.0);
  }

  /**
   * @brief Set the motor balance in range [-1.0; 1.0], where zero means equal PWM on both motors,
   * negative values less PWM on left and positive less PWM on right.
   * 
   * @param balance The balance value.
   */
  void setBalance(float balance)
  {
    this->balance = constrain(balance, -1.0, 1.0);
  }

  [[deprecated("use \"throttle\" methods")]]
  void setLeft(float left)
  {
    this->left = constrain(left, -1.0, 1.0);
  }

  [[deprecated("use \"throttle\" methods")]]
  void setRight(float right)
  {
    this->right = constrain(right, -1.0, 1.0);
  }
};

template <uint16_t Version, uint16_t Features = DEFAULT_FEATURES>
class Quant;

#include "Quant2_0.hpp"
#include "Quant3_0.hpp"
#include "Quant3_1.hpp"
#include "Quant3_2.hpp"