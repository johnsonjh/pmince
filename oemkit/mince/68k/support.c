/* SUPPORT.C - supporting functions for Mince

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
	Created for version two  9/16/80  JTL
	Updated to version three  1/7/80  JTL

	This file contains several useful subroutines that can be
generally used. */

#include "mince.gbl"

ArgEcho(targ)		/* internal routine to display current argument */
	int targ;
{
	int trow, tcol;

#ifdef SUSER
	if (targ==4)
		for (cnt=terminal.delaycnt*terminal.mhz; cnt>0; --cnt)
			if (TKbRdy()) return(FALSE);
#endif
	trow=TGetRow();
	tcol=TGetCol();
	Echo("Arg: ");
	itot(targ);
	TCLEOL();
	TSetPoint(trow,tcol);
	TForce();
	return(TRUE);
	}

Ask(mesg)				/* Ask a yes/no question in the echo line */
	char *mesg;		/* Space or y or Y mean yes, anything else is no */
{
	char tchar;
	int yes;

	Echo(mesg);
	while (TRUE) {
		if (yes = ((KbWait(),tchar=TGetKb())==' ' || toupper(tchar)=='Y'))
			break;
		if (tchar == BELL || tchar == DEL || tchar == BS ||
			toupper(tchar)=='N') break;
		TBell();
		}
	ClrEcho();
	return (yes);
	}

BlockMove(from,to)		/* move a block of characters between Point and
					   From to before To.  Assume Point is before From */
	int from, to;
{
	int i;

	while (BIsBefore(from)) {
		i=Buff();
		BSwapPnt(to);
		BInsert(i);
		BSwapPnt(to);
		BDelete(1);
		}
	}

change(old,new)		/* change old string into new */
	char *old, *new;
{
	BMove(-strlen(old));
	BDelete(strlen(old));
	while (*new) BInsert(*new++);
	}

CheckMode(tmname)		/* see if there is a mode of this name */
	char *tmname;
{
	LowCase(tmname);
#ifdef LARGE
	if (!strcmp(tmname,"c")) return('c');
	if (!strcmp(tmname,"save")) return('s');
#endif
	if (!strcmp(tmname,"fill")) return('f');
	if (!strcmp(tmname,"page")) return('p');
	return(FALSE);
	}

ClrEcho()				/* Clear the echo line */
{
	int trow, tcol;

	trow=TGetRow();
	tcol=TGetCol();
	TSetPoint(TMaxRow()-1,0);
	TCLEOL();
	TSetPoint(trow,tcol);
	TForce();
	}

CopyToMrk(tmpmark,forwdp)	/* copy point to mark to delete buffer */
	int tmpmark, forwdp;
{
	int ltmark;

	BSwitchTo(del_buff);
	if (!DelCmnd(lfunct)) {
		BToEnd();
		ltmark=BCreMrk();
		BToStart();
		BDelToMrk(ltmark);
		BKillMrk(ltmark);
		}
	if (forwdp) BToEnd();
	else BToStart();
	BSwitchTo(buffs[cbuff].bbuff);
	BCopyRgn(tmpmark,del_buff);
	}

Debug(message,value)	/* useful for debugging commands */
	char *message;
	int value;
{
	char tchar;

	Echo(message);
	itot(value);
	IncrDsp();
	return((tchar=TGetKb())==DEL || tchar==BS);
	}

DelayPrompt(mesg)		/* echo mesg if no char is typed within interval */
	char *mesg;		/* return true if mesg printed, else false */
{
	int trow, tcol;

#ifdef SUSER
	for (cnt=terminal.delaycnt*terminal.mhz; cnt; --cnt)
		if (TKbRdy()) return (FALSE);
#endif
	trow=TGetRow();
	tcol=TGetCol();
	Echo(mesg);
	TSetPoint(trow,tcol);
	TForce();
	return(TRUE);
	}

DelCmnd(pfunct)		/* tell if previous command is a delete command */
	int (*pfunct)();
{
	int MDelLin(), MDelWord(), MRDelWord(), MMakeDel();
	int MDelRgn(), MCopyRgn(), MDelELin(), MDelSent();

	return(pfunct==MDelLin || pfunct==MDelWord ||
		pfunct==MRDelWord || pfunct==MMakeDel ||
		pfunct==MDelRgn || pfunct==MDelELin ||
		pfunct==MDelSent || pfunct==MCopyRgn);
	}

