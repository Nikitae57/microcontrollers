#include <avr/io.h>
#include <avr/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define F_CPU 1000000UL 
#define BITRATE 9600
#define BAUD F_CPU / (16 * BITRATE) - 1


bool shouldRun = true;
uint8_t mode = 0;
uint8_t delay = 50;
char comMsg;

void increaseDelay() {
	if (delay >= 250) {
		delay = 0;
	}
	
	delay += 50;
	OCR0A = delay;
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
			PORTB = ~PORTB;
			break;
		}
	}
}

void changeMode() {
	mode = (mode + 1) % 3;
	
	switch(mode) {
		case 0: {
			TCCR0A = 0x02; // CTC
			TCCR0B = 0x05;
			PORTB = 0xff; // Blink
			PORTF = 0b00000001;
			break;
		}
		
		case 1: {
			TCCR0A = 0x40; // Normal
			TCCR0B = 0x05;
			PORTB = 0xff; // Constant light
			PORTF = 0b00000010;
			break;
		}
		
		case 2: { // Moving light
			TCCR0B = (1 << WGM02) | 0x05;
			TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A0); // Fast PWM
			PORTB = 0b10101010;
			PORTF = 0b00000100;
			break;
		}
	}
}

char udr;
ISR(USART0_RX_vect) {
	udr = UDR0;
	
	switch(udr) {
		case 'm': {
			changeMode();
			break;
		}
		
		case 'r': {
			shouldRun = !shouldRun;
			break;
		}
		
		case 's': {
			increaseDelay();
			break;
		}
		
		default: {}
	}
}

int main(void) {
	DDRA = 0;
	DDRB = 0xff;
	DDRF = 0xff;
	EICRA = 0b00111111;
	EIMSK = 0b00000111;
	PORTF = 0b00000001;

	OCR0A = 50;
	TCCR0A = 0x02; // CTC
	TCCR0B = 0x05;

	UCSR0B = 0b10010000;
	UBRR0L = 0x06;
	
	sei();

	while (1) {
		if (TCNT0 == 0) {
			makeTick();
		}
	}
}

