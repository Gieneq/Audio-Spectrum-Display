#include <Arduino.h>
#include "display.h"
#include "analyser.h"
#include "cmath"
// #include "soc/rtc_wdt.h"
// #include "esp_task_wdt.h" // esp_task_wdt_reset();
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define EFFECTS_COUNT 6
effect_t queue[] = {effect_t::FIRE, effect_t::PULSE, effect_t::CLASSIC, effect_t::COLOROFON, effect_t::ORIGINAL, effect_t::COLOROFON_EXTENDED};

int effect_index;

#define EFFECTS_SHUFFLE_INTERVAL 30000
long next_effect_time;

//zrob detekcje energi - zmiana koloru - szybka ostra muzyka czerwien, ladgodna niebieski

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

#define BAR_HEIGHT 21
#define BARS_COUNT 19

volatile double mags[BARS_COUNT];


long last_time_1;
int loops_counter_1;

constexpr int READING_BUFFER_SIZE = 512 * 2;	
volatile int adc_readings_counter;
volatile int readings_buffer[READING_BUFFER_SIZE];
volatile int reading_buffer_index = 0;
hw_timer_t * sampling_timer = NULL;
portMUX_TYPE sampling_timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR on_timer_sample() {
  portENTER_CRITICAL_ISR(&sampling_timerMux);
  adc_readings_counter++;
  readings_buffer[reading_buffer_index++] = analogRead(A6);
  if(reading_buffer_index >= READING_BUFFER_SIZE)
    reading_buffer_index = 0;
  portEXIT_CRITICAL_ISR(&sampling_timerMux);
}


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
        for(int i = 0; i < BARS_COUNT; i++)
            mags[i] = fft_bars[i];


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

        // vTaskDelay(pdMS_TO_TICKS(1));
        feedTheDog();
        loops_counter++;
    }
}

long last_time_core1;
int loops_counter_core1;
double delta_time_core1;

#define DRAW_INTERVAL_MS 33
long last_time_draw;
int loops_counter_draw;

#define UPDATE_INTERVAL_MS 16
long last_time_update;
int loops_counter_update;



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


double bars_threshold[BARS_COUNT];
double bars_pos[BARS_COUNT];
double diffs[BARS_COUNT];

double dt;
double velocities[BARS_COUNT];
double gravity = 100;

