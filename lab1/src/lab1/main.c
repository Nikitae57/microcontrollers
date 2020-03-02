/*
 * lab1.c
 *
 * Created: 28.02.2020 20:33:12
 * Author : Nikita
 */ 

#include <avr/io.h>
#include <avr/delay.h>
#include <stdbool.h>


bool shouldRun = true;
uint8_t mode = 0;
uint16_t delayMs = 200;

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

    while (1) {
		
		if (PINA & (1 << 0)) { // Turn on/off
			shouldRun = !shouldRun;	
		} else if (PINA & (1 << 1)) { // Switch mode
			changeMode();
		} else if (PINA & (1 << 2)) { // Change speed
			increaseDelay();
		}
		
		makeTick();
		dynamicDelay(delayMs);
    }
}

