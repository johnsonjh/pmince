;	PWDIR2.ASM -- DISK DIRECTORIES FROM INSIDE THE PW EDITOR --
;
;	PWDIR2.ASM -- PWDIR.ASM by Ron Friedrich, 5/1/87; modifications
;	in this version by Andrew Marchant Shapiro, 4/30/89.
;
;	NOTE:  This version will ONLY work with Perfect Writer 1.20;  it
;	probably can be modified for 1.03 without trouble, but be careful!
;
;	Ron's original PWDIR.ASM addressed two shortfalls in the PW editor:
;	it allowed you to get a directory of any disk from within the editor,
;	and it automatically logged in new disks, so you didn't need to
;	worry about hitting a disk R/O error.
;
;	The one problem was that Ron made space for it by overwriting the
;	Perfect Speller functions that integrated PW with its spell checker.
;	That was no real problem unless you LIKED PS, and I do, since I have
;	a RAMdisk that's just big enough for PW and PS to reside on.  No
;	other spelling checker quite fit, or was as nicely integrated.
;
;	So I decided to fix things up.  In this version of PWDIR.ASM, PWDIR is
;	added on the END of PW.COM instead of in the middle.  You assemble
;	this file to HEX, and patch it onto PW.COM with DDT.  You then have
;	to SAVE 142 PW.COM.  This makes a PW file that's 36K long
;	instead of 35K on the disk, but that's no problem.
;
;	OK, so the patch is in place.  HOW DO YOU ACTIVATE THE SUCKER? 
;	For this part, you need DDT or PATCH or somesuch.  Use the
;	PW.SYM file (it came with PW) to find a function you don't
;	really need -- like the HELP function, MHELP, which resides at
;	45F1H.  Go to address 45F1H, and patch in the following code:
;
;	CALL 89A0
;	RET
;
;	If you're doing this with PATCH, enter the following bytes:
;
;	CD A0 89 C9
;
;	and save the result.  Now, whenever you push the HELP key sequence
;	(usually ^X-H or ESC-H), it calls MHELP, which in turn calls --
;	yup -- that set of routines hanging off the end of PW.  No
;	sacrifice of internal code is needed, beyond your giving up
;	your least favorite function.  You might want to patch in the
;	calling code at MWHATVER (4AC9).  That's the really useful function
;	that returns the version and date for PW.  Wherever you do the
;	patch, the code remains the same.
;
;	Note -- the directory output overwrites what you have on the screen,
;	but doesn't actually enter the buffer.  It's video only.  To get back
;	to your editing session. use the refresh screen command, ^L or ESC-^L.
;
;
;	-----------------------IMPORTANT-------------------------
;
;	Note that I have removed the disk system reset code from
;	PWDIR.ASM.  The reason has to do with the fact that Ron
;	located it at MGOSPELL, the routine that calls PS.COM in the
;	first place, and that most people still using CP/M now have
;	BDOS replacements that handle logging in new floppies
;
;	---------------------------------------------------------
;
;	The modified code follows.  Like Friedrich's original, it's PD,
;	and you can give it to anyone you want...
;
;
;	PWDIR2.ASM -- ANDREW MARCHANT SHAPIRO  rev. 4/30/89
;	PWDIR.ASM -- RON FRIEDRICH  rev. 5/1/87
;
YES	EQU	0FFH
NO	EQU	0
;
;
ORG 36E1H
TAIL		EQU	89A0H
OVLSTART	EQU	5F00H	;NEXT ROUND SECTOR ADR AFTER MGOSPELL
;
RECSTART	EQU	(OVLSTART-100H)/80H
;
;
;bdos functions
CONIN	EQU	1	;READ CONSOLE (C; BYTE RETURNED TO A)
CONOUT	EQU	2	;TYPE CHARACTER (C; BYTE IN E)
PRINTS	EQU	9	;PRINT STRING FUNCTION (C; ADR IN DE)
RSTDSK	EQU	13	;RESET DISK SYSTEM (C)
SELDSK  EQU	14	;SELECT DISK (C; DISK # IN E)
OPENF	EQU	15	;OPEN FILE
CLOSEF	EQU	16	;CLOSE FILE
SRCH	EQU	17	;SEARCH DIRECTORY (C; FCB IN DE)
SRCHMOR EQU	18	;SEARCH FOR NEXT IN DIRECTORY (C) 
DELF	EQU	19	;DELETE FILE
READF	EQU	20	;READ FILE
WRITEF	EQU	21	;WRITE FILE
MAKEF	EQU	22	;CREATE FILE
CURDSK	EQU	25	;CURRENT DISK (C; DISK # RETURNED IN A)
SETDMA	EQU	26	;(C; DMA adr in DE)
ALLOC	EQU	27	;Get ALLOCATION address (C; returned in HL)
PARAMS	EQU	31	;Get DISK PARAMETER address (C; returned in HL)
READRND	EQU	33	;READ RANDOM FILE
;
;BDOS ADR
BDOS	EQU	5	;DOS ENTRY POINT
FCB1	EQU	5CH	;DEFAULT FILE CONTROL BLOCK
DMA1	EQU	80H	;DEFAULT DIRECT (DISK) MEMORY ACCESS AREA
FCBLEN	EQU	36
CPMREC	EQU	33
;
;	NON GRAPHIC CHARACTERS
LF	EQU	10	;LINE FEED
UP	EQU	11	;CURSOR UP
CR	EQU	13	;CARRIAGE RETURN
CLINE	EQU	24	;CLEAR TO END OF LINE
CLEAR	EQU	26	;CLEAR SCREEN
ESC	EQU	27
HOME	EQU	30
SPACE	EQU	32
;
;DISPLAY DISK DIRECTORY ROUTINE:
;
	ORG	TAIL
	PUSH B
 	CALL	TYPE
	DB	ESC,'=7 ',CLINE,'DIRECTORY: Which disk drive? ',0
	mvi	c,conin
	call	bdos
	CPI	'A'
	JC	ABORT
	call	upcase
	STA	DIRSTAT
;
	MVI	C,CURDSK
	CALL	BDOS
	STA	OLDSEL
	MVI	C,RSTDSK	;NB: also resets DMA to 80H
	CALL	BDOS
	LDA	DIRSTAT
	SUI	'A'
	MOV	E,A
	MVI	C,SELDSK
	CALL	BDOS
	MVI	A,1
	STA	FIVE
	MVI	B,5
	LXI	H,COLCT
	MVI	M,4
	INX	H
	XRA	A
	CALL	FILLMEM
	LXI	H,OVLSTART
	SHLD	DIRTBL
;
;START DIRECTORY SEARCH
	LXI	H,0
	SHLD	FILECT
	CALL	CLEARFCB
	LXI	H,FCB1+1
	MVI	B,11
	MVI	A,'?'
	CALL	FILLMEM
	LXI	D,FCB1
	MVI	C,SRCH
NEXTNAME:
	CALL	BDOS
	cpi	255
	JZ	CALCFREE
	PUSH	PSW	;PUSH A
	LXI	H,FILECT
	INR	M
	LDA	FIVE
	DCR	A
	JNZ	STA5
	MVI	A,5
STA5	STA	FIVE
	MOV	E,A
	MVI	D,0
	LXI	H,COLCT
	DAD	D
	INR	M
	POP	PSW
;A * 32 + 1
	RLC		;Rotate A five times.
	RLC		; ADD A five times also works.
	RLC
	RLC
	RLC
	INR	A
	MVI	B,0	;CLEAR B
	MOV	C,A	;DMA BYTE COUNT IN BC
	LXI	H,DMA1	;DMA ADDRESS IN HL
	DAD	B	;ADD BC+LH = FIRST BYTE OF FILENAME
	XCHG
	LHLD	DIRTBL
	MVI	M,0
	INX	H
	XCHG
	LXI	B,11
	DB	0EDH,0B0H	;Z80 (HL) -> (DE), BC COUNT
	XCHG
	SHLD	DIRTBL
	MVI	C,SRCHMOR
	JMP	NEXTNAME
;
CALCFREE:	;CALCULATE FREE SPACE
	MVI	C,PARAMS
	CALL	BDOS
	INX	H
	INX	H
	MOV	A,M
	STA	ABYTE
	INX	H
	INX	H
	INX	H
	MOV	E,M
	INX	H
	MOV	D,M
	PUSH	D
	MVI	C,ALLOC
	CALL	BDOS
	XCHG
	POP	H
	INX	H
	LXI	B,0
LOOP1:
	PUSH	D
	LDAX	D
	MVI	E,8
LOOP2:
	RAL
	JC	JMP1
	INX	B
JMP1:
	MOV	D,A
	DCX	H
	MOV	A,L
	ORA	H
	JZ	JMP2
	MOV	A,D
	DCR	E
	JNZ	LOOP2
	POP	D
	INX	D
	JMP	LOOP1
JMP2:
	POP	D
	MOV	L,C
	MOV	H,B
	LDA	ABYTE
	SUI	3
	JZ	SORTNAMES
LOOP3:
	DAD	H
	DCR	A
	JNZ	LOOP3
;
SORTNAMES:
	SHLD	FREESIZE
	LHLD	DIRTBL
	INX	H
	SHLD	SORTBL
	SHLD	SORPTR
	LDA	FILECT
	STA	FILECTR
	ana	a
	jz	PSIZE
;
NEXTSORT:
	LXI	H,OVLSTART-12
	LXI	D,12
	LDA	FILECT
	INR	A
	MOV	B,A
	XRA	A
SEEK1ST
	DCR	B
	DAD	D
	CMP	M
	JNZ	SEEK1ST
	SHLD	LOWSTRING
SEEK2ND
	DCR	B
	JZ	LDTBL
	DAD	D
	PUSH	H	;SAVE TEST ADR
	PUSH	D	;SAVE 12
	XCHG
	LHLD	LOWSTRING
CMPLOOP:
	LDAX	D
	CMP	M
	JNZ	OKCMP
	INX	H
	INX	D
	JMP	CMPLOOP
OKCMP:
	POP	D
	POP	H
	JNC	SEEK2ND	;HL$ < DE$
	SHLD	LOWSTRING ;DE$ < HL$
	JMP	SEEK2ND
LDTBL:
	LHLD	LOWSTRING
	MVI	M,0FFH
	INX	H
	XCHG
	LHLD	SORPTR
	MOV	M,E	! INX	H
	MOV	M,D	! INX	H
	SHLD	SORPTR
	LXI	H,FILECTR
	DCR	M
	JNZ	NEXTSORT
;
	call	type! db HOME,cline,0
	LDA	FILECT
	STA	FILECTR
PDIR0:
	MVI	A,21
	STA	LINECT
PDIR1:
	MVI	A,5
	STA	FIVE
	LHLD	SORTBL
PDIR2:
	SHLD	SORPTR
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A
	MVI	B,8
	CALL	PLINE
	MVI	A,'.'
	CALL	PCHAR
	MVI	B,3
	CALL	PLINE
	LXI	H,FILECTR
	DCR	M
	JZ	PSIZE
	LXI	H,FIVE
	mov	e,m
	mvi	d,0
	DCR	M
	JZ	NEXTLINE
	lxi	h,colct
	DAD	d
	MOV	E,M
	mvi	d,0
	LHLD	SORPTR
	DAD D!	DAD D
	push	h
	CALL	TYPE! DB ' | ',0
	pop	h
	JMP	PDIR2
NEXTLINE:
	call	type! db cr,lf,CLINE,0
	LHLD	SORTBL
	INX H!  INX H
	SHLD	SORTBL
	LXI	H,LINECT
	DCR	M
	JNZ	PDIR1
	CALL	TYPE! DB 'Hit any key to continue.',0
	mvi	c,conin
	call	bdos
	CALL	TYPE! DB CR,CLINE,0
	JMP	PDIR0
;
PSIZE:
	CALL	type
	db cr,lf,cline,lf,esc,'=7 ',cline,up,cline
dirstat	db	'A: ',0
	LHLD	FILECT
	call	prnum
	CALL	TYPE! DB ' files;  ',0
	LHLD	FREESIZE
	CALL	PRNUM
	CALL	TYPE
	DB	'K free disk space',0
;
	CALL	LDPWDIROVL
	MVI	C,OPENF
	CALL	BDOS
	LXI	H,RECSTART
	SHLD	FCB1+CPMREC
	MVI	B,32
	MVI	C,SETDMA
	LXI	D,OVLSTART
RESTORE:
	PUSH B! PUSH D
	CALL	BDOS
	LXI	D,FCB1
	MVI	C,READRND
	CALL	BDOS
	POP D! POP B
	ANA	A
	JNZ	NOPE
	LHLD	FCB1+CPMREC
	INX	H
	SHLD	FCB1+CPMREC
	LXI	H,128
	DAD	D
	XCHG
	DCR	B
	JNZ	RESTORE
;
EXITDIR
	LDA	OLDSEL
	MOV	E,A
	MVI	C,SELDSK
	CALL	BDOS
	POP	B
	RET
;
ABORT:	CALL	TYPE ! DB CR,CLINE,0
	POP	B
	RET
;
NOPE	CALL	TYPE
	DB	7,esc,'=7 BAD TROUBLE! Unable to read A:PW.COM',7,0
	POP	B
	RET
;
;----------------------------------------------------
;subroutines
;
LDPWDIROVL
	CALL	CLEARFCB
	LXI	H,PWDIROVL
	LXI	D,FCB1
	LXI	B,12
	DB	0EDH,0B0H	;Z80 COPY MEMORY CMD
	LXI	D,FCB1
	RET
;
clearfcb:
	xra	a
	mvi	b,fcblen
	lxi	h,fcb1
;fall into..
;
FILLMEM:
	MOV	M,A
	INX	H
	DCR	B
	JNZ	FILLMEM
	RET
;
UPCASE:	;MAKES BYTE IN A UPPERCASE
	ana	a	;clear carry flag
	CPI	60H	;IS IT lower case?
	RC		;no
	SUI	20H
	RET
;
PCHAR:	;PRINT CHARACTER IN A, save BC and HL
	push h	! push b
	MVI	C,CONOUT
	MOV	E,A
	CALL	BDOS
	pop b ! POP H
	RET
;
TYPE:	;type text that immediately follows the call to this function.
	XTHL		;= POP H (stack had next program address=text adr)
TYPELOOP:
	MOV	A,M
	ANA	A
	JZ	EXITYPE
	CALL	PCHAR	;Be sure HL is preserved
	INX	H
	JMP 	TYPELOOP
EXITYPE:
	INX	H
	XTHL		;= push h
	RET
;
PLINE:	;PRINT (HL) B COUNT
	MOV	A,M
	CALL	PCHAR
	INX	H
	DCR	B
	JNZ	PLINE
	RET
;
;
;****** ARITHMETIC ROUTINES *******
;
CMPDH:  ;IF DE=HL set ZERO FLAG, if HL<DE set carry flag
	MOV	A,H
	CMP	D
	RNZ
	MOV	A,L
	CMP	E
	RET
;
hsubd:	;HL=HL-DE
	ana	a	;clear carry bit
	DB	0EDH,52H	;Z-80 instr: HL=HL=DE
	RET
;
PRNUM:	;print non-negative integer in HL
	xchg
	xra	a
	lxi	h,s10000
	mov	m,a	;10000
	inx	h
	mov	m,a	;1000
	inx	h
	mov	m,a	;100
	inx	h
	mov	m,a	;10
	inx	h
	mov	m,a	;1
	inx	h
	mov	m,a	;zero flag
	lxi	b,s10000
;setup done, now convert binary to decimal
	lxi	h,10000
	call	btod
	lxi	h,1000
	call	btod
	lxi	h,100
	call	btod
	lxi	h,10
	call	btod
	lxi	h,1
	call	btod
;done converting the number from binary to decimal, now print it...
	lda	s10000
	call	prdig
	lda	s1000
	call	prdig
	call	prcomma
	lda	s100
	call	prdig
	lda	s10
	call	prdig
	mvi	a,1
	sta	zero
	lda	s1
	call	prdig
	ret
;
;
;binary to decimal conversion.  de=binary number
;hl=binary equiv. of decimal factor,  bc=decimal digit adr.
BTOD:
	XCHG
	call	cmpdh
	XCHG
	jc	nextd	;done if binary is less than dec factor
	XCHG
	CALL	HSUBD
	xchg
	ldax	b
	inr	a
	stax	b
	jmp	btod
nextd:	inx	b
	ret
;
prdig	;print the digit in a
	mov	b,a
	ana	a
	jnz	adasc
	lda	zero
	ana	a
	rz
adasc:	mvi	a,1
	sta	zero
	mvi	a,'0'
	add	b
	call	pchar
	ret
;
prcomma:
	lda	zero
	ana	a
	rz
	mvi	a,','
	call	pchar
	ret
;
;	VARIABLE & STRING AREA
;
s10000:	ds	1	;decimal storage
s1000:	ds	1
s100:	ds	1
s10:	ds	1
s1:	ds	1
zero	ds	1	;leading zero flag
;
;
;
FIVE:	DB	1	;counts 5 file names across screen
;
OLDSEL: 	DS	1	;CURRENT DEFAULT DISK
NEWSEL: 	DS	1	;DISK TO CHECK
FILECT		Dw	0	;FILE COUNT
FILECTR		Dw	0	;FILE COUNT (print)
DIRTBL		DS	2
FREESIZE	DS	2
ABYTE:		DS	1
;
LOWSTRING	DS	2
LINECT		DS	1
COLCT		DS	1
COL5		DS	1
COL4		DS	1
COL3		DS	1
COL2		DS	1
COL1		DS	1
SORTBL		DS	2
SORPTR		DS	2
;
PWDIROVL	db	1,'PW      COM'

