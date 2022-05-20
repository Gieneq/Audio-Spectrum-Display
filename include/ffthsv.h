#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include "presets.h"
#include "analyser.h"
#include "effects.h"
#include "cmath"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

extern int bars[BANDS_COUNT];

void FFTHSV_set_effects(const effect_handler_t* effects, const int count);
void FFTHSV_select_effect(int index);

void FFTHSV_begin();
void FFTHSV_update();