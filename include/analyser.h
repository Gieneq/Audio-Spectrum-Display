#pragma once

#include <Arduino.h>
#include "arduinoFFT.h"
#include "presets.h"
#include "bands.h"

void analyser_begin();
void sample_blocking();
void load_samples(int read_index, volatile int* read_buffer, const int buffersize);
void calculate_bars();
double* get_magnitudes();
