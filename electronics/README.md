# Electronics

The electronic part consists of:

* switching input,
* analog filter-amplifier,
* 5 V / 22 A power supply,
* ESP32,
* 399 WS2812B LED display (single zig-zag strip),
* shorter 14 LED strip in base

Schematic of main board:

<p align="center">
  <img width="500" alt="Main Board Schematic FFT Audio Specrum Display ESP32" src="Main_board_schematic.png">
</p>

## Aplifier

The amplifier built around [TDA2822](TDA2822.pdf) is used to amplify signal from AUX or [MAX9814 microphone](MAX9814.pdf). Analg circuit is designed to prepare signals so that bouth sorces can be used. Switching sources is done by unplugging 3.5 mm jack.

Check Instructables for more details about designing. Simulation of this circuit is in [Notebook](SDA_3_analog_simulation.ipynb).

<p align="center">
  <img width="500" alt="Simulation of analog filter using Notebook and Python complex math" src="notebook_simulation_filter.png">
</p>

As seen in diagram inside considered frequency range amplitude is nearly constant. The voltage level can be changed by trimming the potentiometer - visible change in blue and red plots.