DoReplace(query)		/* do query replace and replace string */
	int query;
{
	char tchar;
	int exit;
	char old[STRMAX], new[STRMAX];

	arg=0;
	*old = *new = '\0';
	do if (!GetArg(query ? "Query Replace <ESC>: " : "Replace <ESC>: ",ESC,
			old,STRMAX)) return;
		while (*old=='\0');
	if (!GetArg("with <ESC>: ",ESC,new,STRMAX)) return;
	tmark=BCreMrk();
	if (query) tchar=',';
	else tchar=' ';
	exit=FALSE;

	while (!exit && StrSrch(old,FORWARD)) {
		if (query) {
			if (tchar==',') {
				Echo("Replacing '");
				NLPrnt(old);
				TPrntStr("' with '");
				NLPrnt(new);
				TPrntChar('\'');
				}
			IncrDsp();
			KbWait();
			tchar=TGetKb();
			switch (tchar) {
			case BELL: 
				ClrEcho();
				BKillMrk(tmark);
				return;
			case ' ':
			case ',':			/* comma's turn comes later... */
			case 'Y':
			case 'y':
				break;
			case '!':
				query=FALSE;
				break;
			case '.':
				exit=TRUE;
				continue;
			default:
				continue;
				}
			}
		change(old,new);
		if (tchar==',') {
			IncrDsp();
			if (!Ask("Confirm Replace?")) change(new,old);
			}
		}
	ClrEcho();
	BPntToMrk(tmark);
	BKillMrk(tmark);
	}

Echo(mesg)			/* Say something useful in the echo line */
	char *mesg;
{
	TSetPoint(TMaxRow()-1,0);
	TCLEOL();					/* clear whatever was there */
	TPrntStr(mesg);			/* display the message */
	TForce();
	}

Error(mesg)			/* Display an error message */
	char *mesg;
{
	int trow, tcol;

	trow=TGetRow();
	tcol=TGetCol();
	TDisStr(TMaxRow()-1,TMaxCol()-25,mesg);	/* Display the message */
	TBell();							/* Ring the terminal bell */
	TForce();
	KbWait();
	TSetPoint(TMaxRow()-1,TMaxCol()-25);
	TCLEOL();
	TSetPoint(trow,tcol);
	TForce();
	}

ForceCol(col,forwardp)		/* force the column to be col */
	int col, forwardp;
{
	BMakeCol(col);
	if (col<=0) return;
	if (BGetCol()>col && (BMove(-1), Buff()!=TAB)) BMove(1);
	SIndent(col-BGetCol());
	if (BGetCol()>col && !forwardp) BMove(-1);
	}

GetArg(mesg,term,str,len)	/* input a string argument */
	char *mesg, term, *str;
	int len;
{
	char stemp[80], inpt, *nstr, tcol;
	char *RubOut();

	TDisStr(TMaxRow()-1,0,mesg);
	TCLEOL();
	tcol=TGetCol();
	TForce();
	nstr=stemp;
	while ((KbWait(),inpt=TGetKb())!=term) {
		if (inpt==BELL) {
			ClrEcho();
			return(FALSE);
			}
		if (nstr-stemp>=len-1 && inpt!=DEL && inpt!=BS) {
			nstr=RubOut(stemp,nstr,tcol);
			TBell();
			}
		switch (inpt) {
			case CR:
				TPrntStr("<NL>");
				*nstr++=NL;
				break;
			case BS:
			case DEL:
				if (nstr>stemp) nstr=RubOut(stemp,nstr,tcol);
				break;
			case '\21':			/* ^Q */
				KbWait();
				inpt=TGetKb();
			default:
				TPrntChar(inpt);
				*nstr++=inpt;
				break;
			}
		TForce();
		}
	if (nstr!=stemp) {
		*nstr='\0';
		strcpy(str,stemp);
		}
	else NLPrnt(str);
	TSetPoint(TMaxRow()-1,0);
	TForce();
	return(TRUE);
	}

GetModeId(msg)			/* read in and check a mode id */
	char *msg;
{
	char *tmname[STRMAX];
	int id;

	*tmname='\0';
	if (!GetArg(msg,CR,tmname,STRMAX)) return(FALSE);
	if (id=CheckMode(tmname)) return(id);
	Error("Unknown Mode");
	return(FALSE);
	}

