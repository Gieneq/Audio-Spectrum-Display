#include <Arduino.h>
#include "presets.h"
#include "tools.h"
#include "display.h"
#include "analyser.h"
#include "cmath"

/*
 * VARIABLES
 */
effect_t queue[] = {effect_t::FIRE, effect_t::PULSE, effect_t::CLASSIC, effect_t::COLOROFON, effect_t::ORIGINAL, effect_t::COLOROFON_EXTENDED};
int effect_index;

long next_effect_time;

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

int bars[BANDS_COUNT]; //rendering

double energy;
double burst_energy;
double mean_energy;

constexpr int READING_BUFFER_SIZE = 512 * 2;	
volatile int adc_readings_counter;
volatile int readings_buffer[READING_BUFFER_SIZE];
volatile int reading_buffer_index = 0;
hw_timer_t * sampling_timer = NULL;
portMUX_TYPE sampling_timerMux = portMUX_INITIALIZER_UNLOCKED;


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
void core0_task( void * parameter) {
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


/*
 * CORE 1 - SETUP
 */
void setup() {
    Serial.begin(115200);
    Serial.println(F("FFT Spectrum Display"));
    display_begin();
    set_effect(queue[effect_index]);

    Serial.println(xPortGetCoreID());

    xTaskCreatePinnedToCore(core0_task, "FFT_processor", 64000, NULL, 1, NULL,  0);
    last_time_core1 = millis();
    last_time_draw = millis();
    next_effect_time = millis();
}


/*
 * CORE 1 - CALC EFFECTS, DRAW USING LEDS
 */
void loop() {
    // UPDATE

    if(millis() - next_effect_time > EFFECTS_SHUFFLE_INTERVAL) {
        ++effect_index %= EFFECTS_COUNT;
        set_effect(queue[effect_index]);
        next_effect_time += EFFECTS_SHUFFLE_INTERVAL;
    }

    if(millis() - last_time_update > UPDATE_INTERVAL_MS) {
        dt = (millis() - last_time_update)/1000.0; //sekundy

        for(int i = 0; i < BANDS_COUNT; i++)
            bars_threshold[i] = mags[i];

        // eval_buffer_mean(bars_threshold, BARS_COUNT);
        calc_movement(bars_threshold, bars_pos, velocities, gravity, BANDS_COUNT, dt, diffs);

        for(int i = 0; i < BANDS_COUNT; i++)
            bars[i] = constrain(int(bars_pos[i]), 0, 21);


        calc_energy_eval(bars, bars_threshold, &energy, &burst_energy, &mean_energy);
        display_update(bars, bars_threshold, energy, burst_energy, mean_energy);
        last_time_update += UPDATE_INTERVAL_MS;
    }

    // DRAW
    if(millis() - last_time_draw > DRAW_INTERVAL_MS) {
        display_show();
        last_time_draw += DRAW_INTERVAL_MS;
    }
}
