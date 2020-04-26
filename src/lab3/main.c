

#include <avr/io.h>
#include <avr/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>


bool shouldRun = true;
uint8_t mode = 0;
uint16_t delayMs = 200;

ISR(INT0_vect) {
	cli();
	shouldRun = !shouldRun;	
	sei();
}

ISR(INT1_vect) {
	cli();
	changeMode();	
	sei();
}

ISR(INT2_vect) {
	cli();
	increaseDelay();
	sei();
}

void dynamicDelay(uint16_t delayMs) {
	while (delayMs > 0) {
		delayMs--;
		_delay_ms(1);
	}
}

void makeTick() {
	if (!shouldRun) {
		PORTB = 0;
		return;
	}
	
	switch(mode) {
		case 0: {
			PORTB = ~PORTB;
			break;
		}
		
		case 1: {
			PORTB = 0xff;
			break;
		}
		
		case 2: {
			PORTB = (PORTB << 1);
			if (!(PORTB & (1 << 1))) {
				PORTB++; // If penultimate light is off then turn on last light
			}
			
			break;
		}
	}
}

void increaseDelay() {
	delayMs += 400;
	if (delayMs > 1000) {
		delayMs = 200;
	}
}

void changeMode() {
	mode = (mode + 1) % 3;
	
	switch(mode) {
		case 0: {
			PORTB = 0xff; // Blink
			break;
		}
		
		case 1: {
			PORTB = 0xff; // Constant light
			break;
		}
		
		case 2: { // Moving light
			PORTB = 0b10101010;
			break;
		}
	}
}

int main(void) {
	DDRA = 0;
    DDRB = 0xff;
	EICRA = 0b00111111;
	EIMSK = 0b00000111;
	sei();

    while (1) {
		makeTick();
		dynamicDelay(delayMs);
    }
}