SIndent(targ)			/* put in the right number of spaces */
	int targ;
{
#ifndef CPM
	register int cnt;
#endif

	for (cnt=targ; cnt>0; --cnt) {
		BInsert(' ');
		TKbChk();
		}
	}

index(tstr,tchar)			/* locate character in string */
	char *tstr, tchar;
{
	int cntr;

	for (cntr=0; *tstr; ++cntr) if (*tstr++==tchar) return(cntr);
	return(-1);
	}


IsClose()				/* do we have a closing character */
{
	return(Buff()==')' || Buff()==']' || Buff()=='}' ||
		Buff()=='"' || Buff()=='\'');
	}

IsGray()				/* tell if current char is white space */
{
	return(IsWhite() || (Buff()&255)==NL);
	}

IsNL()				/* check for Newline */
{
	return((Buff()&255)==NL);
	}

IsNLPunct()			/* check for Newline or punctuation */
{
	return(IsNL() || Buff()=='.' || Buff()=='?' || Buff()=='!');
	}


IsParaEnd()			/* check for end of paragraph*/
{
	return(BIsEnd() || Buff()==TAB || IsNL() || Buff()=='@' ||
		Buff()=='.');
	}

IsSentEnd()			/* check for end of sentence.  you begin
			at, for example, a '.'.  it skips over as many of
			')}]"' or "'" as it finds, and tells you whether you
			wind up at a whitespace character */
{
	int IsClose();

	BMove(1);
	MovePast(IsClose,FORWARD);
	return(BIsEnd() || IsGray());
	}		

IsToken()				/* tell if current char is part of a token */
{
	return(isalpha(Buff()) || isdigit(Buff()));
	}

IsWhite()			/* tell if current char is limited white space */
{
	return(Buff()==' ' || Buff()==TAB);
	}

itot(n)					/* print a number on the terminal */
	unsigned n;
{
	if (n>9) itot(n/10);
	TPrntChar(n%10+'0');
	}

KbWait()				/* wait for a character to be typed */
{
#ifdef SUSER
	unsigned cntr;

	for (cntr=15*terminal.delaycnt*terminal.mhz; cntr>0; --cntr)
		if (TKbRdy()) return;
	while (!TKbRdy()) BFlush();
#else
	TKbWait();
#endif
	}

KillToMrk(tmpmark,forwdp)	/* delete region directioned */
	int tmpmark, forwdp;
{
	CopyToMrk(tmpmark,forwdp);
	BDelToMrk(tmpmark);
	}

LowCase(str)				/* convert a string to all lower case */
	char *str;
{
	while (*str=tolower(*str)) ++str;
	}

#ifndef CPM
ltot(n)				/* print a number on the terminal */
	long n;
{
	if (n>9) ltot(n/10);
	TPrntChar((char)(n%10)+'0');
	}
#endif

ModeFlags()			/* Display the mode flags */
{
#ifdef CPM
	unsigned loc, len;
#else
	long loc, len;
#endif

	if (TKbRdy()) return;
	TDisStr(TMaxRow()-2,stat_col," -");
	loc=BLocation();
	len=BLength(buffs[cbuff].bbuff);
	if (len==0) len=1;
#ifdef CPM
	if (loc<655) itot((loc*100)/len);
	else if (loc<6550) itot((loc*10)/(len/10+1));
	else itot(loc/(len/100+1));
#else
	itot((int)((loc*100)/len));
#endif
	TPrntStr("%- ");
	if (TKbRdy()) return;
	TPrntChar(BModp(buffs[cbuff].bbuff) ? '*' : ' ');
	TPrntChar(DelCmnd(lfunct) ? '+' : ' ');
	TCLEOL();
	}

MovePast(pred,forwdp)	/* go forward or back past a thingy */
	int (*pred)(), forwdp;
{
	if (!forwdp) BMove(-1);
	while (!(forwdp?BIsEnd():BIsStart()) && (*pred)()) {
		BMove(forwdp?1:-1);
		TKbChk();
		}
	if (!forwdp && !BIsStart()) BMove(1);
	}

MoveTo(pred,forwdp)		/* go forward or back to a thingy */
	int (*pred)(), forwdp;
{
	if (!forwdp) BMove(-1);
	while (!(forwdp?BIsEnd():BIsStart()) && !(*pred)()) {
		BMove(forwdp?1:-1);
		TKbChk();
		}
	if (!forwdp && !BIsStart()) BMove(1);
	}

