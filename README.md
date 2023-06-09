Low level experiments with the [Cordwood soldering puzzle](https://boldport.com/shop/cordwood-puzzle-1)

| folders     | simulation | device | description |
|-------------|------------|--------|-------------|
| blink       | N/A        |rasp pi | Rotates through green, yellow, red LEDs|
| shift       | N/A        |rasp pi | Same functionality as blink.c but using a 74HC595 shift register |
| temperature | [<img src='https://kennedn.com/blog/posts/snowdon/wokwi_badge.svg'/>](https://wokwi.com/projects/359204276534882305)  |attiny85|  Writes temperature data from a DHT22 to the cordwood LEDs via a 74HC595 shift register |

![](./media/cordwood.gif)

# Build

## Blink & Shift

### Compile and Run

For a given folder, do the following:

```bash
cd ${folder}
# Compile
gcc ${folder}.c -o ${folder}
# Run
sudo ./${folder}
```

## Temperature

### Schematic

![](media/temperature_schematic.svg)

### Final build

<p align="center">
    <img src="./media/temperature.gif" width="48%"/>
    <img src="./media/temperature.jpg" width="48%"/>
</p>

### Fuse setting

The temperature project requires a clock of at least 8MHz to be able to bit bang DHT22 data effectivly. 

The clock can be incrased from the 1MHz factory setting to 8Mhz by setting the Low Fuse to `E2`:

```bash
sudo avrdude -p t85 -c linuxspi -P /dev/spidev0.0 -b 10000 -U lfuse:w:0xE2:m
```

This disables the factory default clock divider bit (CKDIV8). See [fusecalc](https://www.engbedded.com/fusecalc/) for more details.

### Compile and Burn

For a given folder, do the following:

```bash
cd ${folder}
# Compile
avr-gcc ${folder}.c -mmcu=attiny85 -O3 -o ${folder}.bin
# Convert to hex format
avr-objcopy -O ihex ${folder}.bin ${folder}.hex
# Burn to chip
sudo avrdude -p t85 -c linuxspi -P /dev/spidev0.0 -b 10000 -U flash:w:${folder}.hex
```

