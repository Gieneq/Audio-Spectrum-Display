# Electronics

In this article it will be explained how to choose values of passive components to process signal from audio source before being sampled by ADC.

# Circuit theory basics

Before moving to the topic of real circuit, several words about the circuit theory must be written.

When designing blocks of analog filters, several parameters must be considered:
- output impedance of filter,
- input impedance of filter,
- cutoff frequency,
- phase shift.

**Matching output and input impedance** is crucial to obtain useful signal, because if:
- impedance of following block is close to the preceding one, then output voltage will be scaled to half of input voltage,
- impedance of following block is (rule of thumb) 10x greater than preceding impedance, then output voltage will be close to input voltage.

## Real voltage source

Let's consider basic circuit made out of ideal voltage source 1V DC with series resistance R2 (real voltage source) loaded with R1 resistor:

|Simple real voltage source with resistive load|
|:-:|
|<img src="basic_circuit_thevenins_theorem_1.png" alt="Simple real voltage source with resistive load" width="500">|

When considering resistive load rules can be applied more generally to impedance.

Output voltage is described using voltage divider, so gain (ratio of output and input voltages) is given by formula:

<img height="50" src="https://render.githubusercontent.com/render/math?math=k = \frac{U_{out}}{U_{in}} = \frac{R_1}{R_1 \!%2B\! R_2} [\frac{V}{V}]" >

Let's see how gain changes with R1:

|R2|R1|R1/R2|k|
|:-:|:-:|:-:|:-:|
|100R|20R|0.2x|0.17|
|100R|100R|1x|0.5|
|100R|1k|10x|0.91|
|100R|10k|100x|0.99|

The higher R1 the higher gain or output voltage, but it cannot be cranked up freely – output impedance of source and input impedance of receiver must be considered.

## Thevenin's Theorem

Thevenin's theorem says that any linear circuit can be replaced by voltage source and resistance (impedance in general) that behaves in the same way. Equivalent circuit can be modeled using **open circuit voltage** and **short circuit current**.

**Open circuit voltage** can be obtained from voltage divider:

<img height="50" src="https://render.githubusercontent.com/render/math?math=U_{th} = U_{in}\frac{R_1}{R_1 \!%2B\! R_2} [V]" >

Then **short circuit current** is given by formula:

<img height="50" src="https://render.githubusercontent.com/render/math?math=I_{th} = \frac{U_{in}}{R_2} [A]" >

R1 is shorten so Thevenin's current depends only on **series resistance R2**.

The Thevenin's equivalent resistance is:

<img height="50" src="https://render.githubusercontent.com/render/math?math=R_{th} = \frac{U_{th}}{I_{th}} = \frac{R_1 \cdot R_2}{R_{1} \!%2B\! R_{2}} [\Omega]" >

Worth mentioning: **voltage gain of this system is not Thevenin's resistance!**

Equivalent circuit is made using:
- new voltage source with Thevenin's voltage value,
- resistor with Thevenin's resistance value.

|Equivalent circuit|
|:-:|
|<img src="basic_circuit_thevenins_theorem_2.png" alt="" width="500">|

To ensure if both circuits are behaving the same way, let's look at output voltage. Thevenin's circuit's output voltage is passed from voltage source without any loss on resistor, because of no current flow. So output voltage is simply:

<img height="50" src="https://render.githubusercontent.com/render/math?math=U_{out} = U_{th} = U_{in}\frac{R_1}{R_1 \!%2B\! R_2} [V]" >

Let's consider how Thevenin's resistance (called output resistance) vary with R1 and how much current circuit can deliver during short circuit:

|R1|Rth|Uth|Iss| P|
|:-:|:-:|:-:|:-:|:-:|
|20R|16.7R|0.167V|10mA|1.39mW|
|100R|50R|0.5V|10mA|2.5mW|
|1k|91R|0.91V|10mA|0.83mW|
|10k|99R|0.99V|10mA|0.1mW|

As R1 rises, Thevenin's resistance rises to limit given by R2 (R1 is in parallel to R2 so total resistance will never exceed lower of those two) and output voltage also rises sustaining constant short circuit current value. Constant 10mA is consistent with the fact that R1 is shorted, so only R2 counts to the current.

Output resistance varies with R1. Lower output resistance does not mean, that circuit can deliver more power. Despite lower parallel resistance and more current flow, there is lower voltage drop on output. When power deliver to output will be maximised? The table shows power draw by R1 which is evaluated based on power formula:

<img height="50" src="https://render.githubusercontent.com/render/math?math=P = U_{R1} \cdot I_{R1} = \frac{U_{out}^2}{R_1} = \frac{U_{th}^2}{R_1} [W]" >

The most power is transferred when R1 and R2 are equal.

---

Now let's consider equivalent Thevenin's circuit with another resistor R3 = 100R connector to the circuit's output:

|Circuit with R3 added|
|:-:|
|<img src="basic_circuit_thevenins_theorem_3.png" alt="Circuit with R3 added" width="500">|

The goal is to find R1 that maximize power transfer to R3=100R. Because Thevenin's resistance of R1 and R2=100R connection cannot exceed 100R (R2), the most power will be transferred when R1 is close to infinity (open circuit) and thus not influencing parallel connection of R1 and R3. 

So when connecting several blocks, impedance of all blocks should be considered. In general we are limited by 2 impedances:
- source output impedance e.g. 50R,
- receiver impedance e.g. 100k.

|R1|Rth|Uth|IR3|Uout|R13| P3|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|20R|16.7R|0.167V|1.43mA|0.143V|16.7R|0.2mW|
|100R|50R|0.5V|3.33mA|0.33V|50R|1.11mW|
|1k|91R|0.91V|4.76mA|0.47V|91R|2.27mW|
|10k|99R|0.99V|4.97mA|0.497V|99R|2.47mW|

