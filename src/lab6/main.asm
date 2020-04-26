;
; lab6.asm
;
; Created: 26.04.2020 18:10:32
; Author : Nikita
;

.include "m1281def.inc"

.def shouldRunReg = r16
.def modeReg = r17
.def tmp = r18
.def delayReg = r19
.def lightStateReg = r20

.cseg 
.org 0 jmp START
.org $0002 jmp INT0_VEC
.org $0004 jmp INT1_VEC
.org $0006 jmp INT2_VEC

START:
    rcall init
	MAIN_LOOP:
		in tmp, TCNT0
		cpi tmp, 0
		breq main_loop_tick

		jmp MAIN_LOOP

		main_loop_tick:
			call make_tick

    rjmp MAIN_LOOP

init:
	sei

	ser tmp
	out DDRB, tmp
	out DDRF, tmp

	ldi tmp, 0b00111111
	sts EICRA, tmp

	ldi tmp, 0b00000111
	out EIMSK, tmp

	ldi tmp, 1
	out PORTF, tmp

	ldi delayReg, 250
	out OCR0A, delayReg

	ldi tmp, 0x02
	out TCCR0A, tmp
	
	ldi tmp, 0x05
	out TCCR0B, tmp

	ldi shouldRunReg, 1
	ldi lightStateReg, 0xff
	out PORTB, lightStateReg
ret

make_tick:
	cpi shouldRunReg, 1
	breq tick

	;; Shut down	
	clr tmp
	out PORTB, tmp
	ret

	;; Make tick
	tick:
		cpi modeReg, 0
		breq mode_0

		cpi modeReg, 1
		breq mode_1

		cpi modeReg, 2
		breq mode_2

		mode_0:
			com lightStateReg
			out PORTB, lightStateReg
			ret

		mode_1:
			ser lightStateReg
			out PORTB, lightStateReg
			ret

		mode_2:
			com lightStateReg
			out PORTB, lightStateReg
			ret
ret

INT0_VEC:
	cli
		cpi shouldRunReg, 0
		breq turn_on

		;; If we here, then light is turned on. Need to turn off
		ldi shouldRunReg, 0

		turn_on:
			ldi shouldRunReg, 1
	sei
reti

INT1_VEC:
	cli
		inc modeReg
		
		cpi modeReg, 3
		breq reset_mode

		jmp set_mode
		reset_mode:
			clr modeReg
			
		set_mode:
			cpi modeReg, 0
			breq set_mode_0

			cpi modeReg, 1
			breq set_mode_1

			cpi modeReg, 2
			breq set_mode_2

			set_mode_0:
				ldi tmp, 2
				out TCCR0A, tmp

				ldi tmp, 5
				out TCCR0B, tmp

				ldi lightStateReg, 0xff
				out PORTB, lightStateReg

				ldi tmp, 1
				out PORTF, tmp
				
				sei
				reti

			set_mode_1:
				ldi tmp, 0x40
				out TCCR0A, tmp

				ldi tmp, 5
				out TCCR0B, tmp

				ldi lightStateReg, 0xff
				out PORTB, lightStateReg

				ldi tmp, 2
				out PORTF, tmp
				
				sei
				reti

			set_mode_2:
				ldi tmp, 0b01000011
				out TCCR0A, tmp

				ldi tmp, 0b00001101
				out TCCR0B, tmp

				ldi lightStateReg, 0b10101010
				out PORTB, lightStateReg

				ldi tmp, 4
				out PORTF, tmp
				
				sei
				reti

	sei
reti

INT2_VEC:
	cli
	
	cpi delayReg, 250
	breq reset_delay

	jmp increase_delay
	reset_delay:
		ldi delayReg, 0

	increase_delay:
		ldi tmp, 50
		add delayReg, tmp
		out OCR0A, delayReg

	sei
reti