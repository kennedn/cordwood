#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "micros.h"

#define DHT_PIN PB3
#define DATA_PIN PB0
#define LATCH_PIN PB1
#define CLOCK_PIN PB2

uint16_t temperature = 0;
uint16_t humidity = 0;


#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

void read_dht() {
  uint16_t _temperature;
  uint16_t _humidity;
  uint32_t startTime;
  uint16_t data;

  DDRB |= _BV(DHT_PIN);           // Set DHT_PIN as output
  PORTB &= ~(_BV(DHT_PIN));       // Set DHT_PIN LOW
  _delay_us(1000);
  PORTB |= _BV(DHT_PIN);          // set DHT_PIN HIGH
  DDRB &= ~(_BV(DHT_PIN));        // Set DHT_PIN as input

  for (int8_t i = -3; i < 2 * 40; i++) {
    uint8_t age;

    startTime = micros_get();
    do {
      age = (uint32_t)(micros_get() - startTime);
      if (age > 90) {
        // Timeout Error
        return;
      }
    } while (((PINB & _BV(DHT_PIN)) >> DHT_PIN) == (i & 1));

    if (i >= 0 && (i & 1)) {
      data <<= 1;

      if (age > 30) {
        data |= 1;
      }
    }

    if (i == 31) {
      _humidity = data;
    } else if (i == 63) {
      _temperature = data;
    }

  }

  if ((uint8_t)(((uint8_t)_humidity) + (_humidity >> 8) + ((uint8_t)_temperature) + (_temperature >> 8)) != (uint8_t)data ) {
    // Checksum Error
    return;
  }

  humidity = _humidity;
  temperature = _temperature;
}

void pulse_pin(uint32_t pin) {
  PORTB  |= _BV(pin);
  _delay_us(1);
  PORTB &= ~(_BV(pin));
  _delay_us(1);
}

void send_byte(uint8_t bits) {
  for (uint8_t i=0; i < 8; i++) {
    PORTB = (PORTB & ~_BV(DATA_PIN)) |            // Clear DATA_PIN
            (((bits & 1 << i) >> i) << DATA_PIN); // Set DATA_PIN high if bit is high
    pulse_pin(CLOCK_PIN);
  }
  pulse_pin(LATCH_PIN);
}

int main() {
  DDRB |= _BV(DATA_PIN) | _BV(LATCH_PIN) | _BV(CLOCK_PIN) | _BV(DHT_PIN);  // Set PINS as output
  micros_init();
  while(1){
    read_dht();
    uint8_t temp = (uint8_t)(round((temperature & 0x7FFF) * 0.1));
    send_byte(temp);
    _delay_ms(100);
  }

  return 0;
}

