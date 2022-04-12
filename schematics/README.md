# Electronics

|BLock schematic of Audio Spectrum Display V1.0 |
|:-:|
|<img src="spectrum_display_HSVFFT_V1_vis.png" alt="" width="500">|

In this article I willtry to explain how to choose values of passive components to process signal before sampling by ADC.

# Circuit theory basics

Before talking about real circuit several words about circuit theory must be written.

When designing blocks of analog filters, several varibles must be considered:
- output impedance of filter,
- input impedance of filter,
- cutoff frequency,
- phase shift.

Matching output and input impedance is crucial to obtain usefull signal – if:
- following impedance is close to preciding one then output voltage will be scaled to half of input voltage,
- following impedance is 10x greather than preciding impedance, then output voltage will be close to input voltage.

## Real voltage source

Let's consider basic circuit made out of ideal voltage source 1V DC with series resistance R2 (real voltage source) loaded with R1 resistor:

|Simple real voltage source with resistive load|
|:-:|
|<img src="basic_circuit_thevenins_theorem_1.png" alt="" width="500">|

Output voltage is described by voltage divider, so gain (ratio of output and input voltages) is given by formula:

<img height="30" src="https://render.githubusercontent.com/render/math?math=k = \frac{U_{out}}{U_{in}} = \frac{R_1}{R_1 + R_2} [\frac{V}{V}]" >

Let's see how gain changes with R1:

|R2|R1|R1/R2|k|
|:-:|:-:|:-:|:-:|
|100R|20R|0.2x|0.17|
|100R|100R|1x|0.5|
|100R|1k|10x|0.91|
|100R|10k|100x|0.99|

The higher R2 the higher output voltage, but it cannot be cranced up freely – output impedance must be considered.

## Thevenin's Theorem

Thevenin's theorem says that any linear circuit can be reaplaced by voltage source and resistance (impedance in generall) that behaves in the same way. Equivalent circuit canbe modelledusing open circuit voltage and short circuit current.

Open circuit voltage can be obtained from voltage divider:

<img height="30" src="https://render.githubusercontent.com/render/math?math=U_{th} = U_{in}\frac{R_1}{R_1 + R_2} [V]" >

Then short circuit current is given by formula:

<img height="30" src="https://render.githubusercontent.com/render/math?math=I_{th} = \frac{U_{in}}{R_2} [A]" >

Thevenin's equivalent resistance is:

<img height="30" src="https://render.githubusercontent.com/render/math?math=R_{th} = \frac{U_{th}}{I_{th}} = \frac{R_1 \cdot R_2}{R_1 + R_2} [\Omega]" >

Worth mentioning: **voltage gain of this system is not Thevenin's resistance!**

Equivalent circuit is made using:
- new voltage source with Thevenin's voltage value,
- resitorwith Thevenin's resistance value.

|Equivalent circuit|
|:-:|
|<img src="basic_circuit_thevenins_theorem_2.png" alt="" width="500">|

To ensure if both circuits are behaving the same way, let's look at output voltage. Thevenin'ss circuit's output voltage is passed from voltage source witout any loss on resistor, becouse of no current flow. So output voltage is simply:

<img height="30" src="https://render.githubusercontent.com/render/math?math=U_{out} = U_{th} = U_{in}\frac{R_1}{R_1 + R_2} [V]" >

Let's consider how Thevenin's resistance (called output resistance) vary with R1 and how much current circuit can deliver during short circuit:

|R1|Rth|Uth|Iss|
|:-:|:-:|:-:|:-:|
|20R|16.7R|0.17V|10mA|
|100R|50R|0.5V|10mA|
|1k|90R|0.91V|10mA|
|10k|99R|0.99V|10mA|

As R2 rises, Thevenin's resistance rises to limit given by R2 (R1 is in parralel to R2 so total resistance will never exceed lower of those 2) and output voltage also rises sustaining constant short circuit current value.

# Signal preparation

Output signal from microphone can by switched by plugging 3.5mm audio jack to 5 pin socket with mechanical switch. So both signals from microphone and audio jack should be similiar, but only microphone signal can be modified due to plug switching.

|Parameter|Microphone MAX9814|Audio Jack|
|:-|:-:|:-:|
|DC offset|1.2V|Symetrical 0V|
|Amplitude|0.76V|1.11V|
|Gain|Constant 40/50/60dB|Dpendant on source|

Becouse of that, signal from microphone should be high pass filtered to remove DC component. At this stage builtin amplifier is set to maximum 60dB not reaching top ampllitude of audio jack. It is not crucial, becouse then commom preamplifier will be added.

