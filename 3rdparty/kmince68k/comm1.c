/* -*-c,save-*- */
/* COMM1.C	This is part one of the mince command set

The seller of this software hereby disclaim any and all
guarantees and warranties, both express and implied.  No
liability of any form shall be assumed by the seller, nor shall
direct, consequential, or other damages be assumed by the seller.
Any user of this software uses it at his or her own risk.

Due to the ill-defined nature of "fitness for purpose" or similar
types of guarantees for this type of product, no fitness for any
purpose whatsoever is claimed or implied.

The physical medium upon which the software is supplied is
guaranteed for one year against any physical defect.  If it
should fail, return it to the seller, and a new physical medium
with a copy of the purchased software shall be sent.

The seller reserve the right to make changes, additions, and
improvements to the software at any time; no guarantee is made
that future versions of the software will be compatible with any
other version.

The parts of this disclaimer are severable and fault found in any
one part does not invalidate any other parts.

	Copyright (c) 1981 by Mark of the Unicorn
	Created for version two  8/27/80  JTL
	Updated to version three  1/7/81  JTL

	This file contains the control character command execution
routines for the mince editor. There are also a small number of
subordinate routines. */

#include "mince.gbl"

MSetMrk()				/* set the user mark to the point */
{
	BMrkToPnt(mark);
	Echo("Mark Set");
	}

MBegLin()				/* move to the beginning of the current line */
{
	ToBegLine();
	}

MPrevChar()			/* move back a character */
{
	BMove(-arg);
	arg=0;
	}

MDelChar()			/* delete the current character */
{
	BDelete(arg);
	arg=0;
	}

MEndLin()				/* move to the end of the current line */
{
	ToEndLine();
	}

MNextChar()			/* move the point to the next character */
{
	BMove(arg);
	arg=0;
	}

MAbort()				/* abort the current operation */
{
	TBell();
	arg=0;
	}

MIndent()				/* Indent next line same as current */
{
	int IsWhite();

	BInsert(NL);
	BMove(-1);
	ToBegLine();
	MovePast(IsWhite,FORWARD);
	cnt=BGetCol();
	NLSrch();
	TIndent(cnt);
	}

MDelLin()				/* delete the current line */
{
	tmark=BCreMrk();
	if (arg>0) {
		if (!BIsEnd() && (Buff()&255)==NL) BMove(1);
		else ToEndLine();
		}
	else ToBegLine();
	KillToMrk(tmark,(arg>0));
	BKillMrk(tmark);
	}

MNewLin()				/* insert a newline */
{
	BInsert(NL);
	}

MNextLin()			/* move down one line */
{
	if (!VMovCmnd(lfunct)) lcol=BGetCol();
	while(arg-- > 0) NLSrch();
	BMakeCol(lcol);
	}

MOpenLin()			/* insert a newline after the current position */
{
	BInsert(NL);
	BMove(-1);
	}

MPrevLin()			/* move to the previous line */
{
	if (!VMovCmnd(lfunct)) lcol=BGetCol();
	while (arg-- > 0) RNLSrch();
	BMakeCol(lcol);
	}

MQuote()				/* quote next character */
{
	tmp=DelayPrompt("Quote:");
	KbWait();
	cnt=TGetKb();
	while (arg-- > 0) {
		BInsert(cnt);
		TKbChk();
		}
	if (tmp) ClrEcho();
	arg=0;
	}

MRSearch()			/* reverse string search */
{
	if (!GetArg("Reverse Search <ESC>: ",ESC,strarg,STRMAX)) {
		arg=0;
		return;
		}
	tmark=BCreMrk();
	while (arg-- > 0) if (!StrSrch(strarg,BACKWARD)) {
		BPntToMrk(tmark);
		Error("Not Found");
		break;
		}
	BKillMrk(tmark);
	arg=0;
	ClrEcho();
	}

MSearch()				/* forward string search */
{
	if (!GetArg("Forward Search <ESC>: ",ESC,strarg,STRMAX)) {
		arg=0;
		return;
		}
	tmark=BCreMrk();
	while (arg-- > 0) if (!StrSrch(strarg,FORWARD)) {
		BPntToMrk(tmark);
		Error("Not Found");
		break;
		}
	BKillMrk(tmark);
	arg=0;
	ClrEcho();
	}

MSwapChar()			/* swap two characters */
{
	if (BIsEnd() || (Buff()&255)==NL) BMove(-1);
	if (!BIsStart()) BMove(-1);
	tmp=Buff();
	BDelete(1);
	BMove(1);
	BInsert(tmp);
	}

MArg()				/* set up arguments */
{
	int tchar, eflag, cflag;

	tmp=0;
	cflag=FALSE;
	arg *= 4;
	eflag=ArgEcho(arg);
	while ((tchar=TGetKb())>='0' && tchar<='9')  {
		tmp=tmp*10+tchar-'0';
		cflag=TRUE;
		eflag |= ArgEcho(tmp);
		}
	if (cflag) arg=tmp;
	if (eflag) ClrEcho();
	cmnd=tchar;
	argp=TRUE;
	(*functs[cmnd])();
	}

