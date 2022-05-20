#include "effects.h"

/*
 * TOOLS
 */
inline int get_idx(int x, int y) {
  x = DISPLAY_W - x - 1;
  #if ZIGZAGING == 1
      return (x&1 ? DISPLAY_H - y - 1 : y) + x * DISPLAY_H; //todo - tablica?
  #else
      return y + x * DISPLAY_H;
  #endif
}

void inline set_hsv(CRGB* hsv_leds, int x, int y, uint8_t h, uint8_t s, uint8_t v) {
  hsv_leds[get_idx(x, y)] = CHSV(h,s,v);
}

/*
 * FIRE
 */
constexpr uint8_t FIRE_TONGUE_HUE[] = {12, 16, 32, 33, 34, 35, 36, 37, 37, 37, 37, 36, 35, 34, 33, 20, 19, 18, 255, 255, 255};
int burnt[DISPLAY_LEDS_COUNT];
constexpr uint8_t FIRE_HUE = 22; 
constexpr uint8_t BURNT_HSV_VALUE = 32;
constexpr uint8_t BURNT_HUE = 254;
constexpr long BURN_INTERVAL = 300;
long last_burn_time;
extern const uint8_t NOISE_FLAMES[];
int twinkle_counter;
constexpr int FIRE_BASE_TICS = 6;
constexpr int FIRE_RANDOM_TICS = 24;
int total_brightness = MIN_BRIGHTNESS;

long last_burst_time;
constexpr int BRURST_INTERVAL = 2;

void fire_effect(const int* bar_heights, const double* bar_thrs, double* energy, CRGB* display_leds, CRGB* base_leds){
    ++twinkle_counter;

    static int reduce_burn;
    if(millis() - last_burn_time > BURN_INTERVAL){
      reduce_burn = 1;
      last_burn_time += BURN_INTERVAL;
    }

  int base_hue = constrain(int(energy[energy_t::ENERGY_MEAN]), 0, DISPLAY_H);
  for(int base_x = 0; base_x < BASE_LEDS_COUNT; base_x++)
    base_leds[base_x] = CHSV(FIRE_TONGUE_HUE[base_hue], 255,255);
  

    if(total_brightness < TOP_BRIGHTNES/2) {
      if(energy[energy_t::ENERGY_CURRENT] - energy[energy_t::ENERGY_MEAN] > 4)
        total_brightness = TOP_BRIGHTNES;
    }


    if (millis() - last_burst_time > BRURST_INTERVAL) {
      if(total_brightness > MIN_BRIGHTNESS)
        total_brightness-=5;
      if(total_brightness < MIN_BRIGHTNESS)
        total_brightness = MIN_BRIGHTNESS;

      last_burst_time += BRURST_INTERVAL;
    }

    FastLED.setBrightness(total_brightness);

    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
      for(int display_y = 0; display_y < DISPLAY_H; display_y++)
        set_hsv(display_leds, display_x, display_y, 0,0,255);

      for(int display_y = 0; display_y < bar_heights[display_x]; display_y++) {
        int factor = DISPLAY_H - (bar_heights[display_x] - display_y - 1) - 1; // dla BH=4, 17, 18, 19, 20
        uint8_t fire_hue = FIRE_TONGUE_HUE[factor];

        uint8_t fire_saturation = 255;
        if(factor < 10)
          fire_saturation -= (10-factor-1) * 28;

        uint8_t fire_value = 255;
        if(factor > 9 && factor < 21){
          fire_value = NOISE_FLAMES[(twinkle_counter + display_y + display_x*6)%128];
          
          if(factor > 9 && factor < 17){
            //factor: 10...16
            double mix_factor = (factor - 10)/6.0;
            fire_value = (mix_factor * fire_value + (1.0 - mix_factor) * 255); //mix
            if(fire_value>255)
              fire_value = 255;
          }
        }

        set_hsv(display_leds, display_x, display_y, fire_hue, fire_saturation, fire_value);
        burnt[get_idx(display_x, display_y)] = FIRE_BASE_TICS + random(FIRE_RANDOM_TICS);
      } 

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++) {
        int idx = get_idx(display_x, display_y);
        if(burnt[idx] > 0){
          set_hsv(display_leds, display_x, display_y, BURNT_HUE, 255, BURNT_HSV_VALUE);
          if(reduce_burn == 1)
            burnt[idx]--;
        } else
          set_hsv(display_leds, display_x, display_y, 0, 0, 255);
      }
    }


}
