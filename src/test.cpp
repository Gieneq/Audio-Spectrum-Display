// // LED Audio Spectrum Analyzer Display
// //
// // Creates an impressive LED light show to music input
// //   using Teensy 3.1 with the OctoWS2811 adaptor board
// //   http://www.pjrc.com/store/teensy31.html
// //   http://www.pjrc.com/store/octo28_adaptor.html
// //
// // Line Level Audio Input connects to analog pin A3
// //   Recommended input circuit:
// //   http://www.pjrc.com/teensy/gui/?info=AudioInputAnalog
// //
// // This example code is in the public domain.

// #define USE_OCTOWS2811
// //#include <OctoWS2811.h>
// #include <FastLED.h>
// #include <Audio.h>
// //#include <Wire.h>
// //#include <SD.h>
// //#include <SPI.h>

// // The display size and color to use
// const unsigned int matrix_width = 60;
// const unsigned int matrix_height = 32;
// const unsigned int myColor = 0x400020;

// // These parameters adjust the vertical thresholds
// const float maxLevel = 0.5;      // 1.0 = max, lower is more "sensitive"
// const float dynamicRange = 40.0; // total range to display, in decibels
// const float linearBlend = 0.3;   // useful range is 0 to 0.7

// CRGB leds[matrix_width * matrix_height];

// // Audio library objects
// AudioInputAnalog         adc1(A3);       //xy=99,55
// AudioAnalyzeFFT1024      fft;            //xy=265,75
// AudioConnection          patchCord1(adc1, fft);


// // This array holds the volume level (0 to 1.0) for each
// // vertical pixel to turn on.  Computed in setup() using
// // the 3 parameters above.
// float thresholdVertical[matrix_height];

// // This array specifies how many of the FFT frequency bin
// // to use for each horizontal pixel.  Because humans hear
// // in octaves and FFT bins are linear, the low frequencies
// // use a small number of bins, higher frequencies use more.
// int frequencyBinsHorizontal[matrix_width] = {
//    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
//    2,  2,  2,  2,  2,  2,  2,  2,  2,  3,
//    3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
//    5,  5,  6,  6,  6,  7,  7,  7,  8,  8,
//    9,  9, 10, 10, 11, 12, 12, 13, 14, 15,
//   15, 16, 17, 18, 19, 20, 22, 23, 24, 25
// };



// // Run setup once
// void setup() {
//   // the audio library needs to be given memory to start working
//   AudioMemory(12);

//   // compute the vertical thresholds before starting
//   computeVerticalLevels();

//   // turn on the display
//   FastLED.addLeds<OCTOWS2811>(leds,(matrix_width * matrix_height) / 8);
// }

// // A simple xy() function to turn display matrix coordinates
// // into the index numbers OctoWS2811 requires.  If your LEDs
// // are arranged differently, edit this code...
// unsigned int xy(unsigned int x, unsigned int y) {
//   if ((y & 1) == 0) {
//     // even numbered rows (0, 2, 4...) are left to right
//     return y * matrix_width + x;
//   } else {
//     // odd numbered rows (1, 3, 5...) are right to left
//     return y * matrix_width + matrix_width - 1 - x;
//   }
// }

// // Run repetitively
// void loop() {
//   unsigned int x, y, freqBin;
//   float level;

//   if (fft.available()) {
//     // freqBin counts which FFT frequency data has been used,
//     // starting at low frequency
//     freqBin = 0;

//     for (x=0; x < matrix_width; x++) {
//       // get the volume for each horizontal pixel position
//       level = fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);

//       // uncomment to see the spectrum in Arduino's Serial Monitor
//       // Serial.print(level);
//       // Serial.print("  ");

//       for (y=0; y < matrix_height; y++) {
//         // for each vertical pixel, check if above the threshold
//         // and turn the LED on or off
//         if (level >= thresholdVertical[y]) {
//           leds[xy(x,y)] = CRGB(myColor);
//         } else {
//           leds[xy(x,y)] = CRGB::Black;
//         }
//       }
//       // increment the frequency bin count, so we display
//       // low to higher frequency from left to right
//       freqBin = freqBin + frequencyBinsHorizontal[x];
//     }
//     // after all pixels set, show them all at the same instant
//     FastLED.show();
//     // Serial.println();
//   }
// }