NLPrnt(str)			/* print a string with "<NL>"'s */
	char *str;
{
	while (*str)
		if (((*str)&255)!=NL) TPrntChar(*str++);
		else {
			TPrntStr("<NL>");
			++str;
			}
	TForce();
	}

NLSrch()				/* Search for the next new line combination */
{
	return(BCSearch(NL));
	}

Rebind(from,to)		/* change all references to From to To */
	int (*from)(), (*to)();
{
	for (cnt=383; cnt>=0; --cnt) {
		if (functs[cnt]==from) functs[cnt]=to;
		TKbChk();
		}
	}

RNLSrch()				/* Reverse search for the next new line */
{
	return(BCRSearch(NL));
	}

char *RubOut(ostr,str,tcol)	/* delete a character for getarg */
	char *str, *ostr, tcol;
{
	*--str='\0';
	TSetPoint(TGetRow(),tcol);
	NLPrnt(ostr);
	TCLEOL();
	TForce();
	return(str);
	}

strip(to,from)			/* get main part of file name */
	char *to, *from;
{
	int left, right;
	char *tptr;

#ifdef UNIX
	left=0;
	while (index(from+left,'/')>=0) left += index(from+left,'/')+1;
	right=index(from+left,'.');
	if (right<0) right=strlen(from);
	else right += left;
#else
	left=index(from,':')+1;
	right=index(from,'.');
	if (right<0) right=strlen(from);
	TKbChk();
	if (left>=right) {
		left=0;
		right=strlen(from);
		}
#endif
	if (right-left>8) right=left+8;
	for (tptr=from+left; tptr<from+right; ++tptr) *to++ = *tptr;
	*to='\0';
	}

StrSrch(str,forwardp)	/* directioned string search */
	char *str;
	int forwardp;
{
	cnt=0;
	while (str[cnt]!='\0') {
		if (!(forwardp ? BCSearch(*str) : BCRSearch(*str))) break;
		if (!forwardp) BMove(1);
		for (cnt=1; !BIsEnd() && str[cnt]; ++cnt) {
			if (Buff()!=str[cnt] && tolower(Buff())!=str[cnt]) break;
			BMove(1);
			TKbChk();
			}
		if (!forwardp || str[cnt]!='\0') BMove((forwardp?1:0)-cnt);
		TKbChk();
		}
	return (str[cnt]=='\0');
	}

ToBegLine()			/* move to the beginning of the line */
{
	if (BCRSearch(NL)) BMove(1);
	}

ToEndLine()			/* move to the end of the line */
{
	if (BCSearch(NL)) BMove(-1);
	}

TIndent(targ)			/* put in the right number of tabs and spaces */
	int targ;
{
	for (cnt=targ; cnt>=tabincr; cnt-=tabincr) {
		BInsert(TAB);
		TKbChk();
		}
	SIndent(cnt);
	}

ToNotWhite(forwardp)	/* move to non whitespace */
	int forwardp;
{
	int IsGray();

	MovePast(IsGray,forwardp);
	}

ToSentEnd(forwardp)		/* move to a sentence end */
	int forwardp;
{
	int IsNLPunct();

	while (TRUE) {
		MoveTo(IsNLPunct,forwardp);
		if (!forwardp) BMove(-1);
		if (BIsEnd() || BIsStart()) return;
		if (IsNL()) {
			BMove(1);
			tmp=IsParaEnd();
			if (forwardp) BMove(-1);
			}
		else {
			tmark=BCreMrk();
			tmp=IsSentEnd();
			BPntToMrk(tmark);
			BKillMrk(tmark);
			}
		if (tmp) return;
		BMove(forwardp ? 1:-1);
		}
	}

ToWhite(forwardp)		/* Move to whitespace */
	int forwardp;
{
	int IsGray();

	MoveTo(IsGray,forwardp);
	}

ToWord()				/* move to the beginning of a word */
{
	int IsToken();

	MoveTo(IsToken,FORWARD);
	}

#ifdef CPM
UpCase(str)				/* convert a string to all upper case */
	char *str;
{
	while (*str=toupper(*str)) ++str;
	}
#endif

VMovCmnd(pfunct)		/* tell if previous command was a vertical move */
	int (*pfunct)();
{
	int MNextLin(), MPrevLin();
	return(pfunct==MNextLin || pfunct==MPrevLin);
	}

/* END OF SUPPORT.C - supporting routines for Mince */

