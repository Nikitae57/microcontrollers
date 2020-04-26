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

.cseg 
.org 0 jmp START
;;.org $0002 jmp INT0_VEC
;;.org $0004 jmp INT1_VEC
;;.org $0006 jmp INT2_VEC

START:
    rcall INIT
	MAIN_LOOP:

    rjmp MAIN_LOOP

INIT:
	ser tmp
	out DDRB, tmp
	out DDRF, tmp

	ldi tmp, 0b00111111
	sts EICRA, tmp

	ldi tmp, 0b00000111
	out EIMSK, tmp

	ldi tmp, 1
	out PORTF, tmp

	ldi tmp, 50
	out OCR0A, tmp

	ldi tmp, 0x02
	out TCCR0A, tmp
	
	ldi tmp, 0x05
	out TCCR0B, tmp
ret