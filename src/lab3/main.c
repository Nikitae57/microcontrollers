

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
	//increaseDelay();
	sei();
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

void changeMode() {
	mode = (mode + 1) % 3;
	
	switch(mode) {
		case 0: {
			TCCR0A = 0x02; // CTC
			PORTB = 0xff; // Blink
			PORTF = 0b00000001;
			break;
		}
		
		case 1: {
			TCCR0A = 0x40; // Normal
			PORTB = 0xff; // Constant light
			PORTF = 0b00000010;
			break;
		}
		
		case 2: { // Moving light
			TCCR0A = 0x03; // Fast PWM
			PORTB = 0b10101010;
			PORTF = 0b00000100;
			break;
		}
	}
}

int main(void) {
	DDRA = 0;
	DDRB = 0xff;
	DDRF = 0xff;
	EICRA = 0b00111111;
	EIMSK = 0b00000111;
	PORTF = 0b00000001;
	sei();

	OCR0A = 200;
	TCCR0A = 0x02; // CTC
	TCCR0B = 0x05;
	//OCR0A = 255;
	//TIMSK1 = 1;
	//TCCR0B = 0b00000101;

	while (1) {
		if (TCNT0 == 1) {
			makeTick();
			//PORTF ^= 0xff;
			//PORTB = PORTB^0xff;
		}
		
		//makeTick();
		//dynamicDelay(delayMs);
	}
}