void calc_movement(double* thrs, double* pos, double* vel, double grav, int datalen, double dt, double* diffs) {


    for(int i = 0; i < BARS_COUNT; i++) {
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

    for(int i = 0; i < BARS_COUNT; i++) {
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

int bars[BARS_COUNT]; //rendering

void loop() {
    // UPDATE

    if(millis() - next_effect_time > EFFECTS_SHUFFLE_INTERVAL) {
        ++effect_index %= EFFECTS_COUNT;
        set_effect(queue[effect_index]);
        next_effect_time += EFFECTS_SHUFFLE_INTERVAL;
    }

    if(millis() - last_time_update > UPDATE_INTERVAL_MS) {
    // Serial.print("Update: ");
    // Serial.println(millis() - last_time_update - UPDATE_INTERVAL_MS);
        dt = (millis() - last_time_update)/1000.0; //sekundy

        for(int i = 0; i < BARS_COUNT; i++)
            bars_threshold[i] = mags[i];

        // eval_buffer_mean(bars_threshold, BARS_COUNT);
        calc_movement(bars_threshold, bars_pos, velocities, gravity, BARS_COUNT, dt, diffs);

        for(int i = 0; i < BARS_COUNT; i++)
            bars[i] = constrain(int(bars_pos[i]), 0, 21);

        draw_bars(bars, bars_threshold);
        last_time_update += UPDATE_INTERVAL_MS;
    }

    // DRAW
    if(millis() - last_time_draw > DRAW_INTERVAL_MS) {
        // Serial.print("Render: ");
        // Serial.println(millis() - last_time_draw - DRAW_INTERVAL_MS);
        display_show();
        last_time_draw += DRAW_INTERVAL_MS;
    }

    // delta_time_core1 = millis() - last_time_core1;
    // if(delta_time_core1 > 6000) {
    //     delta_time_core1 /= max(loops_counter_core1, 1);
    //     Serial.print("Core ");
    //     Serial.print(xPortGetCoreID());
    //     Serial.print(" FPS=");
    //     Serial.print(int(1000.0/delta_time_core1));
    //     Serial.print("Hz, buffidx=");
    //     Serial.print(reading_buffer_index);
    //     Serial.println(". ");
    //     // print_bars();
    //     // Serial.println();
    //     last_time_core1 = millis();
    //     loops_counter_core1 = 0;
    // }    
    // loops_counter_core1++;
}
















    // last_time_ease = micros();
    // last_time_1 = millis();
    // last_time_evalfft = micros();


// portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED; 
// TaskHandle_t complexHandlerTask;
// hw_timer_t * adcTimer = NULL; // our timer

// void complexHandler(void *param) {
// }


// void eval_fft() {
//     if(micros() - last_time_evalfft > EVALFFT_INTERVAL_US) {
//         int* fft_bars = calculate_bars();
//         for(int i = 0; i < BARS_COUNT; i++)
//             target_bars[i] = fft_bars[i];
//         last_time_evalfft -= EVALFFT_INTERVAL_US;
//     }
// }

// int ease_bars() {
//     if(micros() - last_time_ease > EASE_INTERVAL_US){
//         int done = 1;
//         while(micros() - last_time_ease > EASE_INTERVAL_US) {
//             for(int i = 0; i < BARS_COUNT; i++) {
//                 int diff = target_bars[i] - bars[i];
//                 if(diff > 0)
//                     bars[i]++;
//                 else if (diff < 0)
//                     bars[i]--;
//                 if (diff != 0)
//                     done = 0;
//             }
//             last_time_ease += EASE_INTERVAL_US;
//         }
//         return done;
//     }
//     return 2;
// }

// void loop() {
//     // if (ease_bars() == 1) {
//         sample_blocking();
//         calculate_bars();
//         int* fft_bars = get_heights();
//         for(int i = 0; i < BARS_COUNT; i++)
//             bars[i] = fft_bars[i];

        
//     draw_bars(bars);
//     display_show();
//     // }

//     // count_fps();
// }















// void complexHandler(void *param) {
//   while (true) {
//     // Sleep until the ISR gives us something to do, or for 1 second
//     uint32_t tcount = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));  
//     if (check_for_work) {
//       // Do something complex and CPU-intensive
//     }
//   }
// }

// void IRAM_ATTR onTimer() {
//   // A mutex protects the handler from reentry (which shouldn't happen, but just in case)
//   portENTER_CRITICAL_ISR(&timerMux);

//   // Do something, e.g. read a pin.
  
//   if (some_condition) { 
//     // Notify complexHandlerTask that the buffer is full.
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     vTaskNotifyGiveFromISR(complexHandlerTask, &xHigherPriorityTaskWoken);
//     if (xHigherPriorityTaskWoken) {
//       portYIELD_FROM_ISR();
//     }
//   }
//   portEXIT_CRITICAL_ISR(&timerMux);
// }

// void setup() {
//   xTaskCreate(complexHandler, "Handler Task", 8192, NULL, 1, &complexHandlerTask);
//   adcTimer = timerBegin(3, 80, true); // 80 MHz / 80 = 1 MHz hardware clock for easy figuring
//   timerAttachInterrupt(adcTimer, &onTimer, true); // Attaches the handler function to the timer 
//   timerAlarmWrite(adcTimer, 45, true); // Interrupts when counter == 45, i.e. 22.222 times a second
//   timerAlarmEnable(adcTimer);
// }






    // // for (int efreq = 0; efreq < 100; efreq++) {
    // //     double freq = 20 * pow(1.0715, efreq);
    // //     feedTone(freq);
    // //     int* bars = calculate_bars();
    // //     draw_bars(bars);
    // //     display_show();
    // //     delay(50);
    // // }


    
// void print_bars() {
//     Serial.print("B: |");
//     for(int i=0; i<BARS_COUNT; i++) {
//         Serial.print(bars[i]);
//         Serial.print(" | ");
//     }
//     Serial.println();
// }

// constexpr int MEAN_BUFFER_LEN = 1;
// constexpr int MEAN_BUFFER_TOTAL_LEN = MEAN_BUFFER_LEN * BARS_COUNT;
// double mean_buffer[MEAN_BUFFER_TOTAL_LEN];
// int free_index = 0;

// void feed_mean_buffer(double* samples_batch, int datalen){
//     for(int i=0; i<datalen; i++) {
//         mean_buffer[i + free_index * datalen] = samples_batch[i];
//     }

//     free_index++;
//     free_index %= MEAN_BUFFER_LEN;
// }

// void eval_buffer_mean(double* dst_batch, int datalen) {
//     for(int i=0; i<datalen; i++) {
//         dst_batch[i] = 0.0;
//         for(int j=0; j<MEAN_BUFFER_LEN; j++){
//             dst_batch[i] += mean_buffer[i + j * datalen];
//         }
//         dst_batch[i] /= MEAN_BUFFER_LEN;
//         Serial.println(dst_batch[i]);
//     }
// }

