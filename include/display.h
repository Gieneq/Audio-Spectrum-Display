#pragma once

#include "Arduino.h"
#include "FastLED.h"
#include "tools.h"
#include "presets.h"

constexpr int NOISE_FLAMES_count = 128;
const uint8_t NOISE_FLAMES[] = {199, 218, 234, 245, 252, 255, 253, 248, 239, 228, 216, 204, 191, 180, 170, 163, 157, 154, 152, 152, 153, 155, 157, 159, 160, 160, 159, 157, 154, 151, 147, 143, 139, 136, 134, 133, 133, 134, 136, 138, 140, 143, 144, 146, 146, 145, 143, 140, 136, 131, 127, 122, 119, 116, 114, 114, 114, 116, 119, 123, 127, 131, 135, 137, 139, 139, 138, 136, 132, 128, 123, 119, 115, 112, 110, 110, 111, 114, 118, 124, 130, 137, 143, 148, 151, 153, 153, 151, 147, 141, 135, 127, 120, 114, 108, 105, 104, 105, 109, 115, 122, 131, 140, 148, 155, 160, 163, 162, 157, 150, 139, 126, 110, 94, 79, 64, 53, 44, 40, 40, 45, 55, 69, 87, 108, 131, 155, 178};
enum class effect_t {
    ORIGINAL,
    CLASSIC,
    COLOROFON,
    COLOROFON_EXTENDED,
    FIRE,
    PULSE,
    MATRIX
};

void display_begin();
void display_show();
void set_effect(effect_t effect_type);
void inline set_rgb(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void inline set_hsv(int x, int y, uint8_t h, uint8_t s, uint8_t v);
void fill_rgb(uint8_t r, uint8_t g, uint8_t b);
void test_bench();
void display_update(const int* bar_heights, const double* bar_thrs, double energy, double burst_energy, double mean_energy);
