#include "display.h"

effect_t effect = effect_t::ORIGINAL;

//TOOLS
long last_burst_time;
constexpr int BRURST_INTERVAL = 2;

//CLASSIC EFFECT
constexpr uint8_t CLASSIC_BAR_HUE[] = {100, 100, 100, 100, 100, 100, 100, 100, 60, 60, 60, 60, 60, 60, 60, 60, 0, 0, 0, 0, 0};

//COLOROFON & COLOROFON_EXTENDED EFFECTS
constexpr long COLOROFON_HUE_INTERVAL = 100;
long last_colorofon_hue_time;
constexpr int COLOROF_BLOCKS = 3;
constexpr int COLOROFON_BAR_BINS[] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2};
int colorofon_hue_phases[COLOROF_BLOCKS];
int colorofon_extended_hue_phase;

// FIRE EFFECT
constexpr uint8_t FIRE_TONGUE_HUE[] = {12, 16, 32, 33, 34, 35, 36, 37, 37, 37, 37, 36, 35, 34, 33, 20, 19, 18, 255, 255, 255};
int burnt[DISPLAY_LEDS_COUNT];
constexpr uint8_t FIRE_HUE = 22; 
constexpr uint8_t BURNT_HSV_VALUE = 32;//static_cast<uint8_t>(-0.18 MEAN + 68);
constexpr uint8_t BURNT_HUE = 254;
constexpr long BURN_INTERVAL = 300;
long last_burn_time;
extern const uint8_t NOISE_FLAMES[];
int twinkle_counter;
constexpr int FIRE_BASE_TICS = 6;
constexpr int FIRE_RANDOM_TICS = 24;
int total_brightness = MIN_BRIGHTNESS;


// ELECTRIC
constexpr int PULSE_DEFAULT_HUE = 160;
constexpr int PULSE_HUE_GAP = 32;
constexpr int PULSE_TRHRESHOLD = 8;
int pulse_shooted[DISPLAY_W];
int pulse_height[DISPLAY_W];
constexpr long PULSE_INTERVAL = 14;
long last_pulse_time;

constexpr int PULSE_BUFF_LEN = 8;
int pulse_buff_end_index = 0;
int pulse_buff_front_index = PULSE_BUFF_LEN-1;
int pulse_buffer[DISPLAY_W * PULSE_BUFF_LEN];
uint8_t hue_shuffle[DISPLAY_W * 2];
constexpr int PULSE_TAIL_LEN = 8;

CRGB display_leds[DISPLAY_LEDS_COUNT];
CRGB base_leds[BASE_LEDS_COUNT];

void display_begin() {
  FastLED.addLeds<LED_TYPE, DISPLAY_PIN, COLOR_ORDER>(display_leds, DISPLAY_LEDS_COUNT).setCorrection(TypicalSMD5050);
  FastLED.addLeds<LED_TYPE, BASE_PIN, COLOR_ORDER>(base_leds, BASE_LEDS_COUNT).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MIN_BRIGHTNESS);
  fill_rgb(0, 0, 0);
  display_show();

  //preset equal distance
  for(int i = 0; i < COLOROF_BLOCKS; i++)
    colorofon_hue_phases[i] = uint8_t(i*255.0/COLOROF_BLOCKS);

  for(int i = 0; i < DISPLAY_W; i++){
    hue_shuffle[i * 2] = PULSE_DEFAULT_HUE;
    hue_shuffle[i * 2 + 1] = random(PULSE_DEFAULT_HUE-PULSE_HUE_GAP, PULSE_DEFAULT_HUE+PULSE_HUE_GAP);
  }

  // last_fire_hue_time = millis();
  last_burn_time = millis();
  last_colorofon_hue_time = millis();
  last_burst_time = millis();
}


static inline int get_idx(int x, int y) {
  x = DISPLAY_W - x - 1;
  #if ZIGZAGING == 1
      return (x&1 ? DISPLAY_H - y - 1 : y) + x * DISPLAY_H;
  #else
      return y + x * DISPLAY_H;
  #endif
}

void display_show(){
  FastLED.show();
}