|Audio Jack max volume 1kHz | Microphone the same audio source, headphones 1cm from device|
|:-------------------------:|:-------------------------:|
|![](jack_1khz.BMP)  |  ![](micro_1khz.BMP)|

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

## MAX9814 microphone

Links:
- [Analog Devices IC - MAX9814 datasheet](https://datasheets.maximintegrated.com/en/ds/MAX9814.pdf)
- [Adafruit Module: schematic, EAGLE files](https://github.com/adafruit/Adafruit-MAX9814-AGC-Microphone-PCB)

Crucial parameters:
- Supply Voltage: 2.7V to 5.5V 
- Zout: 50R
- Min resistive load: 5k
- Max capacitive load: 200pF
- Max output current 2mA

## High pass microphone signal

High pass RC filter will be used with cutoff frequency (maximal damped frequency) od 20Hz which is angular frequency of 125 rad/s. 20kHz max frequency is considered which coresponds to 125664 rad/s.

Resistance should be minimum 10x grather than outputimpedance of signal source to not overload source and lower voltage. Recomended resistance is 5k.
```python
w0 = 125
R0 = 5000
C0 = 1/(R0*w0)
print(C0)
```
Resulting capacity is 1.6uF:
>1.6e-06

Now the question is wherher it is a good choice. System looks like this with internal impedance 50R included:

|Schematic of audio source with RC high pass filter|
|:-:|
|<img src="schematic_microphone_high_pass.png" alt="" width="500">|

## Capacitor reactance

Capacitor reactance which is imaginary part of complex capacitor's impedance is given by formula:

<img height="30" src="https://render.githubusercontent.com/render/math?math=X_C = \Im(-j\frac{1}{\omega C}) = \frac{1}{\omega C}" >

Generate angular frequency Numpy array:

```python
def gen_w_array(min_f=20, max_f=20000, steps=100):
  min_w = 2*pi*min_f
  max_w = 2*pi*max_f

  exp_a = min_w
  exp_b = (max_w/exp_a)**(1/(steps-1))

  w = exp_a*exp_b**np.arange(0,steps)
  return w

w = gen_w_array(min_f = 0.1, max_f = 40000, steps = 200)
```
Samples are exponentially distributed:

|Frequencyrange 20Hz (125rad/s) to 20kHz(125664rad/s)|
|:-:|
|<img src="angular_frequency_samples.png" alt="" width="500">|

Capacitance (capacitorreactances) ranges from 10k to 8R:

|Capacitance of 1.6 uF capacitor|
|:-:|
|<img src="capacitance_1u6F.png" alt="" width="500">|

## Output impedance

To have better look on this circuit Thevenin theore can be used. It requires finding open circuit voltage and short circuit current and then dividing them to find out complex impedance.

Voltage source can be described as time dependant voltage:

<img height="30" src="https://render.githubusercontent.com/render/math?math=u(t) = 1.2 + 0.76 \cdot cos(\omega t) [V]" >

or using notation with complex voltage:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{U} = 0.76 [V]" >

DC part will be trimmed, cosine part is lets say frozen so that final phasor in complex plane is rotated by phase shift and scaled by RMS voltage or amplitude (in this case).

Output voltage can be find out using voltage divider:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{U_{th}} = \bar{U}\frac{R_0}{R_0+R_{in}+Z_{C0}} [V]" >

During short circuit R0 is excluded and final current is:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{I_{th}} = \frac{\bar{U}}{R_{in}+Z_{C0}} [A]" >

Final Thevenin resistance is:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{Z_{th}} = \frac{\bar{U_{th}}}{\bar{I_{th}}} = \frac{R_0 (R_{in}+Z_{C0})}{R_0+R_{in}+Z_{C0}} [\Omega]" >

```python
ZC0 = -1j/(w*C0)
U_th = U * R0 /(R0 + Rin + ZC0)
I_th = U / (Rin + ZC0)
Z_th = R0 * (Rin + ZC0) / (R0 + Rin + ZC0)
```
Both voltage and current canbe plotted:

|THevenin voltage in V | Thevenin current in A|
|:-------------------------:|:-------------------------:|
|![](capacitance_1u6F_voltage.png)  |  ![](capacitance_1u6F_voltage_current.png)|

Ratio between voltage and current is impedance and it describes both:
- magnitud: how many times voltage is greater than current
- phase: how current is shifted relative to current.

warto zauwazyc ze modul impedancji jest wieksza niz dla uproszczenia samej kapacytancji

# Audio Jack

Wstep do elektroniki - schemat blokowy i opiskazdego bloku


Coming coon



