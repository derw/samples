;* * * * * * * * * * * * * * * * * * * * *
;* Copyright (C) 2020
;* Marek Gębski marekgebski(at)yandex.ru
;*
;*  binary-coded decimal implementation
;*
;*
;* * * * * * * * * * * * * * * * * * * * *

.def counter  = R17
.def NumH     = R31
.def NumL     = R30
.def dig1   	= R22
.def dig2   	= R23
.def dig3   	= R24
.dseg
dNumber:
dNumH:
  .byte 1
dNumL:
  .byte 1
compDigit1:
  .byte 1
compDigit2:
  .byte 1
compDigit3:
  .byte 1
digit:
	.byte 6
value:
  .byte 3
valueBEnd:
  .byte 2
valueWEnd:
.cseg
;* * * * * * * * * * * * *
;* Binary-coded decimal word long
;* input:
;*   NumL
;*   NumH                          
;* output: compressed BCD          
;*   digit1
;*            LSB oneness
;*            MSB tens
;*   digit2
;*            LSB hundreds
;*            MSB thousands
;*   digit3
;*            LSB ten of thousands
;* Word to compressed BCD
;* * * * * * * * * * * * *
w2bcd:
  clr dig1       ; Oneness and dozens
  clr dig2       ; Hundreds and thousands
  clr dig3       ; Tens of thousands
  ldi counter, 16  ; 16 shift counter
w2bcd_shift:
  mov  rmp, dig1 ; Check first oneness
  andi rmp, 0xF    ; Discard 4 MSB 
  cpi  rmp, 0x5    ; if lower then 5
  brlo w2bcd_next1 ; skip
  ldi  rmp, 0x3    ; else add 3
  add  dig1, rmp
w2bcd_next1:
  mov  rmp, dig1 ; Check dozens
  andi rmp, 0xF0   ; Discard 4 LSB 
  cpi  rmp, 0x50   ; if lower then 5
  brlo w2bcd_next2 ; skip
  ldi  rmp, 0x30   ; else add 30
  add  dig1, rmp
w2bcd_next2:
  mov  rmp, dig2 ; Check hundreds
  andi rmp, 0xF
  cpi  rmp, 0x5
  brlo w2bcd_next3
  ldi  rmp, 0x3
  add  dig2, rmp
w2bcd_next3:
  mov  rmp, dig2 ; Check thousands
  andi rmp, 0xF0
  cpi  rmp, 0x50
  brlo w2bcd_next4
  ldi  rmp, 0x30
  add  dig2, rmp
w2bcd_next4:
  lsl  NumL         ; Shift left Binary
  rol  NumH         ; Shift left with carry
  rol  dig1      ; Shift left with carry 
  rol  dig2      ; Shift left with carry
  rol  dig3      ; Shift left with carry
  dec  counter     ; Subtracts 1 from counter
  brne w2bcd_shift ; Brench if counter<>0
  sts compDigit1, dig1
  sts compDigit2, dig2
  sts compDigit3, dig3
ret

;* * * * * * * * * * * * *
;* Uncompress BCD
;* input:
;*    compDigit1 - oneness, tens
;*    compDigit2 - hundreds, thousands
;*    compDigit3 - ten of thousens
;* output:
;*     digit - address of SRAM with numbers
;*
;* * * * * * * * * * * * *
w2bcd_uncompress:
  ldi YH, HIGH(digit)
  ldi YL, LOW(digit)

	lds rmp, compDigit1
	andi rmp, 0xF
	std  Y+5, rmp

	lds rmp, compDigit1
	andi rmp, 0xF0
	swap rmp
	std  Y+4, rmp

	lds rmp, compDigit2
	andi rmp, 0xF
	std  Y+3, rmp

	lds rmp, compDigit2
	andi rmp, 0xF0
	swap rmp
	std  Y+2, rmp

	lds rmp, compDigit3
	andi rmp, 0xF
	std  Y+1, rmp

	ldi rmp, 0x0				;* minus signe
	std  Y+0, rmp
ret

;* * * * * * * * * * * * *
;* Convert to negative number
;* input:
;*     NumL
;*     NumH
;* output:
;*     NumL
;*     NumH
;* set flag indicate negative number
;*
;* * * * * * * * * * * * *
w2bcd_minus:
	clF rFlag, fMinusW
	sbrs NumH, 7
	ret
	seF rFlag, fMinusW
	ser rmp
	eor NumL, rmp
	eor NumH, rmp
	ldi rmp,  1
	add NumL, rmp
	clr rmp
	adc NumH, rmp
ret

;* * * * * * * * * * * * *
;* Binary-coded decimal byte long
;* input:
;*   NumL
;* output: compressed BCD          
;*   digit1
;*            LSB oneness
;*            MSB tens
;*   digit2
;*            LSB hundreds
;* * * * * * * * * * * * *
b2bcd:
  clr dig1
  clr dig2
  clr dig3
  ldi counter, 8
b2bcd_shift:
  mov  rmp, dig1
  andi rmp, 0xF
  cpi  rmp, 0x5
  brlo b2bcd_next1
  ldi  rmp, 0x3
  add  dig1, rmp
b2bcd_next1:
  mov  rmp, dig1
  andi rmp, 0xF0
  cpi  rmp, 0x50
  brlo b2bcd_next2
  ldi  rmp, 0x30
  add  dig1, rmp
b2bcd_next2:
  lsl  NumL        ; shift left Binary
  rol  dig1
  rol  dig2
  dec  counter
  brne b2bcd_shift
  sts compDigit1, dig1
  sts compDigit2, dig2
  sts compDigit3, dig3
ret

;* * * * * * * * * * * * *
;* Uncompress BCD
;* input:
;*    compDigit1 - oneness, tens
;*    compDigit2 - hundreds
;* output:
;*     digit - address of SRAM with numbers
;*
;* * * * * * * * * * * * *
b2bcd_uncompress:
  ldi YH, HIGH(digit)
  ldi YL, LOW(digit)

	lds rmp, compDigit1  ;* oneness
	andi rmp, 0xF
	std  Y+3, rmp

	lds rmp, compDigit1  ;* tens
	andi rmp, 0xF0
	swap rmp
	std  Y+2, rmp

	lds rmp, compDigit2  ;* hundreds
	andi rmp, 0xF
	std  Y+1, rmp

	ldi rmp, 0x0				;* minus signe
	std  Y+0, rmp
ret

;* * * * * * * * * * * * *
;* Convert to negative number
;* input:
;*     NumL
;* output:
;*     NumL
;* set flag indicate negative number
;*
;* * * * * * * * * * * * *
b2bcd_minus:
	clF rFlag, fMinusB
	sbrs NumL, 7
	ret
	seF rFlag, fMinusB
	ser rmp
	eor NumL, rmp
	ldi rmp,  1
	add NumL, rmp
ret

