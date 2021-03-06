# Generator

This Python script allows generating bins to bounds assignment array for C code. There is available ready to use file with the array for 256 point FFT.

## Bins to bands

FFT produce an array of magnitudes. Each array's cell corresponds to magnitude of specific frequency range. This range width results from samping rate and FFT resolution.

Those frequency ranges are called bins, and they have constant width. To visualise them on the display with lower resolution some compression is needed. Thus, bins are converted to wider bounds by summing up corresponding magnitudes.

Sound can be better visualised using log scale - small frequencies will have higher resolution, higher frequencies will be grouped in wider ranges.

## Usage example

```python

list_bonds(220, 18000, 40000, 512, 19)

```

Above function returns list of size equal FFT points containing indices of bounds of the display to which bins (array position) should be added.