/* -*-C,save-*- */
/******************************************************************
 *	KEYPAD.C - KMince code (adapted from Keymacs, ((C) copyright by Andrew
 *		   S. Cromarty, 1982), by Robert Heller)
 *
 *-------------------------------------------------------------------------
 *
 *	(C) copyright 1984, by Robert Heller.  This document may be copied by
 *	anyone for any non-profit purpose as long as the following 2 conditions
 *	are observed:
 *	1.  Partial reproduction or duplication of the Keypad Editor for MINCE
 *	    is strictly forbidden.  DOCUMENTATION IS INCLUDED IN THIS
 *	    RESTRICTION.  Only the complete package may be copied, stored,
 *	    reproduced, or transmitted.
 *	2.  This copyright notice MUST accompany any and all parts of said
 *	    package as is exists in this document.  Any modification of this
 *	    copyright notice shall constitute violation of copyright.
 *
 *-------------------------------------------------------------------------
 *
 * This code makes use of the "numeric keypad" which many popular smart 
 * terminals (such as H/Z-19's, Z29's, VT1xx's, VK100's, etc.) have.  This 
 * keypad is used to control movement, delete, and search functions in a
 */
#include "mince.gbl"
/* only ONE of the following defines should be enabled */
/*	#define H19	1	*/	/* Heath/Zenith H-/Z-19 */
#define Z29	1		/* Heath/Zenith H-/Z-29 */
/*	#define VT100	1	*/	/* DEC ANSI-type terminals: VT1xx series*/
/*	#define VT52	1	*/	/* VT52 type terminals (esp. InfoTon) */
/*	#define VI50	1	*/	/* Visual 50 terminal */
KeyPad()
{
	int MPrevChar(), MPrevLin(), MNextChar();
	int BWord(), MPrevPage(), FWord();
	int MBegLin(), MNextLin(), MEndLin();
	int MRSearch(), MNextPage(), MSearch();

	int MRDelChar(), MDelRgn(), MDelChar();
	int MRDelWord(), MSetMrk(), MDelWord();
	int MRDelLin(), MYank(), MDelLin();

	int MNotImpl(), MQryRplc(), MReplace();

	int MKeyPad();

#ifdef H19
#include "H19keys.c"
#endif
#ifdef Z29
#include "Z29Keys.C"
#endif
#ifdef VT52
#include "VT52Keys.C"
#endif
#ifdef VI50
#include "VI50Keys.C"
#endif
#ifdef VT100
#include "VT100Key.C"
#endif
	functs[256+('K'-64)] = MRDelLin;
}
MKeyPad()
{
	int ch;
	int (*funct)();

	int MPrevChar(), MPrevLin(), MNextChar();
	int BWord(), MPrevPage(), FWord();
	int MBegLin(), MNextLin(), MEndLin();
	int MRSearch(), MNextPage(), MSearch();
	int MNotImpl();
	int MReplace(), MQryRplc();
#ifdef H19
#else
	int MDKeyPad();
#endif

	ch = TGetKb();
	switch (ch) {
	case 'p' : funct = MRSearch; break;	/* 0-key */
	case 'n' : funct = MNextPage; break;	/* .-key */
	case 'M' : funct = MSearch; break;	/* enter-key */
	case 'q' : funct = MBegLin; break;	/* 1-key */
	case 'r' : funct = MNextLin; break;	/* 2-key */
	case 's' : funct = MEndLin; break;	/* 3-key */
	case 't' : funct = BWord; break;	/* 4-key */
	case 'u' : funct = MPrevPage; break;	/* 5-key */
	case 'v' : funct = FWord; break;	/* 6-key */
	case 'w' : funct = MPrevChar; break;	/* 7-key */
	case 'x' : funct = MPrevLin; break;	/* 8-key */
	case 'y' : funct = MNextChar; break;	/* 9-key */
#ifdef H19
#else
	case 'l' : funct = MDKeyPad; break;	/* ,-key (Destructor shift) */
#endif
#ifdef VT100
	case 'Q' : funct = MQryRplc; break;	/* PF2-key */
	case 'R' : funct = MReplace; break;	/* PF3-key */
#endif
	default: funct = MNotImpl; break;
	}
	(*funct)();
	cmnd = fndkey(funct);
	lfunct = funct;
}
#ifdef VT100
MVT100Arw()
{
	int ch;
	int (*funct)();

	int MNextLin(), MPrevLin(), BWord(), FWord();
	int MNotImpl();

	ch = TGetKb();
	switch (ch) {
	case 'B' : funct = MNextLin; break;	/* M-O-B (Down arrow key) */
	case 'D' : funct = BWord; break;	/* M-O-D (Left arrow key) */
	case 'C' : funct = FWord; break;	/* M-O-C (Right arrow key) */
	case 'A' : funct = MPrevLin; break;	/* M-O-A (Up arrow key) */
	default : funct = MNotImpl; break;
	}
	(*funct)();
	cmnd = fndkey(funct);
	lfunct = funct;
}
#endif
#ifdef H19
#else
MDKeyPad()
{
	int ch;
	int (*funct)();

	int MRDelChar(), MDelRgn(), MDelChar();
	int MRDelWord(), MSetMrk(), MDelWord();
	int MRDelLin(), MYank(), MDelLin();

	int MAbort(), MNotImpl();

	DelayPrompt("KeyPad destructor: ");
	while(!TKbRdy()) ;

	if (TGetKb() != ESC) {MNotImpl(); return;}
#ifdef VT100
	if (TGetKb() != 'O') {MNotImpl(); return;}
#else
	if (TGetKb() != '?') {MNotImpl(); return;}
#endif

	ch = TGetKb();
	switch (ch) {
	case 'q' : funct = MRDelLin; break;	/* 1-key */
	case 'r' : funct = MYank; break;	/* 2-key */
	case 's' : funct = MDelLin; break;	/* 3-key */
	case 't' : funct = MRDelWord; break;	/* 4-key */
	case 'u' : funct = MSetMrk; break;	/* 5-key */
	case 'v' : funct = MDelWord; break;	/* 6-key */
	case 'w' : funct = MRDelChar; break;	/* 7-key */
	case 'x' : funct = MDelRgn; break;	/* 8-key */
	case 'y' : funct = MDelChar; break;	/* 9-key */
	case 'm' : funct = MAbort; break;	/* dash-key (unshift) */
	default: funct = MNotImpl; break;
	}
	(*funct)();
	lfunct = funct;
	cmnd = fndkey(funct);
}
#endif
MRDelLin()				/* delete the current line (reversely) */
{
	tmark=BCreMrk();
	if (arg>0) {
		if (!BIsStart() && (prevch()&255)==NL) BMove(-1);
		else ToBegLine();
		}
	else ToEndLine();
	KillToMrk(tmark,!(arg>0));
	BKillMrk(tmark);
	}
