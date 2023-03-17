Raspberry Pi raw register access experiments with the [Cordwood soldering puzzle](https://boldport.com/shop/cordwood-puzzle-1)

| folders | description |
|------|-------------|
| blink | Rotates through green, yellow, red LEDs|
| shift | Same functionality as blink.c but using a 74HC595 shift register |
| temperature | Writes temperature data from a DHT22 to the cordwood LEDs via a 74HC595 shift register |

![](./media/cordwood.gif)

# Build

For a given folder, do the following:

```bash
cd ${folder}
# Compile
avr-gcc ${folder}.c -mmcu=attiny85 -Os -o ${folder}.bin
# Convert to hex format
avr-objcopy -O ihex ${folder}.bin ${folder}.hex
# Burn to chip
sudo avrdude -p t85 -c linuxspi -P /dev/spidev0.0 -b 10000 -U flash:w:${folder}.hex
```


