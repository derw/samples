# samples
Assembler samples for atmega µC.

## logical operator
### xor
```
        sbis PIND, 0 ; skip if set 
        sbic PIND, 1 ; skip if clear
        sbic PIND, 1 ; skip if clear
        rjmp zero
one:    sbi PORTD, 2
        rjmp return
zero:   cbi PORTD, 2
return:
```
### or
```
        sbis PIND, 0 ; skip if set 
        sbic PIND, 1 ; skip if clear
        sbis PIND, 0 ; skip if set
        rjmp zero
one:    sbi PORTD, 2
        rjmp return
zero:   cbi PORTD, 2
return:
```
### and
```
        sbis PIND, 0 ; skip if set 
        rjmp zero
        sbis PIND, 1 ; skip if set
        rjmp zero
one:    sbi PORTD, 2
        rjmp return
zero:   cbi PORTD, 2
return:
```