In most cases receivers are buffer circuits made out of op-amps with high input impedance and low output impedance. So to sustain high quality signal all processing blocks should be spread across minimum and maximum impedances.

When audio signal passes to ADC it is required to know how ADC processes signal and thus find out required current draw.

---

Now let's pick R2 = 100R, R1 = 1k and connect several R3 loads. As before Rth = 91R and Uth = 0.91V.

|R1|R3|R13|IR3|Uout|P3|
|:-:|:-:|:-:|:-:|:-:|:-:|
|1k|10|9.91R|9.01mA|0.09V|0.81mW|
|1k|100|90.91R|4.76mA|0.48V|2.28mW|
|1k|1k|500R|0.83mA|0.83V|0.69mW|
|1k|10k|909R|0.09mA|0.9V|0.08mW|

Again, when connecting resistors in parallel (in this case R1 and R3) final resistance will be smaller than the smaller value of those two. Knowing that, it is not recommended to use R3 lower than R1, because R3 becomes lower of those two, and lowers significantly overall parallel resistance.

As pointed before when R3 = Rth power reaches maximum. When R3 rises much higher than output resistance of source (Rth), voltage drop at the output reaches voltage of source (Uth). So again by the rule of thumb 10 times greater resistance for R3 should be choosen. Then overall current draw will be much less (0.99mA) and output voltage is closer to input voltage (0.9V).

---
When looking at real voltage source with series impedance and several parallel impedances, the output impedance (or equivalent Thevenin's resistance) can be find out using **parallel connection of all impedances**. **The resulting impedance will be less than the least resistance in parallel.**

## Summary

When chaining filter blocks the following tips should be considered:
- Impedance of blocks should be higher than output impedance of source and lower than input impednce of receiver.
- Chained blocks should have increasing impedance - by factor 10 should be enough. It grants high output voltage of each block, and lowers overall current draw to be close to current draw caused by the lowest parallel impedance.

|BLock schematic of Audio Spectrum Display V1.0 |
|:-:|
|<img src="spectrum_display_HSVFFT_V1_vis.png" alt="immm" width="500">|

# Signal preparation

Output signal from microphone can by switched by plugging 3.5mm audio jack to 5 pin socket with mechanical switch. So both signals from microphone and audio jack should be similiar, but only microphone signal can be modified due to plug switching.

|Parameter|Microphone MAX9814|Audio Jack|
|:-|:-:|:-:|
|DC offset|1.245V|Symetrical 0V|
|Amplitude|0.765V|1.185V|
|V RMS|0.54|0.84|
|Gain|Constant 40/50/60dB|Dpendant on source|

Because of that, signal from microphone should be high pass filtered to remove DC component. At this stage builtin amplifier is set to maximum 60dB with option to change gain in case of clipping.

|Audio Jack max volume 1kHz | Microphone the same audio source, headphones 1cm from device|
|:-------------------------:|:-------------------------:|
|![](jack_1khz.BMP)  |  ![](micro_1khz.BMP)|

As depicted signal from microphone has lower maximal amplitude, but amplifying before being passed to switch socket is not vital. Both signals will be amplified before passing to ADC.

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

<img height="50" src="https://render.githubusercontent.com/render/math?math=X_C = \Im(-j\frac{1}{\omega C}) = \frac{1}{\omega C}" >

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

<img height="30" src="https://render.githubusercontent.com/render/math?math=u(t) = 1.2 \!%2B\! 0.76 \cdot cos(\omega t) [V]" >

or using notation with complex voltage:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{U} = 0.76 [V]" >

DC part will be trimmed, cosine part is lets say frozen so that final phasor in complex plane is rotated by phase shift and scaled by RMS voltage or amplitude (in this case).

Output voltage can be find out using voltage divider:

<img height="50" src="https://render.githubusercontent.com/render/math?math=\bar{U_{th}} = \bar{U}\frac{R_0}{R_0+R_{in} \!%2B\! Z_{C0}} [V]" >

During short circuit R0 is excluded and final current is:

<img height="50" src="https://render.githubusercontent.com/render/math?math=\bar{I_{th}} = \frac{\bar{U}}{R_{in} \!%2B\! Z_{C0}} [A]" >

Final Thevenin resistance is:

<img height="50" src="https://render.githubusercontent.com/render/math?math=\bar{Z_{th}} = \frac{\bar{U_{th}}}{\bar{I_{th}}} = \frac{R_0 (R_{in} \!%2B\! Z_{C0})}{R_0+R_{in}+Z_{C0}} [\Omega]" >

```python
ZC0 = -1j/(w*C0)
U_th = U * R0 /(R0 + Rin + ZC0)
I_th = U / (Rin + ZC0)
Z_th = R0 * (Rin + ZC0) / (R0 + Rin + ZC0)
```
Both voltage and current canbe plotted:

|Thevenin voltage in V | Thevenin current in A|
|:-------------------------:|:-------------------------:|
|![](capacitance_1u6F_voltage.png)  |  ![](capacitance_1u6F_voltage_current.png)|

Ratio between voltage and current is impedance and it describes both:
- magnitud: how many times voltage is greater than current
- phase: how current is shifted relative to current.

warto zauwazyc ze modul impedancji jest wieksza niz dla uproszczenia samej kapacytancji

# Audio Jack

Wstep do elektroniki - schemat blokowy i opiskazdego bloku


Coming coon



<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEyNjM2ODYyMzksLTQzNjc1NDYwNSwtMj
AzNDQxNjA1NCwtMjA0OTQ2MzI3LDE0OTk1MDY3MzQsOTg0ODQ2
NDM4LC0xNTYwODEwNSwtMTc2MDc5NjY1OSwtMTcwNDQxNTU1MF
19
-->