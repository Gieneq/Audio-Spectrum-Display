// #include "tools.h"

// /*
//  * CALC ALL BAR HEIGHTS
//  */
// void calc_movement(double* thrs, double* pos, double* vel, double grav, int datalen, double dt, double* diffs) {
//     for(int i = 0; i < BANDS_COUNT; i++) {
//         pos[i] += dt * vel[i];

//         if(pos[i] < 0.0) {
//             pos[i] = 0.0;
//             vel[i] = 0.0;
//         }

//         if(pos[i] > 21.0) {
//             pos[i] = 21.0;
//             vel[i] = 0.0;
//         }
//     }

//     for(int i = 0; i < BANDS_COUNT; i++) {
//         thrs[i] = constrain(thrs[i], 0.0, 21.0);


//         diffs[i] = thrs[i] - pos[i];
//         // idz w gore
//         if(diffs[i] >= 0.0) {
//             // vel[i] += 32.0 * diff/21.0;
//             pos[i] = thrs[i];
//             vel[i] = 0;
//         }
//         else{
//             vel[i] += -grav * dt - 0.5;
//         }

//         // vel[i] += -grav * dt;
//     }
// }

// /*
//  * CALC BURST OF BRIGHTNES
//  */
// constexpr int BURST_BUFFER_LEN = 16;
// int burst_buffer[BURST_BUFFER_LEN];
// int burst_end_index;
// int burst_front_index = BURST_BUFFER_LEN-1;

// void calc_energy_eval(int* bar_heights, double* bar_thrs, double* energy){
//   int sum_bar_thrs = 0;
//   for(int display_x = 0; display_x < 3; display_x++)
//     sum_bar_thrs += bar_heights[display_x];

//   ++burst_end_index%=BURST_BUFFER_LEN;
//   ++burst_front_index%=BURST_BUFFER_LEN;

//   energy[energy_t::ENERGY_CURRENT] = double(sum_bar_thrs) / 3;
//   burst_buffer[burst_end_index] = *energy;
//   energy[energy_t::ENERGY_BURST] = burst_buffer[burst_end_index] - burst_buffer[burst_front_index];
  
//   energy[energy_t::ENERGY_MEAN] = 0.0;
//   for(int burst_i = 0; burst_i < BURST_BUFFER_LEN; burst_i++)
//     energy[energy_t::ENERGY_MEAN] += burst_buffer[burst_i];
//   energy[energy_t::ENERGY_MEAN] /= BURST_BUFFER_LEN;

//   #if DEBUG_FPS == 1
//     Serial.print("TB:"); Serial.print(total_brightness/10.0); Serial.print(", "); 
//     Serial.print("Energy:"); Serial.print(energy); Serial.print(", "); 
//     Serial.print("Mean_energy:"); Serial.print(mean_energy); Serial.print(", ");
//     Serial.print("Burst_energy:");  Serial.println(burst_energy);
//   #endif
// }

// void feedTheDog(){
//   // feed dog 0
//   TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
//   TIMERG0.wdt_feed=1;                       // feed dog
//   TIMERG0.wdt_wprotect=0;                   // write protect
//   // feed dog 1
//   TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
//   TIMERG1.wdt_feed=1;                       // feed dog
//   TIMERG1.wdt_wprotect=0;                   // write protect
// }