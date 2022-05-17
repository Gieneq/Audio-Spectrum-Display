#include "ffthsv.h"

/*
 * VARIABLES
 */
// effect_t queue[] = {effect_t::FIRE, effect_t::PULSE, effect_t::CLASSIC, effect_t::COLOROFON, effect_t::ORIGINAL, effect_t::COLOROFON_EXTENDED};
// int effect_index;
// long next_effect_time;

CRGB display_leds[DISPLAY_LEDS_COUNT];
CRGB base_leds[BASE_LEDS_COUNT];

#define EFFECTS_MAX_COUNT 8
effect_handler_t effects_options[EFFECTS_MAX_COUNT];
int effects_count;
int current_effect_index;

volatile double mags[BANDS_COUNT];

long last_time_1;
int loops_counter_1;

long last_time_core1;
int loops_counter_core1;
double delta_time_core1;

long last_time_draw;
int loops_counter_draw;

long last_time_update;
int loops_counter_update;

double bars_threshold[BANDS_COUNT];
double bars_pos[BANDS_COUNT];
double diffs[BANDS_COUNT];

double dt;
double velocities[BANDS_COUNT];
double gravity = 100;


constexpr int ENERGY_PARAMS_COUNT = 3;
double energy[ENERGY_PARAMS_COUNT];

constexpr int READING_BUFFER_SIZE = 512 * 2;	
volatile int adc_readings_counter;
volatile int readings_buffer[READING_BUFFER_SIZE];
volatile int reading_buffer_index = 0;
hw_timer_t * sampling_timer = NULL;
portMUX_TYPE sampling_timerMux = portMUX_INITIALIZER_UNLOCKED;




/*
 * CALC ALL BAR HEIGHTS
 */
void calc_movement(double* thrs, double* pos, double* vel, double grav, int datalen, double dt, double* diffs) {
    for(int i = 0; i < BANDS_COUNT; i++) {
        pos[i] += dt * vel[i];

        if(pos[i] < 0.0) {
            pos[i] = 0.0;
            vel[i] = 0.0;
        }

        if(pos[i] > 21.0) {
            pos[i] = 21.0;
            vel[i] = 0.0;
        }
    }

    for(int i = 0; i < BANDS_COUNT; i++) {
        thrs[i] = constrain(thrs[i], 0.0, 21.0);


        diffs[i] = thrs[i] - pos[i];
        // idz w gore
        if(diffs[i] >= 0.0) {
            // vel[i] += 32.0 * diff/21.0;
            pos[i] = thrs[i];
            vel[i] = 0;
        }
        else{
            vel[i] += -grav * dt - 0.5;
        }

        // vel[i] += -grav * dt;
    }
}

/*
 * CALC BURST OF BRIGHTNES
 */
constexpr int BURST_BUFFER_LEN = 16;
int burst_buffer[BURST_BUFFER_LEN];
int burst_end_index;
int burst_front_index = BURST_BUFFER_LEN-1;

void calc_energy_eval(int* bar_heights, double* bar_thrs, double* energy){
  int sum_bar_thrs = 0;
  for(int display_x = 0; display_x < 3; display_x++)
    sum_bar_thrs += bar_heights[display_x];

  ++burst_end_index%=BURST_BUFFER_LEN;
  ++burst_front_index%=BURST_BUFFER_LEN;

  energy[energy_t::ENERGY_CURRENT] = double(sum_bar_thrs) / 3;
  burst_buffer[burst_end_index] = *energy;
  energy[energy_t::ENERGY_BURST] = burst_buffer[burst_end_index] - burst_buffer[burst_front_index];
  
  energy[energy_t::ENERGY_MEAN] = 0.0;
  for(int burst_i = 0; burst_i < BURST_BUFFER_LEN; burst_i++)
    energy[energy_t::ENERGY_MEAN] += burst_buffer[burst_i];
  energy[energy_t::ENERGY_MEAN] /= BURST_BUFFER_LEN;

  #if DEBUG_FPS == 1
    Serial.print("TB:"); Serial.print(total_brightness/10.0); Serial.print(", "); 
    Serial.print("Energy:"); Serial.print(energy); Serial.print(", "); 
    Serial.print("Mean_energy:"); Serial.print(mean_energy); Serial.print(", ");
    Serial.print("Burst_energy:");  Serial.println(burst_energy);
  #endif
}

void feedTheDog(){
  // feed dog 0
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
  // feed dog 1
  TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG1.wdt_feed=1;                       // feed dog
  TIMERG1.wdt_wprotect=0;                   // write protect
}









/*
 * CORE 0 INTERRUPT - SAMPLING ADC
 */