static int prevch()
{int tempch;

	if (!BIsStart()) {BMove(-1); tempch = Buff(); BMove(1);}
	else tempch = 0;
	return(tempch);
}
static int fndkey(cmd)
int (*cmd)();
{int i;

	for (i=0;i<384;i++)
		if (cmd == functs[i]) return(i);
	return(cmnd);
}
/*****************************************************************
 *	WY-50 hacks.  Assumes that the function keys have be setup
 *	to xmit ESC-{ x, where x is:
 *	A for F1, B for F2, etc and a for shift-F1, b for shift-f2, etc.
 *
 *	Robert Heller
 *	last mod 17-aug-1985 (this section only)
 */

wy50_finit()
{
    int MWY50_Fn(),MRDelLin();

    functs[256+('K'-64)] = MRDelLin;
    functs[128+'{'] = MWY50_Fn;
    }
static MWY50_Fn()
{
    int ch;
    int (*funct)();

    int MPrevChar(), MPrevLin(), MNextChar();
    int BWord(), MPrevPage(), FWord();
    int MBegLin(), MNextLin(), MEndLin();
    int MRSearch(), MNextPage(), MSearch();
    int MNotImpl();
    int MReplace(), MQryRplc();
    int MRDelChar(), MDelRgn(), MDelChar();
    int MRDelWord(), MSetMrk(), MDelWord();
    int MRDelLin(), MYank(), MDelLin();

    int MAbort(), MNotImpl();

    ch = TGetKb();
    switch (ch) {
    case 'A' : funct = BWord; break;		/* F1 key */
    case 'B' : funct = FWord; break;		/* F2 key */
    case 'C' : funct = MNextLin; break;		/* F3 key */
    case 'D' : funct = MPrevLin; break;		/* F4 key */
    case 'a' : funct = BWord; break;		/* shift-F1 key */
    case 'b' : funct = FWord; break;		/* shift-F2 key */
    case 'c' : funct = MNextLin; break;		/* shift-F3 key */
    case 'd' : funct = MPrevLin; break;		/* shift-F4 key */
    case 'E' : funct = MPrevChar; break;	/* F5 key */
    case 'e' : funct = MRDelChar; break;	/* shift-F5 key */
    case 'F' : funct = MNextChar; break;	/* F6 key */
    case 'f' : funct = MDelChar; break;		/* shift-F6 key */
    case 'G' : funct = BWord; break;		/* F7 key */
    case 'g' : funct = MRDelWord; break;	/* shift-F7 key */
    case 'H' : funct = FWord; break;		/* F8 key */
    case 'h' : funct = MDelWord; break;		/* shift-F8 key */
    case 'I' : funct = MPrevLin; break;		/* F9 key */
    case 'i' : funct = MDelRgn; break;		/* shift-F9 key */
    case 'J' : funct = MNextLin; break;		/* F10 key */
    case 'j' : funct = MYank; break;		/* shift-F10 key */
    case 'K' : funct = MPrevPage; break;	/* F11 key */
    case 'k' : funct = MSetMrk; break;		/* shift-F11 key */
    case 'L' : funct = MNextPage; break;	/* F12 key */
    case 'l' : funct = MNextPage; break;	/* shift-F12 key */
    case 'M' : funct = MBegLin; break;		/* F13 key */
    case 'm' : funct = MRDelLin; break;		/* shift-F13 key */
    case 'N' : funct = MEndLin; break;		/* F14 key */
    case 'n' : funct = MDelLin; break;		/* shift-F14 key */
    case 'O' : funct = MRSearch; break;		/* F15 key */
    case 'o' : funct = MQryRplc; break;		/* shift-F15 key */
    case 'P' : funct = MSearch; break;		/* F16 key */
    case 'p' : funct = MReplace; break;		/* shift-F16 key */
    default : funct = MNotImpl; break;
    }
    (*funct)();
    lfunct = funct;
    cmnd = fndkey(funct);
    }

