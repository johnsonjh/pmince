; ATERM.ASM		Hand-coded terminal routines
;
;		Copyright 1981 by Mark of the Unicorn Inc.
;		Created from code in TERM.C 81.6.19 Gyro
;


		maclib cmac
		
		maclib bds

		maclib mince

		
		direct
		define TKbChk			; shadowing C version
		define TPrntCha		; shadowing C version
		define ATPrtChr		; print-char function for AsmDsp
		define ATWidth			; width function for AsmDsp
		define TPutChar		; shadowing C version
		define ATPutChr		; print-char function for AsmDsp
		enddir



; ****************************************************************
; TKbChk						Check for keyboard input and queue it

		prelude TKbChk, <QFull, QShove, TBell>
		
		push b
		lda inport + 0			; inport.biosp
		ora a
		reloc jz, TKC1			; if (inport.biosp ?
		reloc <lxi h,>, TKC2
		push h
		lhld 1
		inx h
		inx h
		inx h
		pchl
TKC2		ora a
		reloc jnz, TKC3		; ... bios (2) : ...
		pop b
		ret
TKC1		lda inport + 2			; inport.statport
		sta iohack + 1
		call iohack			; ... ((inp (inport.statport) ...
		mov b, a
		lda inport + 4			; inport.readymask
		ana b				; ... & inport.readymask) ...
		mov b, a
		lda inport + 5			; inport.polarity
		cmp b
		reloc jz, TKC3			; ... == inport.polarity) ...
		pop b
		ret
TKC3		lxi h, kbdq
		push h
		exrel call, QFull		; QFull (&kbdq)
		pop d
		mov a, h
		ora l
		reloc jnz, TKC4		; if (!QFull (&kbdq)) ...
		lxi h, kbdq
		push h
		lda inport + 0			; inport.biosp
		ora a
		reloc jz, TKC5			; inport.biosp ? ...
		reloc <lxi h,>, TKC7
		push h
		lhld 1
		lxi d, 6
		dad d
		pchl					; bios (3) (returns to TKC7)
TKC5		lda inport + 1			; inport.dataport
		sta iohack + 1
		call iohack			; inp (inport.dataport)
		mov b, a
		lda inport + 3			; inport.datamask
		ana b				; inp (inport.dataport) & inport.datamask
TKC7		mov l, a				; character in a
		mvi h, 0
		push h
		exrel call, QShove		; QShove (..., &kbdq)
		pop d
		pop d
		pop b
		ret
TKC4		exrel call, TBell
		pop b
		ret

		postlude TKbChk



; ****************************************************************
; TPrntCha (c)					Print a character
;	sets up registers, then calls ATPrtChr

		prelude TPrntCha, ATPrtChr
		
		call ma1toh
		mov a, l
		mvi l, 0				; force TForce call
		exrel jmp, ATPrtChr
		
		postlude TPrntCha



; ****************************************************************
; ATPrtChr (char)				Print a char for AsmDsp
;	char is in A
;	if L <= 0, calls TForce (for talking to rest of world)
;	if L > 0, punts TForce

		prelude ATPrtChr, <TForce, ATPutChr>
		
ATPC0	push b
		cpi 20h
		reloc jm, ATPC1		; if (char >= ' ' ...
		cpi 7Fh
		reloc jp, ATPC1		; && char <= '~')
		dcr l				; TForce flag
		mov c, a
		exrel cm, TForce
		exrel call, ATPutChr	; ATPutChr (char)
		lxi h, pcol
		inr m				; ++pcol
		mov a, m
		lxi h, clrcol
		xchg
		lhld prow
		dad d				; &clrcol[prow]
		cmp m
		reloc jm, ATPC2		; if (clrcol[prow] <= pcol) ...
		mov m, a				; clrcol[prow] = pcol
ATPC2	lxi h, scol
		inr m				; ++scol
		pop b
		ret
ATPC1	cpi 9h				; TAB
		reloc jnz, ATPC3		; if (char == TAB) ...
		lda tabincr
		ora a
		mov b, a
		lda pcol
		reloc jnz, ATPC4
		inr b
ATPC4	sub b
		reloc jp, ATPC4
		mov b, a
ATPC5	mvi a, 20h
		reloc call, ATPC0		; ATPrtChr (' ')
		mvi l, 1
		inr b
		reloc jnz, ATPC5
		pop b
		ret
ATPC3	cpi 80H
		reloc jm, ATPC6		; if (char & 0x80) ...
		mov c, a
		mvi a, 7Eh
		reloc call, ATPC0		; ATPrtChr ('~')
		mov a, c
		ani 7Fh
		mvi l, 1
		reloc call, ATPC0
		pop b
		ret
ATPC6	mov c, a
		mvi a, 5Eh			; ATPrtChr ('^')
		reloc call, ATPC0
		mov a, c
		xri 40H
		mvi l, 1
		reloc call, ATPC0		; ATPrtChr (char ^ '@')
		pop b
		ret
		
		postlude ATPrtChr



; ****************************************************************
; ATWidth (char)				Fast TWidth (in usual case)
;	Takes <char> in A; result in H

		prelude ATWidth, TWidth

		cpi 20h
		reloc jm, ATW1			; (char >= ' ' ...
		cpi 7Fh
		reloc jp, ATW1			; ... && char <= '~')
		lxi h, 1				; ? 1
		ret
ATW1		mov l, a
		mvi h, 0
		push h
		lhld pcol
		push h
		exrel call, TWidth		; : TWidth (pcol, char)
		pop d
		pop d
		ret

		postlude ATWidth



; ****************************************************************
; TPutChar (c)					Output a character

		prelude TPutChar, ATPutChr
		
		call ma1toh
		push b
		mov c, l				; c => c
		exrel call, ATPutChr
		pop b
		ret
		
		postlude TPutChar



; ****************************************************************
; ATPutChr (char)				Output a character for ATPrtChr
;	char is in C

		prelude ATPutChr
		
		lda outport + 0		; outport.biosp
		ora a
		reloc jz, TPC1			; if (outport.biosp)
		lhld 1
		lxi d, 9
		dad d
		pchl					; bios (4, c)
TPC1		lda outport + 2		; outport.statport
		sta iohack + 1
TPC3		call iohack			; inp (outport.statport)
		mov b, a
		lda outport + 4		; outport.readymask
		ana b
		mov b, a
		lda outport + 5		; outport.polarity
		cmp b
		reloc jnz, TPC3		; loop while not ready
		lda outport + 1		; outport.dataport
		sta iohack + 4
		mov a, c
		call iohack + 3		; outp (outport.dataport, c)
		ret
		
		postlude ATPutChr




;
;  End of ATERM.ASM  --  Hand-coded terminal routines
;

