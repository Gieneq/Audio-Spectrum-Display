#include "analyser.h"
#include "math.h"

constexpr double NOISE_LIMIT = 512.0 * 4;
constexpr double BAR_HEIGHT_MODIFIER = BAR_HEIGHT/(1024.0*128.0); //(COLS_HEIGHT)/1512.0/128.0;
constexpr unsigned long SAMPLING_PERIOD_US = static_cast<unsigned long>(1000000.0 / SAMPLING_FREQUENCY);
constexpr uint16_t USABLE_SAMPLES_COUNT = SAMPLES_COUNT/2 - 1;
unsigned long sampling_time_us;
int sampling_counter;

double vReal[SAMPLES_COUNT];
double vImag[SAMPLES_COUNT];

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES_COUNT, SAMPLING_FREQUENCY);

double magnitude_bands[BANDS_COUNT];

extern const int bands512[];

void analyser_begin() {
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        vReal[i] = 0.0;
        vImag[i] = 0.0;
    }
    for (int i = 0; i < BANDS_COUNT; i++)
        magnitude_bands[i] = 0.0;
}

void sample_blocking(){
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        vReal[i] = analogRead(AUDIO_PIN);
        vImag[i] = 0;
        delayMicroseconds(SAMPLING_PERIOD_US);
    }
}

void load_samples(int read_index, volatile int* read_buffer, const int buffersize) {
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        vReal[i] = read_buffer[read_index--]; //DC removal  - 1800
        vImag[i] = 0;
        if(read_index < 0)
            read_index = buffersize-1;
    }
}

void calculate_bars() {
    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    FFT.Compute(FFT_FORWARD);

    FFT.ComplexToMagnitude();  // wynik w vReal
  
    for (int i = 0; i < BANDS_COUNT; i++)
      magnitude_bands[i] = 0.0;

    for (int i = 1; i < USABLE_SAMPLES_COUNT; i++) {
        if (vReal[i] > NOISE_LIMIT)
            magnitude_bands[bands512[i]] += vReal[i];
    }
    for (int i = 0; i < BANDS_COUNT; i++)
      magnitude_bands[i] *= BAR_HEIGHT_MODIFIER;
}

double* get_magnitudes() {
    return magnitude_bands;
}
