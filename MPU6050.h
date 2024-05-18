#pragma once
#ifndef MY_MPU6050
#define MY_MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void SetupMPU(){
  Serial.println("SETUP MPU6050...");

  Wire.begin(16, 0);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip!");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("FINISH SETUP MPU6050.");
}

#endif