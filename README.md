# Audio-Spectrum-Display 🎧

![Audio Spectrum Display using ESP32, FFT and WS2812B FastLED strip](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/mini.jpg)

19 bands (21 height) audio spectrum display built around ESP32 chip. It process audio signal from 3.5 mm socket or microphone using FFT and drives matrix according to resulting magnitudes - each column of display represent specific frequency content in signal. The higher the amplitude of specific frequency, the higher magnitude.

## Casing

![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_6.jpg)

Casing is magde out of laser cut acrylic. 399 LED block are milled, so tak light can better disperse.

![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_1.jpg)

## Matrix (WS2812B strip)

The matrix (or programmable LED strip) is made using individual WS2812B LEDs soldered to custom PCBs. Custom PCBs grants good heat distribution (entire display can draw more than 100W of power) and equal spacing. There exists programmable LEDs strips with slightly higher spacing, which can be less time-consuming solution.

![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_2.jpg)
![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_3.jpg)

## Electronics

As power source 5V/22A power supply was choosen. ESP32 devmodule is main CPU of this project. So far its ASDC is used to sample signal. It is not best option becouse its max frequency reaches 5ksps - optimal sampling frequency is 40ksps becouse of Nyquist–Shannon sampling theorem.

![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_power_supply_4.jpg)

Both cores of ESp32 are used, so (so far) there is no WiFi connection. Tasks distribution:
* core 0 is responsible for samping and FFT evaluation,
* core 1 is responsible for animation and driving LEDs. 

Sampling is driven by timer to achieve stable sampling period.

![alt text](https://github.com/Gieneq/Audio-Spectrum-Display/blob/master/img/audio_spectrum_display_acrylic_fft_fastled_esp32_programming.PNG)

399 are driven synchronously so based on baud rate, driving all of them takes around 12 ms, so top frequency is 83 Hz (30 is enough).

Preamplifier is built around TDA2822 circuit with 40dB amplification.

## Effects 🔥

Code is split to severa files, where effect.h and effect.cpp are used to create custom effects. Fire effect's function prototype:
```
void fire_effect(const int* bar_heights, const double* bar_thrs, double* energy, CRGB* display_leds, CRGB* base_leds);
```
All those parameters are "tools" used to achieve dynamic effects.

THen handler should be made and placed in function pointers array like this:
```
effect_handler_t fire_effect_handlers[] = {fire_effect};

void setup() {
    Serial.begin(115200);
    FFTHSV_begin();
    FFTHSV_set_effects(fire_effect_handlers, 1);
    FFTHSV_select_effect(0);
    FFTHSV_benchmark_params(10,20000,200,10000);
    FFTHSV_benchmark(false);
}
```
Check my YT channel where I upload updates. If you are interested in this project consider subscribing ✋

https://www.youtube.com/watch?v=PZK8QYlz3o4

## Do it yourself

I was inspired with projects available online. I've made drawings in Corel Draw and have written my custom code based on [ArduinoFFT library](https://github.com/kosme/arduinoFFT). I haven't seen any full code for more advanced processing, so you are free to use it. I'll develop it to make it more robust.

## Plans

The most important thing is to use better ADC converter to achieve wider bandwidth - displays looks like it works properrly but it is not. I'm really considering using ADS1256 IC beuse it has great sampling frequency, built in amplifier even up to 64 gain, filter and resulting effective resolution of 17 bit (see datasheet).

To do:
- [x] Order ADS1256 ADC,
- [ ] Test ADS1256,
- [ ] BPM detector,
- [ ] Hardware FFT with FPGA (or separate MCU),
- [ ] WiFi and Home Assistant Integration,
- [ ] Driving Board PCB,
- [ ] Better preamplifier circuit.

