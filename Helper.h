#pragma once
#ifndef HEPLER
#define HEPLER

#include "esp_heap_caps.h"
#include <cmath>

/*
* - Print ram of proccesser
*/
void printDeviceRam() {
  Serial.print("FREE RAM: ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.print("/");
  Serial.print(ESP.getHeapSize() / 1024);
  Serial.println("_KB");
}


float arrMin(float arr[], int size){
  float min = arr[0];
  for(size_t i = 0; i < size; i++){
    if(arr[i] < min){
      min = arr[i];
    }
  }
  return min;
}

float arrMax(float arr[], int size){
  float max = arr[0];
  for(size_t i = 0; i < size; i++){
    if(arr[i] > max){
      max = arr[i];
    }
  }
  return max;
}

float arrMean(float arr[], int size){
    float sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += arr[i];
    }
    return (sum / size);
}

float arrStd(float arr[], int size){
  float _arrMean = arrMean(arr, size);
  float sum = 0;
  for (size_t i = 0; i < size; i++) {
      float _val = arr[i] - _arrMean;
        sum += (_val * _val);
    }
  return sqrt(sum / size);
}

/*_____________________________________________AI__________________________________________*/
const int DATA_NUM = 1200;
const int AZ_DATA_NUM = 200;
const int FEATURE_NUM = 4;

float INPUT_DATA[DATA_NUM];
float AZ_DATA[AZ_DATA_NUM];

int currPos = 0;
int shiftVal = 6 * 40;
float DATA_FEATURES[FEATURE_NUM];

void shiftInputDataLeft(){
  currPos = DATA_NUM - shiftVal;
  for(size_t i = 0; i < currPos; i++){
    INPUT_DATA[i] = INPUT_DATA[i + shiftVal];
  }
}

void addData(float ax, float ay, float az, float gx, float gy, float gz){

  if(currPos >= DATA_NUM){
    shiftInputDataLeft();
  }

  INPUT_DATA[currPos++] = ax;
  INPUT_DATA[currPos++] = ay;
  INPUT_DATA[currPos++] = az;

  INPUT_DATA[currPos++] = gx;
  INPUT_DATA[currPos++] = gy;
  INPUT_DATA[currPos++] = gz;
}

void caculateFeature(){
  DATA_FEATURES[0] = arrMin(INPUT_DATA, DATA_NUM);
  DATA_FEATURES[1] = arrMax(INPUT_DATA, DATA_NUM);
  DATA_FEATURES[2] = arrMean(INPUT_DATA, DATA_NUM);
  DATA_FEATURES[3] = arrStd(INPUT_DATA, DATA_NUM);
}

void getAzData(){
  int j = 2;
  for(size_t i = 0; i < 200; i++){
    AZ_DATA[i] = INPUT_DATA[j];
    j += 6;
  }
}
/*__________________________________________END AI__________________________________________*/

#endif

