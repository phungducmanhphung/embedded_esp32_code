#pragma once
#ifndef FFTUTILS
#define FFTUTILS

#include "Helper.h"

const int SAMPLES = 200; // Số mẫu (200 mẫu thu được)
const int SAMPLING_FREQUENCY = 40; // Tần số lấy mẫu (Hz)
const int MAX_FRE_POS = 3;

float realPart[SAMPLES];
float imagPart[SAMPLES];

float signalData[SAMPLES];
float signalFrequency[SAMPLES / 2];

void dft(){
  for (int k = 0; k < SAMPLES/2; k++) {
    realPart[k] = 0;
    imagPart[k] = 0;

    for (int n = 0; n < SAMPLES; n++) {
      realPart[k] += signalData[n] * cos(2 * PI * n * k / SAMPLES);
      imagPart[k] -= signalData[n] * sin(2 * PI * n * k / SAMPLES);
    }
  }
  for (int k = 0; k < SAMPLES/2; k++) {
    signalFrequency[k] = sqrt(realPart[k] * realPart[k] + imagPart[k] * imagPart[k]);
  }
}

float getMaxFrequency(float arr[]){
  for(size_t i =0; i < SAMPLES; i++){
      signalData[i] = arr[i];
  }
  dft();
  int maxFPos = 0;
  float maxF = signalFrequency[0];
  for (int k = 0; k <= MAX_FRE_POS; k++) {
     if(maxF < signalFrequency[k]){
        maxFPos = k;
        maxF = signalFrequency[k];
     }
  }
  return maxFPos * 0.2;
}

#endif