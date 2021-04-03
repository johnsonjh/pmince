/* COMM5.C	Accumulated random Mince commands
			Contributed to AUG by Mark of the Unicorn 07/20/81

	Copyrignt (C) 1981 Mark of the Unicorn

This  file contains the source for assorted commands that for one 
reason or another (usually lack of code space) didn't make it into the
official Mince version. 

Note to potential users: some of this code belongs in places other
than in a COMMx.C file. (i.e. InsCmnd() belongs in Mince.) This means
that you will have to move them there with CLIB, and move the one that
is already there to another place. Alternately, put them at the end of
the CBIND.C file and put it on the link line first, but only if you
have added the extra globals to MINCE.GBL (as per Jason's
instructions). Be careful about little things like this, it may mean
the difference between code that works and code that does not. Also be
especially wary of the use of tmp, cnt, tmark, spare and other
'trashable' globals. If you call other functions that use these you
can get into trouble. It is probably always safer to redeclare these
at the beginning (and lose a little stack) than to risk trashing at
the hands of subsequent functions. Now that we can add globals, I
would reccommend NOT using spare anymore. -[BAD, 09/22/81] */

#include "mince.gbl"

/* M-O */

MDropLine()		/* drop rest of this line vertically */
{
	if (BIsEnd() || IsNL()){
		BInsert(NL);
		BMove(-1);
		return;
		}
	tmp=BGetCol();
	BInsert(NL);
	TIndent(tmp);
	RNLSrch();
	}

/* M-C-O */
MCloseWhite()		/* replace following greyspace with a blank */
{
	MDelWhite();
	while (!BisEnd() && IsNL()){
		BDelete(1);
		MDelWhite();
		}
	Binsert(' ');
	arg=0;
	}

/* The following are from Scott's Mince (MSCOTT.C) */

/* this M-T always swaps the previous two characters */
MSwapChar()
{
	BMove(-1);
	tmp=Buff();
	BDelete(1);
	BMove(-1);
	BInsert(tmp);
	BMove(1);
	}

/* this doesn't split the line; I keep it on C-J */
MIndNL()			/* Indent new line same as current */
{
	ToEndLine();
	MIndRest();
	}

/* This splits the line and indents; i keep it on M-C-J */
MIndRest()			/* move rest of line to indented new line */
{
	tmark=BCreMrk();
	cnt = GetInd();
	BPntToMrk(tmark);
	Binsert (NL);
	TIndent (cnt);
	BKillMrk(tmark);
	}

/*for convenience */ 
GetInd()				/* Get this line's indentation */
{
	int IsWhite();
	
	ToBegLine();
	MovePast(&IsWhite,FORWARD);
	return (BGetCol());
	}
	
/* This opens a new line before the current one, and gives it the 
same indentation; I keep it on M-O */
MOpenIndLin()			/* open line at current indentation */
{
	tmp = GetInd();
	ToBegLin();
	BInsert (NL);
	BMove(-1);
	TIndent (tmp);
	}
	

/* C-X > */
MIndntRgn()			/* indent the region */
{
	IndRgn (indentcol);
	}

/* C-X < */
MUndntRgn()			/* undent the region */
{
	IndRgn (-indentcol);
	}

/* Internal to the above */
IndRgn(amt)			/* add amt to the region's indentation */
{
	if (tmp = BIsAfterMrk (mark)) BSwapPnt (mark);
	tmark = BCreMrk();
	while (BIsBeforeMrk (mark)) {
		cnt = GetInd();
		MDelWhite();
		TIndent (cnt + amt);
		NLSrch();
		}
	BPntToMrk (tmark);
	if (tmp) BSwapPnt (mark);
	BKillMrk (tmark);
	}

/* experimental: if you're at the beginning of a word, it moves to 
the end of the previous word; else it moves to the beginning of 
the next word. Currently on M-Z.  */
MOthrTokn()			/* move to other non-token boundry */
{
	if (!BIsEnd() && !IsToken()) MoveTo (&IsToken, FORWARD);
	else MoveTo (&IsToken, BACKWARD);
	}

/* This special cases the end of the buffer, to always display "100%" there */
ModeFlags()			/* Display the mode flags */
{
	int tloc, tlen;

	if (TKbRdy()) return;
	TDisStr(TMaxRow()-2,stat_col," -");
	tloc=BLocation(buffs[cbuff].bbuff);
	tlen=BLength(buffs[cbuff].bbuff);
	if (tloc==tlen) itot(100);
	else if (tloc<325) itot((tloc*100)/tlen);
	else if (tloc<3250) itot((tloc*10)/(tlen/10+1));
	else itot (tloc/(tlen/100+1));
	TPrntStr("%- ");
	TPrntChar(BModp(buffs[cbuff].bbuff) ? '*' : ' ');
	TPrntChar(DelCmnd(lfunct) ? '+' : ' ');
	TCLEOL();
	}