void IRAM_ATTR on_timer_sample() {
  portENTER_CRITICAL_ISR(&sampling_timerMux);
  adc_readings_counter++;
  readings_buffer[reading_buffer_index++] = analogRead(A6);
  if(reading_buffer_index >= READING_BUFFER_SIZE)
    reading_buffer_index = 0;
  portEXIT_CRITICAL_ISR(&sampling_timerMux);
}

/*
 * CORE 0 - SETUP, SAMPLE, CALC FFT
 */
void core0_task(void * parameter) {
    Serial.print("FFT sampler/processor running on core ");
    Serial.println(xPortGetCoreID());
    // analyser_begin();
    static long last_time = millis();
    static int loops_counter = 0;
    static double delta_time;
    
    // #interrupt
    sampling_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(sampling_timer, &on_timer_sample, true);
    timerAlarmWrite(sampling_timer, 25, true); //40 kHz
    timerAlarmEnable(sampling_timer);
    // #interrupt

    for(;;) {
        load_samples(reading_buffer_index, readings_buffer, READING_BUFFER_SIZE);
        calculate_bars();
        double* fft_bars = get_magnitudes();
        // feed_mean_buffer(fft_bars, BARS_COUNT);
        for(int i = 0; i < BANDS_COUNT; i++)
            mags[i] = fft_bars[i];

        #if DEBUG_FPS == 1
        delta_time = millis() - last_time;
        if(delta_time > 4000) {
            delta_time /= max(loops_counter, 1);
            Serial.print("Core ");
            Serial.print(xPortGetCoreID());
            Serial.print(" FPS=");
            Serial.print(int(1000.0/delta_time));
            Serial.print("Hz, buffidx=");
            Serial.print(reading_buffer_index);
            Serial.print(", sr=");
            Serial.print(int(adc_readings_counter/4.0));
            Serial.println("Hz");
            Serial.println();
            last_time = millis();
            adc_readings_counter = 0;
            loops_counter = 0;
        }
        #endif

        // vTaskDelay(pdMS_TO_TICKS(1));
        feedTheDog();
        loops_counter++;
    }
}

// void display_effect(const int* bar_heights, const double* bar_thrs, double energy, double burst_energy, double mean_energy, effect_handler_t effect_function) {
//   effect_function(bar_heights, bar_thrs, energy, burst_energy, mean_energy, display_leds, base_leds);
// }

static inline void display_show(){
  FastLED.show();
}

/*
 * CORE 1 - SETUP
 */
void FFTHSV_begin(){
    Serial.println(F("FFT Spectrum Display"));
    FastLED.addLeds<LED_TYPE, DISPLAY_PIN, COLOR_ORDER>(display_leds, DISPLAY_LEDS_COUNT).setCorrection(TypicalSMD5050);
    FastLED.addLeds<LED_TYPE, BASE_PIN, COLOR_ORDER>(base_leds, BASE_LEDS_COUNT).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(MIN_BRIGHTNESS);
    display_show();
    // set_effect(queue[effect_index]);

    Serial.println(xPortGetCoreID());

    xTaskCreatePinnedToCore(core0_task, "FFT_processor", 64000, NULL, 1, NULL,  0);
    last_time_core1 = millis();
    last_time_draw = millis();
    // next_effect_time = millis();
}


/*
 * CORE 1 - CALC EFFECTS, DRAW USING LEDS
 */
void FFTHSV_update(){

    // if(millis() - next_effect_time > EFFECTS_SHUFFLE_INTERVAL) {
    //     ++effect_index %= EFFECTS_COUNT;
    //     set_effect(queue[effect_index]);
    //     next_effect_time += EFFECTS_SHUFFLE_INTERVAL;
    // }

    if(millis() - last_time_update > UPDATE_INTERVAL_MS) {
        dt = (millis() - last_time_update)/1000.0; //sekundy

        for(int i = 0; i < BANDS_COUNT; i++)
            bars_threshold[i] = mags[i];

        // eval_buffer_mean(bars_threshold, BARS_COUNT);
        calc_movement(bars_threshold, bars_pos, velocities, gravity, BANDS_COUNT, dt, diffs);

        for(int i = 0; i < BANDS_COUNT; i++)
            bars[i] = constrain(int(bars_pos[i]), 0, 21);


        calc_energy_eval(bars, bars_threshold, energy);
        effects_options[current_effect_index](bars, bars_threshold, energy, display_leds, base_leds);

        last_time_update += UPDATE_INTERVAL_MS;
    }

    // DRAW
    if(millis() - last_time_draw > DRAW_INTERVAL_MS) {
        display_show();
        last_time_draw += DRAW_INTERVAL_MS;
    }
}

void FFTHSV_set_effects(const effect_handler_t* effects, const int count) {
    for(int i = 0; i<count; i++){
        effects_options[effects_count++] = effects[i];
        if(effects_count>=EFFECTS_MAX_COUNT)
            return;
    }
}

void FFTHSV_select_effect(int index) {
    current_effect_index = index;
}