void inline set_rgb(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  display_leds[get_idx(x, y)] = CRGB(r,g,b);
}

void inline set_hsv(int x, int y, uint8_t h, uint8_t s, uint8_t v) {
  display_leds[get_idx(x, y)] = CHSV(h,s,v);
}

void fill_rgb(uint8_t r, uint8_t g, uint8_t b) {
  for(int display_y = 0; display_y < DISPLAY_H; display_y++){
    for(int display_x = 0; display_x < DISPLAY_W; display_x++)
      set_rgb(display_x, display_y, r,g,b);
  }
}

void set_effect(effect_t effect_type) {
  effect = effect_type;
}


extern double energy;
extern double burst_energy;
extern double mean_energy;

void display_update(const int* bar_heights, const double* bar_thrs, double energy, double burst_energy, double mean_energy) {


  // ORIGINAL
  if (effect == effect_t::ORIGINAL) {
    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {

      for(int display_y = 0; display_y < bar_heights[display_x]; display_y++) {
        uint8_t hue = uint8_t(display_y * 255.0 / DISPLAY_H);
        set_hsv(display_x, display_y, hue,255,255);
      }
      
      if(bar_heights[display_x] > 0)
        set_rgb(display_x, bar_heights[display_x] - 1, 255,255,255);

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++)
        set_rgb(display_x, display_y, 0,0,0);
    }
  }

  // CLASSIC
  else if (effect == effect_t::CLASSIC) {
    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
      for(int display_y = 0; display_y < bar_heights[display_x]; display_y++) {
        uint8_t hue = CLASSIC_BAR_HUE[display_y];
        set_hsv(display_x, display_y, hue, 255, 255);
      }

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++)
        set_rgb(display_x, display_y, 0,0,0);
    }
  }

  // COLOROFON
  else if (effect == effect_t::COLOROFON) {
    if(millis() - last_colorofon_hue_time > COLOROFON_HUE_INTERVAL) {
      for(int i = 0; i < COLOROF_BLOCKS; i++){
        colorofon_hue_phases[i]++;
        colorofon_hue_phases[i] %= 255;
      }
      last_colorofon_hue_time += COLOROFON_HUE_INTERVAL;
    }

    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
      uint8_t hue = uint8_t(colorofon_hue_phases[COLOROFON_BAR_BINS[display_x]]);

      for(int display_y = 0; display_y < bar_heights[display_x]; display_y++)
        set_hsv(display_x, display_y, hue,255,255);

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++)
        set_rgb(display_x, display_y, 0,0,0);
    }
  }

  // COLOROFON_EXTENDED
  else if (effect == effect_t::COLOROFON_EXTENDED) {

    if(millis() - last_colorofon_hue_time > COLOROFON_HUE_INTERVAL) {
      colorofon_extended_hue_phase++;
      colorofon_extended_hue_phase%=255;
      last_colorofon_hue_time += COLOROFON_HUE_INTERVAL;
    }

    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
      uint8_t hue = uint8_t(int(colorofon_extended_hue_phase + display_x * 255.0 / DISPLAY_W) % 255);

      for(int display_y = 0; display_y < bar_heights[display_x]; display_y++) {
        set_hsv(display_x, display_y, hue,255,255);
      }
      
      if(bar_heights[display_x] > 0)
        set_rgb(display_x, bar_heights[display_x] - 1, 255,255,255);

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++)
        set_rgb(display_x, display_y, 0,0,0);
    }
  }

  // FIRE
  else if (effect == effect_t::FIRE) {
    ++twinkle_counter;

    static int reduce_burn;
    if(millis() - last_burn_time > BURN_INTERVAL){
      reduce_burn = 1;
      last_burn_time += BURN_INTERVAL;
    }

  int base_hue = constrain(int(mean_energy), 0, DISPLAY_H);
  for(int base_x = 0; base_x < BASE_LEDS_COUNT; base_x++)
    base_leds[base_x] = CHSV(FIRE_TONGUE_HUE[base_hue], 255,255);
  

    if(total_brightness < TOP_BRIGHTNES/2) {
      if(energy - mean_energy > 4)
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
        set_rgb(display_x, display_y, 0,0,0);

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

        set_hsv(display_x, display_y, fire_hue, fire_saturation, fire_value);
        burnt[get_idx(display_x, display_y)] = FIRE_BASE_TICS + random(FIRE_RANDOM_TICS);
      } 

      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++) {
        int idx = get_idx(display_x, display_y);
        if(burnt[idx] > 0){
          set_hsv(display_x, display_y, BURNT_HUE, 255, BURNT_HSV_VALUE);
          if(reduce_burn == 1)
            burnt[idx]--;
        } else
          set_rgb(display_x, display_y, 0, 0, 0);
      }
    }
  }

  // PULSE
  else if (effect == effect_t::PULSE) {
    ++twinkle_counter %= 128;

    if(millis() - last_pulse_time > PULSE_INTERVAL){
      for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
        if(pulse_shooted[display_x] == 1) {
          if(bar_heights[display_x] - pulse_height[display_x] > 0)
            pulse_height[display_x]++;
          else{
            pulse_shooted[display_x] = 0;
            pulse_height[display_x] = 0;
            hue_shuffle[display_x*2] = hue_shuffle[display_x*2 + 1];
            hue_shuffle[display_x*2 + 1] = random(PULSE_DEFAULT_HUE-PULSE_HUE_GAP, PULSE_DEFAULT_HUE+PULSE_HUE_GAP);
          }
        }
      }
      last_pulse_time += PULSE_INTERVAL;
    }

    ++pulse_buff_end_index %= PULSE_BUFF_LEN;
    ++pulse_buff_front_index %= PULSE_BUFF_LEN;
    
    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
      pulse_buffer[display_x * PULSE_BUFF_LEN + pulse_buff_front_index] = bar_heights[display_x];
      //czy ostatnio dolecial
      if(pulse_shooted[display_x] == 0) {
        if(pulse_buffer[display_x * PULSE_BUFF_LEN + pulse_buff_front_index] - pulse_buffer[display_x * PULSE_BUFF_LEN + pulse_buff_end_index] > PULSE_TRHRESHOLD) {
          //shoot from ground!
          pulse_shooted[display_x] = 1;
          pulse_height[display_x] = 0;
        }
      }
    }
    
    for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
        
      for(int display_y = 0; display_y < pulse_height[display_x]; display_y++) {
        uint8_t saturation = 255;
        if(display_y <= pulse_height[display_x]-1 && display_y > pulse_height[display_x]-PULSE_TAIL_LEN-1){
          if(display_y > pulse_height[display_x]-3) {
            saturation = 0;
          } else {
            uint8_t diff = pulse_height[display_x] - display_y-1-2;
            saturation = uint8_t(255.0*diff/PULSE_TAIL_LEN);
          }
        }
        //new color
        set_hsv(display_x, display_y, hue_shuffle[display_x*2 + 1], saturation, 255);
      }

      for(int display_y = pulse_height[display_x]; display_y < bar_heights[display_x]; display_y++) {

        set_hsv(display_x, display_y, hue_shuffle[display_x*2], 255, 255);
      }

      //clear
      for(int display_y = bar_heights[display_x]; display_y < DISPLAY_H; display_y++)
        set_rgb(display_x, display_y, 0,0,0);

      // if(pulse_shooted[display_x] == 1)
      //   set_hsv(display_x, pulse_height[display_x], 255, 0, 255);

    }
  }


}




// void draw_bars(int* bar_heights) {
//   for(int display_x = 0; display_x < DISPLAY_W; display_x++) {
//     for(int display_y = 0; display_y < DISPLAY_H; display_y++)
//       set_rgb(display_x, display_y, 0,0,0);

//     for(int display_y = 0; display_y < bar_heights[display_x]; display_y++) {
//       set_rgb(display_x, display_y, 0,120,0);
//     }
//     if(bar_heights[display_x] > 0)
//       set_rgb(display_x, bar_heights[display_x] - 1, 120,0,0);
//   }
// }