// // Run once from setup, the compute the vertical levels
// void computeVerticalLevels() {
//   unsigned int y;
//   float n, logLevel, linearLevel;

//   for (y=0; y < matrix_height; y++) {
//     n = (float)y / (float)(matrix_height - 1);
//     logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
//     linearLevel = 1.0 - n;
//     linearLevel = linearLevel * linearBlend;
//     logLevel = logLevel * (1.0 - linearBlend);
//     thresholdVertical[y] = (logLevel + linearLevel) * maxLevel;
//   }
// }



// #define SCL_INDEX 0x00
// #define SCL_TIME 0x01
// #define SCL_FREQUENCY 0x02
// #define SCL_PLOT 0x03


// void loop()
// {
//   /*SAMPLING*/
//   microseconds = micros();
//   for(int i=0; i<samples; i++)
//   {
//       vReal[i] = analogRead(CHANNEL);
//       vImag[i] = 0;
//       while(micros() - microseconds < sampling_period_us){
//         //empty loop
//       }
//       microseconds += sampling_period_us;
//   }
//   /* Print the results of the sampling according to time */
//   long st = micros();
// //  Serial.println("Data:");
// //  PrintVector(vReal, samples, SCL_TIME);
//   FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
// //  Serial.println("Weighed data:");
// //  PrintVector(vReal, samples, SCL_TIME);
//   FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
// //  Serial.println("Computed Real values:");
// //  PrintVector(vReal, samples, SCL_INDEX);
// //  Serial.println("Computed Imaginary values:");
// //  PrintVector(vImag, samples, SCL_INDEX);///////////
//   FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
// //  Serial.println("Computed magnitudes:");
// //  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
// //  double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
// //  Serial.println(x, 6); //Print out what frequency is the most dominant.
// //  Serial.println(); //Print out what frequency is the most dominant.
// //  Serial.println(); //Print out what frequency is the most dominant.
// //  while(1); /* Run Once */
//   // delay(2000); /* Repeat after delay */
//   st = micros() - st;
//   Serial.println(st);
// }

// // void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
// // {
// //   for (uint16_t i = 0; i < bufferSize; i++)
// //   {
// //     double abscissa;
// //     /* Print abscissa value */
// //     switch (scaleType)
// //     {
// //       case SCL_INDEX:
// //         abscissa = (i * 1.0);
// // 	break;
// //       case SCL_TIME:
// //         abscissa = ((i * 1.0) / samplingFrequency);
// // 	break;
// //       case SCL_FREQUENCY:
// //         abscissa = ((i * 1.0 * samplingFrequency) / samples);
// // 	break;
// //     }
// //     Serial.print(abscissa, 6);
// //     if(scaleType==SCL_FREQUENCY)
// //       Serial.print("Hz");
// //     Serial.print(" ");
// //     Serial.println(vData[i], 4);
// //   }
// //   Serial.println();
// // }




#include <Arduino.h>
// #include <FastLED.h>
// #include "arduinoFFT.h"

// constexpr int AUDIO_PIN = A0;

// arduinoFFT FFT = arduinoFFT();

// const uint16_t samples = 1024;
// const double samplingFrequency = 40000;

// unsigned int sampling_period_us;
// unsigned long microseconds;

// double vReal[samples];
// double vImag[samples];

// void sample();

// void animate();

// void setup() {
//     sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
//     Serial.begin(115200);
//     Serial.println("FFT Spectrum Display");
// }

// void loop() {
//     sample();
//     animate();
// }

// void sample() {
//     for (int i = 0; i < samples; i++) {
//         vReal[i] = analogRead(AUDIO_PIN);
//         vImag[i] = 0;
//         while (micros() - microseconds < sampling_period_us) {
//         }
//         microseconds += sampling_period_us;
//     }
// }

// void animate() {
//     FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);

//     FFT.Compute(vReal, vImag, samples, FFT_FORWARD);

//     FFT.ComplexToMagnitude(vReal, vImag, samples);
// }
