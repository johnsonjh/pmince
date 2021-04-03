/* term.c	This is the low level terminal support package.

	Copyright 1980 by Jason T. Linhart
	Updated to version two 5/15/80 JTL

	This is the generalized terminal support package. It deals
with things like where the cursor is and how to get it to somewhere
new as well as fancy things like clear to end of line and clear to end
of screen. */

#include "mince.gbl"

TInit()				/* set up the keyboard input queue */
{
	int trow;

	QInit(&kbdq,KBBUFMAX);
	put_string(&terminal.init);
	for (trow=0; trow<terminal.nrows; ++trow) clrcol[trow]=terminal.ncols;
	srow=scol=255;			/* init to impossible values */
	TClrWind();
	}

TFini()				/* set terminal back to defaults */
{
	TForce();
	put_string(&terminal.deinit);
	}

TForce()				/* Position the cursor to the point */
{
	if (pcol>=terminal.ncols) {
		pcol=0;
		Error("Off Screen");
		}
	if (srow==prow && scol==pcol) return;
	put_string(&terminal.cpos1);
	put_coord(FIRST);
	put_string(&terminal.cpos2);
	put_coord(SECOND);
	put_string(&terminal.cpos3);
	put_pad(terminal.ncpospad);
	srow=prow;
	scol=pcol;
	}

TSetPoint(irow,icol)	/* Set the position of the active point */
	int irow, icol;
{
	prow=irow;
	pcol=icol;
	}

TGetRow()				/* Return the row the terminal is on */
{
	return(prow);
	}

TGetCol()				/* Return the column the terminal is on */
{
	return(pcol);
	}

TMaxRow()				/* Return the max # of rows on the terminal */
{
	return(terminal.nrows);
	}

TMaxCol()				/* Return the max # of columns on the terminal */
{
	return(terminal.ncols);
	}

TBell()				/* Ring the terminal bell */
{
	put_string(&terminal.bell);
	}

TPrntChar(ichar)		/* Print a character */
	char ichar;
{
	int tcol;

	if (ichar>=' ' && ichar<='~') {
		TForce();
		TPutChar(ichar);
		++scol;
		++pcol;
		if (clrcol[prow]<pcol) clrcol[prow]=pcol;
		}
	else switch(ichar) {
		case '\t':
			tcol=tabincr-(pcol%tabincr);
			while (tcol) {
				TPrntChar(' ');
				--tcol;
				}
			break;
		case NL:
			pcol=0;
			if (prow<terminal.nrows-1) ++prow;
			else prow=0;
			TCLEOL();
			break;
		default:
			if (ichar&128) {
				TPrntChar('~');
				TPrntChar(ichar & ~128);
				}
			else {
				TPrntChar('^');
				TPrntChar(ichar^'@');
				}
			break;
		}
	}

TWidth(colcnt,tchar)	/* determine width of tchar */
	int colcnt;
	char tchar;
{
	int wid, delta;

	if (tchar>=' ' && tchar<='~') return(1);
	switch(tchar) {

	case NL:
		return(-colcnt);
	case TAB:
		colcnt %= terminal.ncols;
		wid=tabincr-(colcnt%tabincr);
		delta=terminal.ncols-colcnt;
		if (delta<wid) wid=delta+tabincr;
		break;
	default:
		if (tchar&128) wid = TWidth(colcnt+1,tchar & ~128)+1;
		else wid=2;
		delta=terminal.ncols-(colcnt%terminal.ncols);
		if (delta<wid) wid+=delta;
		break;
		}
	return(wid);
	}

TCLEOL()				/* Clear to end of line */
{
	int tcol;

	if (pcol>=clrcol[prow]) return;
	TForce();
	if (terminal.cleol.len) {
		put_string(&terminal.cleol);
		put_pad(terminal.ncleolpad);
		}
	else for (tcol=pcol; tcol<clrcol[prow]; ++tcol) {
		TPutChar(' ');
		++scol;
		TKbChk();
		}
	clrcol[prow]=pcol;
	}

TClrLine()			/* Goto the beginning of the line and clear it */
{
	pcol=0;
	TCLEOL();
	}

TCLEOW()				/* Clear to end of window */
{
	int trow, tcol;

	if (terminal.cleow.len) {
		TForce();
		put_string(&terminal.cleow);
		put_pad(terminal.ncleowpad);
		}
	else {
		trow=prow;
		tcol=pcol;
		while (prow<terminal.nrows) {
			TCLEOL();
			++prow;
			pcol=0;
			}
		prow=trow;
		pcol=tcol;
		}
	}

TClrWind()			/* Home and clear window */
{
	int trow;

	prow=pcol=0;
	if (terminal.hcl.len) {
		put_string(&terminal.hcl);
		put_pad(terminal.nhclpad);
		for (trow=0; trow<terminal.nrows; ++trow) clrcol[trow]=0;
		srow=scol=0;
		}
	else TCLEOW();
	}

TPrntStr(string)		/* Print a string */
	char *string;
{
	while (*string) TPrntChar(*string++);
	}

TDisStr(row,col,string)	/* Display a string at row,col */
	int row, col;
	char *string;
{
	TSetPoint(row,col);
	while (*string) TPrntChar(*string++);
	}

TKbRdy()				/* Returns TRUE if input available */
{
	TKbChk();
	return (!QEmpty(&kbdq));
	}

TGetKb()				/* Returns an input character */
{
	while (QEmpty(&kbdq)) TKbChk();	/* Wait for input */
	return(QGrab(&kbdq));		/* Pass back the character */
	}

TKbChk()				/* Check for keyboard input and queue it */
{
	if (inport.biosp ? bios(2) : 
		((inp(inport.statport)&inport.readymask) == inport.polarity))
		if (!QFull(&kbdq)) QShove(inport.biosp ? bios(3) :
			(inp(inport.dataport)&inport.datamask), &kbdq);
		else TBell();
	}

TPutChar(ochar)		/* print a character on the terminal */
	char ochar;
{
	if (outport.biosp) bios(4,ochar);
	else {
		while ((inp(outport.statport)&outport.readymask) !=
			outport.polarity);
		outp(outport.dataport,ochar);
		}
	}

/* the following are internal routines */

put_string(sdef)		/* output a command string */
	struct str *sdef;
{
	int tlen;
	char *tptr;

	tptr=&terminal.strspc[sdef->idx];
	for (tlen=sdef->len; tlen; --tlen) TPutChar(*tptr++);
	}

put_coord(firstp)		/* output a cursor-positioning coordinate */
	int firstp;
{
	char location;

	if (firstp == terminal.rowfirstp) location=prow+terminal.rowbias;
	else location=pcol+terminal.colbias;

	if (terminal.binaryp) TPutChar(location^terminal.compp);
	else put_num(location);
	}


put_num(num)			/* output a number to the terminal */
	unsigned num;
{
	if (num>=10) put_num(num/10);
	TPutChar(num%10+'0');
	}

put_pad(npads)			/* do padding */
	int npads;
{
	int cnt;

	if (terminal.padp == CHARPAD) while (npads--) TPutChar(terminal.padchar);
	if (terminal.padp == DELAYPAD)
		while (npads--) for (cnt=mhz*8; cnt; --cnt);
	}

/* END OF TERM.C - terminal support code */

