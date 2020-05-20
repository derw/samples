;* * * * * * * * * * * * * * * * * * * * *
;* Print number, word 
;* input:  none
;* output: none
;* registers modified: none
;* * * * * * * * * * * * * * * * * * * * *
w2bcd_lcd_print:

  ; digit, SRAM address,
  ; representing the BCD digit.
  ldi YH, HIGH(digit)
  ldi YL, LOW(digit)
  ; value, SRAM address,
  ; repsesenting the ASCII code of digit.
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
	; BCD to ASCII 
  ldi  rm1, 6
exchange:
	ld   rmp, Y+
	ori  rmp, 0x30
	st  Z+, rmp
  dec  rm1
  brne exchange
	
	; space and minus
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
loop0d:
cpi  ZL, LOW(valueWEnd) ; branch if end
  breq loop0de
	ld   rmp, Z  ; load digit do R16
	cpi  rmp, 0x30 ; branch if not ASCII zero
	brne loop0de
	ldi  rmp, 0x20 ; load space char to R16
	st   Z+,  rmp  ; replace zero with space
	rjmp loop0d    ; loop
loop0de:
	ldi  rmp, 0x2D ; load minus char to R16
	sbrc rFlag, fMinusW ; skip if not negative value
	st   -Z,  rmp ; otherwise store minus char 

	; send to lcd
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
	ldi  rm1, 6
loop0c:
	ld rmp,Z+
	rcall send_to_lcd ; Send to LCD
	dec rm1
	brne loop0c
ret
;* * * * * * * * * * * * * * * * * * * * *
;* Print number, one byte
;* input:  none
;* output: none
;* registers modified: none
;* * * * * * * * * * * * * * * * * * * * *
b2bcd_lcd_print:

  ldi YH, HIGH(digit)
  ldi YL, LOW(digit)
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
	; BCD to ASCII 
  ldi  rm1, 4
bexchange:
	ld   rmp, Y+
	ori  rmp, 0x30
	st  Z+, rmp
  dec  rm1
  brne bexchange
	
	; space and minus
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
bloop0d:
	cpi  ZL, LOW(valueBEnd)
  breq bloop0de
	ld   rmp, Z
	cpi  rmp, 0x30
	brne bloop0de  ; Is ASCII zero char
	ldi  rmp, 0x20 ; white space char
	st   Z+,  rmp       ; store minus signe
  rjmp bloop0d
bloop0de:
	sbrc rFlag, fMinusB ; Is negative number
	ldi  rmp, 0x2D      ; minus signe
	sbrc rFlag, fMinusB ; Is negative number
	st   -Z,  rmp       ; store minus signe

	; send to lcd
  ldi ZH, HIGH(value)
  ldi ZL, LOW(value)
	ldi  rm1, 4
bloop0c:
	ld rmp,Z+
	rcall send_to_lcd ; Send to LCD
	dec rm1
	brne bloop0c
ret

;* * * * * * * * * * * * * * * * * * * * *
;* Cursor position
;* input:  R16
;* output: none
;* registers modified: R16
;* * * * * * * * * * * * * * * * * * * * *
cursor_position:
  cbi PORTB_LCD, RS_LCD
  rcall send_to_lcd
  sbi PORTB_LCD, RS_LCD
  ret

;* * * * * * * * * * * * * * * * * * * * *
;* Initialize lcd 
;* input:  none
;* output: none
;* registers modified: none
;* * * * * * * * * * * * * * * * * * * * *
on_lcd:
; setting the output direction of the lines connected to the LCD
  in  R16, DDRB_LCD
  ori R16, (1<<RS_LCD)|(1<<EN_LCD)
  out DDRB_LCD, R16

  in  R16, PORTB_LCD
  andi R16, ~((1<<RS_LCD)|(1<<EN_LCD))
  out PORTB_LCD, R16

  in  R16, DDR_LCD
  ori R16, (1<<DB7_LCD)|(1<<DB6_LCD)|(1<<DB5_LCD)|(1<<DB4_LCD)
  out DDR_LCD, R16

  in  R16, PORT_LCD
  andi R16, ~((1<<DB7_LCD)|(1<<DB6_LCD)|(1<<DB5_LCD)|(1<<DB4_LCD))
  out PORT_LCD, R16

