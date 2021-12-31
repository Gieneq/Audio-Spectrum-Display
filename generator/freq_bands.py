def list_bonds(min_freq, max_freq, sampling_rate, samples_count, bands_count):
    bands = []

    bin_width = sampling_rate / samples_count
    print(f"Bin width: {round(bin_width, 3)} Hz")

    usable_bins = samples_count//2 - 1
    print(f"Usable bins: {usable_bins}")

    freq_multiplier = (max_freq/min_freq)**(1.0/(bands_count - 1))

    print(f"Frequency multiplier: {round(freq_multiplier, 3)}")

    center_bins = []
    for i_band in range(bands_count+1):
        band_freq = min_freq * (freq_multiplier) ** i_band
        center_bin = (band_freq / bin_width)
        center_bins.append({"i_band": i_band, "band_freq":band_freq, "center_bin":center_bin})
    # print(center_bins)

    min_max_bonds = []

    for i_band in range(bands_count):
        sel_bin = center_bins[i_band]["center_bin"]
        next_bin = center_bins[i_band+1]["center_bin"]
        low_bin = 0
        if i_band > 0:
            low_bin = max(int(round(min_max_bonds[i_band - 1]["high"])), 0)
        high_bin = min(int(round((next_bin-sel_bin)//2 + sel_bin)), usable_bins)
        if i_band == bands_count - 1:
            high_bin = usable_bins
        range_list = list(range(low_bin, high_bin))
        min_max_bonds.append({"low":low_bin, "high":high_bin, "range":range_list})
        print(f"{i_band}:  {low_bin}-{high_bin},  {range_list}")

    check_sum = 0
    for i, min_max_bond in enumerate(min_max_bonds):
        items_count = len(min_max_bond["range"])
        check_sum += items_count
        bands.extend([i]*items_count)

    print(f"Usable bins count ({check_sum}) correct: {check_sum == usable_bins}")


    return bands


bands = list_bonds(220, 18000, 40000, 512, 19)
# bands = list_bonds(260, 18000, 40000, 512, 19)

str_output = f"int bands[{len(bands)}]" + " = {\n"
file = open("bands_from_bins.txt", 'w')

for i, b in enumerate(bands):
    str_output += str(b) + ", "
    if i % 32 == 31:
        str_output += "\n"
str_output = str_output[0:-2]
str_output += "};\n"

file.write(str_output)