/* if you want M-<digit> to work for arguments, put this on M-0 through
M-9, ... */
MArgDgt()				/* meta-digit arguments */
{
	arg=(cmnd&0x7f)-'0';
	NumArg(arg);
	}

/* ... and put this on C-U. */
MArg()				/* set up arguments */
{
	arg *= 4;
	NumArg(0);
	}
	
/* internal to the above. */
NumArg(tmp)			/* collect numeric arguments */
	int tmp;
{
	int tchar, eflag, cflag;

	cflag = FALSE;
	eflag = ArgEcho(arg);
	while ((tchar=(cmnd=TGetKb())&0x7F)>='0' && tchar<='9') {
		tmp=tmp*10+tchar-'0';
		cflag=TRUE;
		eflag |= ArgEcho(tmp);
		}
	if (cflag) arg=tmp;
	if (eflag) ClrEcho();
	argp=TRUE;
	(*functs[cmnd])();
	}

/* auto-fill with the following properties: it never just inserts
a NL (if you type spaces past the fill column it will just keep
inserting them), so that it never wraps unless there is actually a
word to wrap, and it only deletes one space before inserting the CR.
Used with Fill Para below, it insures that sentences will ALWAYS end 
with two spaces if you typed them that way (at the price of an 
occasional trailing space on a line). */
MFillChk()			/* check to see if we are beyond fillwidth
					   and do something about it */
{
	tmark=BCreMrk();
	tmp=FALSE;
	while (TRUE) {
		ToNotWhite(BACKWARD);
		if (BGetCol() <= fillwidth) break;
		tmp=TRUE;	
		ToWhite(BACKWARD);
		}
	if(tmp) {
		ToNotWhite(FORWARD);
		BMove(-1);
		BDelete(1);
		BInsert(NL);
		SIndent(indentcol);
		}
	BPntToMrk(tmark);
	BInsert((cmnd==CR) ? NL : cmnd);
	BKillMrk(tmark);
	}

/* Fill paragraph to go with the above. When it breaks a line,
it only deletes one space before inserting the Nl. */
MFillPara()			/* fill paragraph*/
{
	int IsWhite();
	
	if (argp) fillwidth=arg;
	arg=0;
	tmark=BCreMrk();
	BMove(-1);
	MFPara();
	BMove(-1);
	if (BIsEnd() || IsGray()) {
		BPntToMrk(tmark);
		BKillMrk(tmark);
		return;
		}
	BMove(1);
	tmp=BCreMrk();
	MBPara();
	
	While (BIsBefore(tmp)) {
		ToWhite(FORWARD);
		if (BGetCol() > fillwidth) {
			ToWhite(BACKWARD);
			BMove(-1);
			BDelete(1);
			BInsert(NL);
			SIndent(indentcol);
			ToWhite(FORWARD);
			}
		MovePast(&IsWhite,FORWARD);
		if (IsNL() && BIsBefore(tmp)) {
			BMove(1);
			MDelWhite();
			BMove(-1);
			BDelete(1);
			BInsert(' ');
			}
		}
	
	BPntToMrk(tmark);
	BKillMrk(tmark);
	BKillMrk(tmp);
	}

/* End of stuff from MSCOTT.C */

/* Electric shift lock functions
These consider ';' to begin a comment, and NL to end one (i.e. are designed
for assembler or most Lisps). They uppercase everything as it is typed,
unless it's in a comment. */

#define NORMAL 0
#define COMMENT 1
	int context;

/* this goes in mince */
InsCmnd(lfunct)		/* tell if previous command was self-insert */
	int(*lfunct)();
{
	int MInsert();
	return (lfunct == &MInsert);
	}

/* This goes in comm1 */
MInsert()				/* self inserting commands */
{
	int pmark;
	
	if (!InsCmnd(lfunct)) {
		context = NORMAL;
		pmark = BCreMrk();
		if (RNLSrch()) BMove(1);
		while (BIsBeforeMrk(pmark)) {
			if (Buff() == ';') {
				context = COMMENT;
				break;
				}
			BMove(1);
			}
		BPntToMrk(pmark);
		BKillMrk(pmark);
		}
	if (cmnd == ';') context = COMMENT;
	else if (cmnd == CR) context = NORMAL;
	if (context == NORMAL) BInsert(toupper(cmnd));
	else BInsert(cmnd);
	}

/* End of electric shift lock stuff */

/* END OF COMM5.C */	