; wait at least 45 ms for voltage stabilization
  delay_ms_45

  sbi PORTB_LCD, EN_LCD
  ; load seqance 0011
  in R16, PORT_LCD
  ori R16, (1<<DB4_LCD)|(1<<DB5_LCD)
  out PORT_LCD, R16
  ; confirm with falling edge EN
  cbi PORTB_LCD, EN_LCD

  ; wait at least 4.1 ms
	delay_us_4100

  ; load sequence 0011 for the secend time.
  sbi PORTB_LCD, EN_LCD
  ; confirm with falling edge EN
  cbi PORTB_LCD, EN_LCD

  ; wait at least 100 µs
	delay_um_100

  ; load sequence 0011 for the third time.
  sbi PORTB_LCD, EN_LCD
  ; confirm with falling edge EN
  cbi PORTB_LCD, EN_LCD

  ; wait at least 100 µs
	delay_um_100

  sbi PORTB_LCD, EN_LCD
  ; 4-bit interface setting
  cbi PORT_LCD, DB4_LCD
  ; confirm with falling edge EN
  cbi PORTB_LCD, EN_LCD

  ; set display parameters
  ;D4 = 0 (the data has 4 bits)
  ;D3 = 1 (2 lines of characters)
  ;D2 = 0 (matrix 5 * 8 pixels)
  ldi r16, 0b00001000
  cbi PORTB_LCD, RS_LCD
  rcall send_to_lcd
  sbi PORTB_LCD, RS_LCD

  ; set the display mode
  ; bit 2 = 1 (incrementing the data write address)
  ; bit 1 = 1 (disables shifting right)
  ldi r16, 0b00000110
  cbi PORTB_LCD, RS_LCD
  rcall send_to_lcd
  sbi PORTB_LCD, RS_LCD

  ; turn on the display
  ; bit 2 = 1 (display on)
  ; bit 1 = 1 (cursor active)
  ; bit 0 = 0 (cursor not flashing)
  ldi r16, 0b00001110
  cbi PORTB_LCD, RS_LCD
  rcall send_to_lcd
  sbi PORTB_LCD, RS_LCD

  rcall clean_lcd

ret

;* * * * * * * * * * * * * * * * * * * * *
;* Clean lcd
;* input:  none
;* output: none
;* registers modified: R16
;* * * * * * * * * * * * * * * * * * * * *
clean_lcd:
  ldi R16, 1
  cbi PORTB_LCD, RS_LCD
  rcall send_to_lcd
  sbi PORTB_LCD, RS_LCD

  ; wait 1.64 ms
  delay_us_1640
ret

;* * * * * * * * * * * * * * * * * * * * *
;* Print on lcd 
;* input:  Z - address to data
;*         R20 - number of characters 
;* output: none
;* registers modified: R16, R20
;* * * * * * * * * * * * * * * * * * * * *
print_lcd:
  petla_print_lcd:
  lpm R16, Z+
  rcall send_to_lcd
  dec R20
  brne petla_print_lcd
ret

;* * * * * * * * * * * * * * * * * * * * *
;* Sent byte to lcd
;* in the procedure it is assumed that lines DB4, DB5, DB6 and DB7
;* connected to the 4 oldest lines of the PORT_LCD port
;* input:  R16 - data
;* output: none
;* registers modified:
;* * * * * * * * * * * * * * * * * * * * *
send_to_lcd:

  push R18
  push rm1
  
  sbi PORTB_LCD, EN_LCD  ;set EN

  ; sending the oldest 4 bits of data
  mov rm1, R16 ; copy data to rm1
  andi rm1, 0b11110000 ; leave the 4 oldest bits
  in R18, PORT_LCD ; download port status from LCD
  andi R18, 0b00001111 ; clear the 4 oldest bits
  or R18, rm1 ; set the bits according to the data system
  out PORT_LCD, R18 ; send the 4 oldest bits to the LCD display

  nop ; wait 1 measure
  cbi PORTB_LCD, EN_LCD ; confirmation of sending data (falling edge)

  nop ; wait 1 measure
  sbi PORTB_LCD, EN_LCD  ; EN setting

  ; sending the 4 youngest data bits
  swap R16 ; replace the 4 youngest bits with the oldest
  andi R16, 0b11110000 ; leave the 4 oldest bits
  in R18, PORT_LCD ; download port status from LCD
  andi R18, 0b00001111 ; clear the 4 oldest bits
  or R18, R16 ; set the bits according to the data system
  out PORT_LCD, R18  ; send the 4 youngest bits to the LCD display

  nop ; wait 1 measure
  cbi PORTB_LCD, EN_LCD ; confirmation of sending data (falling edge)

  ; wait at least 37 µs
	delay_us_37
  pop rm1
  pop R18

ret
