#pragma once
#include "presets.h"
#include "Arduino.h"
// #include "soc/rtc_wdt.h"
// #include "esp_task_wdt.h" // esp_task_wdt_reset();
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

void calc_movement(double* thrs, double* pos, double* vel, double grav, int datalen, double dt, double* diffs);
void calc_energy_eval(int* bar_heights, double* bar_thrs, double* energy, double* burst_energy, double* mean_energy);
void feedTheDog();