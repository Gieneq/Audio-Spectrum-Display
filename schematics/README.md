# Electronics

|BLock schematic of Audio Spectrum Display V1.0 |
|:-:|
|<img src="spectrum_display_HSVFFT_V1_vis.png" alt="" width="500">|

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

<img height="30" src="https://render.githubusercontent.com/render/math?math=u(t) = 1.2 + 0.76*cos(\omega t) [V]" >

or using notation with complex voltage:

<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{U} = 0.76 [V]" >

DC part will be trimmed, cosine part is lets say frozen so that final phasor in complex plane is rotated by phase shift and scaled by RMS voltage or amplitude (in this case).

Output voltage can be find out using voltage divider:
<img height="30" src="https://render.githubusercontent.com/render/math?math=\bar{U_{out}} = \bar{U}\frac{R_0}{R_0+R_{in}+C_0}" >


# Audio Jack

Wstep do elektroniki - schemat blokowy i opiskazdego bloku


Coming coon



