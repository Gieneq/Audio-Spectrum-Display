#pragma once

#include <Arduino.h>
#include "arduinoFFT.h"
#include "bands.h"

#define AUX_PIN A6
#define MICRO_PIN A3

constexpr int AUDIO_PIN = AUX_PIN; // A3 - module, A0 - not used anymore, A6 - jack

constexpr uint16_t SAMPLES_COUNT = 512; //1024

// constexpr double AMPLITUDE = 1024.0;

constexpr double SAMPLING_FREQUENCY = 40000.0;

constexpr int BANDS_COUNT = 19;
constexpr int COLS_HEIGHT = 21;

// int min_bar_height;
// int max_bar_height;
// int bar_heights[BANDS_COUNT];

void analyser_begin();
void sample_blocking();
void load_samples(int read_index, volatile int* read_buffer, const int buffersize);
void calculate_bars();
void feedTone(double frequency);
double* get_magnitudes();
