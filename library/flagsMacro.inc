
;**************************************************************
;* macro: clF
;*
;* description: clear flag in flags register
;*
;* inputs: @0 - register of flags
;*				 @1 - value indicats flag in register of flags
;*
;*
;* registers modified: none
;**************************************************************
.macro clF ; clear flag
	push R16
  ldi R16, 0xFF
  subi R16, (1 << @1)
	and @0, R16
	pop R16
.endmacro

;**************************************************************
;* macro: seF
;*
;* description: set flag in flag register
;*
;* inputs: @0 - register of flags
;*				 @1 - value indicats flag in register of flags
;*
;*
;* registers modified: none
;**************************************************************
.macro seF ; set flag
	ori  @0, (1 << @1)
.endmacro
;**************************************************************
;* macro: tgF
;*
;* description: toggle flag
;*
;* inputs: @0 - register of flags
;*				 @1 - value indicats flag in register of flags
;*
;*
;* registers modified: none
;**************************************************************
.macro tgF ; toggle flag
	eor  @0, (1 << @1)
.endmacro
