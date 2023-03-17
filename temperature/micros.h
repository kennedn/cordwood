#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION		10	        // Max resolution in uS
#define PRESCALER 		8	        // Clock divider

static volatile uint32_t microseconds;

void micros_init(void){
	TCCR0A = _BV(WGM01);		        // Set TIMER0 in Compare Match (CTC) mode
	TCCR0B = _BV(CS01);		        // Set TIMER0 prescaler to 8
	TIMSK = _BV(OCIE0A);		        // Enable Compare Match Interrupt
	OCR0A = ((F_CPU / PRESCALER) /          // Set the match value based on CPU speed and resolution,
	         (1000000 / RESOLUTION)) - 1;	// (clock freq / resolution freq) - 1
	sei();				        // Enable Interrupts
}

// Get current microseconds
uint32_t micros_get(){
	uint32_t us;

	cli();	 			        // Disable interrupts
	us = microseconds;
	sei();				        // Enable interrupts

	return us;
}

ISR(TIMER0_COMPA_vect){
	microseconds += RESOLUTION;
}
