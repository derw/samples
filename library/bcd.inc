;* * * * * * * * * * * * * * * * * * * * *
;* Copyright (C) 2020
;* Marek Gębski marekgebski(at)yandex.ru
;*
;* * * * * * * * * * * * * * * * * * * * *



.def rmp = R16 ; Define multipurpose register
.def rm1 = R18 ; Define multipurpose register
.def rm2 = R19 ; Define multipurpose register
.def counter  = R17
.def NumH     = R31
.def NumL     = R30
.def dig1   = R22
.def dig2   = R23
.def dig3   = R24
.dseg
digit1:
  .byte 1
digit2:
  .byte 1
digit3:
  .byte 1
.cseg
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
; Word to compressed BCD
w2bcd:
  clr dig1       ; Oneness and dozens
  clr dig2       ; Hundreds and thousands
  clr dig3       ; Tens of thousands
  ldi counter, 16  ; 16 shift counter
  mov rm1, NumL    ; Load first byte 
  mov rm2, NumH    ; Load secend byte
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
  lsl  rm1         ; Shift left Binary
  rol  rm2         ; Shift left with carry
  rol  dig1      ; Shift left with carry 
  rol  dig2      ; Shift left with carry
  rol  dig3      ; Shift left with carry
  dec  counter     ; Subtracts 1 from counter
  brne w2bcd_shift ; Brench if counter<>0
  sts digit1, dig1
  sts digit2, dig2
  sts digit3, dig3
  ret
;* input:
;*   NumL
;* output: compressed BCD          
;*   digit1
;*            LSB oneness
;*            MSB tens
;*   digit2
;*            LSB hundreds
; Byte to compressed BCD
b2bcd:
  clr dig1
  clr dig2
  clr dig3
  ldi counter, 8
  mov rm1, NumL
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
  lsl  rm1        ; shift left Binary
  rol  dig1
  rol  dig2
  dec  counter
  brne b2bcd_shift
  sts digit1, dig1
  sts digit2, dig2
  sts digit3, dig3
  ret
