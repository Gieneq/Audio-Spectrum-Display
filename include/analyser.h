#pragma once

#include <Arduino.h>
#include "arduinoFFT.h"
#include "bands.h"
#include "presets.h"

void analyser_begin();
void sample_blocking();
void load_samples(int read_index, volatile int* read_buffer, const int buffersize);
void calculate_bars();
void feedTone(double frequency);
double* get_magnitudes();