MNextPage()			/* move down a page */
{
	BPntToMrk(sstart);
	for (cnt=WHeight()+PrefLine()-2; cnt>0; --cnt) NLSrch();
	ScrnRange();
	}

MDelRgn()				/* delete from the point to the mark */
{
	KillToMrk(mark,BIsBeforeMrk(mark));
	}

MCtrlX()				/* process cntrl-x commands */
{
	tmp=DelayPrompt("Control-X:");
	KbWait();
	cmnd=(TGetKb() & ~128)+256;
	if (tmp) ClrEcho();
	(*functs[cmnd])();
	}

MYank()				/* Insert the delete buffer at the point */
{
	BMrkToPnt(mark);
	BSwitchTo(del_buff);
	BToEnd();
	tmark=BCreMrk();
	BToStart();
	BCopyRgn(tmark,buffs[cbuff].bbuff);
	BKillMrk(tmark);
	BSwitchTo(buffs[cbuff].bbuff);
	}

MMeta()				/* Process Meta commands */
{
	tmp=DelayPrompt("Meta:");
	KbWait();
	cmnd=TGetKb()|128;
	if (tmp) ClrEcho();
	(*functs[cmnd])();
	}

MDelIndent()			/* delete the indentation from this line */
{
	tmark=BCreMrk();
	ToBegLine();
	MDelWhite();
	BPntToMrk(tmark);
	BKillMrk(tmark);
	arg=0;
	}

MInsert()				/* self inserting commands */
{
#ifdef ELECTRIC
	if (!isalpha(cmnd & 0x7F)) EDoAbrev(cmnd);
	else BInsert(cmnd & 0x7F);
#else
	BInsert(cmnd & 0x7F);
#endif
	}

MRDelChar()			/* delete the previous character */
{
	tmark=BCreMrk();
	BMove(-arg);
	BDelToMrk(tmark);
	BKillMrk(tmark);
	arg=0;
	}

	/* Page mode commands */
MOverwrite()			/* overwrite instead of inserting */
{
	if (!BIsEnd() && !IsNL()) {
		MPFChar();
		BMove(-1);
		BDelete(1);
		}
	BInsert(cmnd);
	}

MPBegLine()			/* move to the first non-white character */
{
	int IsWhite();

	ToBegLine();
	MovePast(IsWhite,FORWARD);
	if (IsNL()) MDelWhite();
	}

MPBChar()				/* move to the left one character */
{
	ForceCol(BGetCol()-1,BACKWARD);
	}

MPFChar()				/* move to the right one character */
{
	ForceCol(BGetCol()+1,FORWARD);
	}

MPEndLine()			/* go to end of line less white space */
{
	ToEndLine();
	MDelWhite();
	}

MPBackSpace()			/* rub out previous character */
{
	if (BGetCol()==0) return;
	MPBChar();
	if (!BIsEnd() && !IsNL()) {
		MPFChar();
		BMove(-1);
		BDelete(1);
		}
	BInsert(' ');
	BMove(-1);
	}
	
MPTab()				/* move over a tab stop */
{
	ForceCol(BGetCol()+TWidth(BGetCol(),TAB),FORWARD);
	}

MPNextLine()			/* move to the next line in exact same column */
{
	MNextLin();
	ForceCol(lcol,FORWARD);
	}

MPPrevLine()			/* move to previous column in same column */
{
	MPrevLin();
	ForceCol(lcol,FORWARD);
	}

#ifdef LARGE
	/* C Mode commands */
MCIndent()			/* Indent for C code */
{
	int IsWhite(), wid;

	tmark=BCreMrk();
	ToBegLine();
	MovePast(IsWhite,FORWARD);
	if (BIsAfter(tmark)) BPntToMrk();
	wid=BGetCol();
	while (BIsBefore(tmark)) {
		if (Buff()=='{') wid += indentcol;
		else if (Buff()=='}') wid -= indentcol;
		BMove(1);
		TKbChk();
		}
	BPntToMrk(tmark);
	BKillMrk(tmark);
	BInsert(NL);
	TIndent(wid);
	}

	/* Auto save mode */
MSInsert()			/* Auto save self insert */
{
	static unsigned in_cnt;

#ifdef ELECTRIC
	if (!isalpha(cmnd & 0x7F)) EDoAbrev(cmnd);
	else BInsert(cmnd & 0x7F);
#else
	BInsert(cmnd & 0x7F);
#endif
	if (++in_cnt>256) {
		MFileSave();
		in_cnt=0;
		}
	}
#endif

/* END OF COMM1.C -Mince command routines */

