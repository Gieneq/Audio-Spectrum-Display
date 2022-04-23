#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include "presets.h"
#include "tools.h"
#include "analyser.h"
#include "effects.h"
#include "cmath"
// #include "soc/rtc_wdt.h"
// #include "esp_task_wdt.h" // esp_task_wdt_reset();
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

// void display_begin();
// void display_show();

void FFTHSV_set_effects(const effect_handler_t* effects, const int count);
void FFTHSV_select_effect(int index);
// void FFT_RGB_shuffle(bool shuffle_on);

void FFTHSV_begin();
void FFTHSV_update();