#pragma once
#include "Arduino.h"
#include "FastLED.h"


typedef enum {
    ENERGY_CURRENT,
    ENERGY_BURST,
    ENERGY_MEAN
} energy_t;

typedef void (*effect_handler_t)(const int* bar_heights, const double* bar_thrs, double* energy, CRGB* display_leds, CRGB* base_leds);

/*
 * DEBUG
 */
#define DEBUG_FPS 0

/*
 * SETTINGS
 */
#define DRAW_INTERVAL_MS 33
#define UPDATE_INTERVAL_MS 16

/*
 * HARDWARE LEDS
 */
#define ZIGZAGING 1
#define LED_TYPE WS2812B
constexpr uint8_t DISPLAY_PIN = 18;
constexpr uint8_t BASE_PIN = 19;
constexpr auto COLOR_ORDER = GRB;
constexpr int MIN_BRIGHTNESS = 30;
constexpr int TOP_BRIGHTNES = 250;

/*
 * DISPLAY SHAPE
 */
constexpr int BANDS_COUNT = 19;
constexpr int BAR_HEIGHT = 21;
constexpr int DISPLAY_W = BANDS_COUNT;
constexpr int DISPLAY_H = BAR_HEIGHT;
constexpr int DISPLAY_LEDS_COUNT = BANDS_COUNT * BAR_HEIGHT;

/*
 * BASE SHAPE
 */
constexpr int BASE_LEDS_COUNT = 2 * 7;

/*
 * HARDWARE BUTTONS
 */
constexpr uint8_t BUTTON_RED_PIN = 1;
constexpr uint8_t BUTTON_YELLOW_UP_PIN = 2;
constexpr uint8_t BUTTON_YELLOW_DOWN_PIN = 3;
constexpr uint8_t BUTTON_BLUE_PIN = 4;

/*
 * HARDWARE AUDIO INPUT
 */
constexpr uint8_t AUX_PIN = A6; //jack-micro
constexpr uint8_t MICRO_PIN = A3; // not used
constexpr uint8_t AUDIO_PIN = AUX_PIN;

/*
 * FFT SETTINGS
 */
constexpr int SAMPLES_COUNT = 512;
constexpr double SAMPLING_FREQUENCY = 40000.0;

/*
 * EFFECTS
 */
#define EFFECTS_SHUFFLE_INTERVAL 30000
#define EFFECTS_COUNT 1