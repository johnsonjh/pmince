/* MACBIND.C - Customized BINDINGS.C

This contains MMakMacro and MDoMacro, which build and execute macros.
MMakMacro is bound to M-M and MDoMacro is bound to M-C-M. Macros are
saved in spare. Written by Barry A. Dobyns. 07/25/81.
Modifications Record:
		07/25/81	Original implementation in MINCE 2.6 completed. [BAD].

Also, This contains the MNewDsp and MRNewDsp, which take arguments.
The argument indicates how many lines to move down or up before doing
a NewDsp. Without an arg, they are equivalent to a single NewDsp. I
always seem to be doing a C-U nn C-N C-L or C-U nn C-P C-L and this
takes care of that for me in an elegant way. Written by Barry A.
Dobyns 07/22/81.
Modifications Record:
		07/22/81	Original implementation in MINCE 2.6 completed. [BAD].

This one has MIncSearch() and MIncRSearch() which implement
incremental searches in a way halfway between Mince's  non-incremental
MSearch and EMACS ^R Incremental Search. The MIncSearch gets the
search argument all at once, like MSearch, and unlike ^R Incremental Search
but leaves the point after the best match (the most matches from left
to right for the search arg) unlike MSearch and like ^R Incremental Search.
I have taken the liberty of binding them to C-R and C-S so as to
replace the regular MSearch and MRsearch, since I couldn't find any other
good ways to name them consistent with the current naming philosophy.
MIncSearch() and MIncRSearch() written by Barry A. Dobyns 07/07/81.
Modifications record:
		07/07/81	Original Implementation in MINCE 2.5 completed. [BAD].
		07/22/81	Bug fixes and installation in Version 2.6.
                    Significant speed improvements by running under
                    2.6, even with the redisplay, it is still faster
                    than the old 2.5 searches alone. I strongly do not
                    reccommend permanent implementation in 2.5. [BAD].
		07/23/81  Renamed to MSearch and MRSearch to save space. 
				I only need one set of search commands at a time.
				Since L2 and CLINK both ignore all occurences of a
				function except for the first. With COMM1 ahead of
                    the -l option on the L2 command line, we MUST name
                    these guys the same as the functions they replace,
                    or edit them out of either the source (before
                    compilation) or the .CRL (with CLIB). I can compile
                    and link this into a small -e7900 MINCE in 2.6,
                    but I believe my TERM routines are a bit shorter
                    overall, so you may not be able to. [BAD].
		09/19/81  Renamed back to MIncSearch and MIncRSearch, since
                    I have moved MSearch and MRSearch, along with all
                    of the page mode functions [e.g. MP????() ] into
                    COMM4.C so that they will not be linked in
                    anymore. COMM4.C appears after the -l switch on
                    the L2 command line. Page Mode is commented out of
                    BINDINGS.C. It is tempting to put all of the
                    COMMn.C files after the -l option, and not play
                    musical source code anymore. [BAD].
*/

/* BINDINGS.C - Set up the key bindings for Mince

The seller of this software hereby disclaims any and all
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
	Created for Version 2.3  10/4/80  JTL
	Updated to version three  1/7/81  JTL

	This file contains the function table initializers and mode set up
functions */

#include "mince.gbl"

finit1()			/* initialize the control and self insert key bindings */
{
	int cnt;
	int MInsert(), MSetMrk(), MBegLin(), MPrevChar();
	int MNotImpl(), MDelChar(), MEndLin(), MNextChar();
	int MAbort(), MDelLin(), MNewDsp(), MNewLin();
	int MNextLin(), MOpenLin(), MPrevLin(), MQuote();
	int MIncRSearch(), MIncSearch(), MSwapChar(), MArg();
	int MNextPage(), MDelRgn(), MCtrlX(), MYank();
	int MMeta(), MDelIndent(), MRDelChar(), MIndent();


	for (cnt=0; cnt<128; ++cnt) functs[cnt] = MInsert;
	TKbChk();
 */
	functs[1] = MBegLin;		/* C-A */
	functs[2] = MPrevChar;		/* C-B */
	functs[3] = MNotImpl;		/* C-C */
	functs[4] = MDelChar;		/* C-D */
	functs[5] = MEndLin;		/* C-E */
	functs[6] = MNextChar;		/* C-F */
	functs[7] = MAbort;			/* C-G */
	functs[8] = MPrevChar;		/* C-H */
							/* C-I */
	functs[10] = MIndent;		/* C-J */
	functs[11] = MDelLin;		/* C-K */
	functs[12] = MNewDsp;		/* C-L */
	functs[13] = MNewLin;		/* C-M */
	functs[14] = MNextLin;		/* C-N */
	functs[15] = MOpenLin;		/* C-O */
	functs[16] = MPrevLin;		/* C-P */
	functs[17] = MQuote;		/* C-Q */
	functs[18] = MIncRSearch;		/* C-R */
	functs[19] = MIncSearch;		/* C-S */
	functs[20] = MSwapChar;		/* C-T */
	functs[21] = MArg;			/* C-U */
	functs[22] = MNextPage;		/* C-V */
	functs[23] = MDelRgn;		/* C-W */
	functs[24] = MCtrlX;		/* C-X */
	functs[25] = MYank;			/* C-Y */
	functs[26] = MNotImpl;		/* C-Z */
	functs[ESC] = MMeta;		/* ESC */
	functs[28] = MDelIndent;		/* C-\ */
	functs[29] = MNotImpl;		/* C-] */
	functs[30] = MNotImpl;		/* C-^ */
	functs[31] = MNotImpl;		/* C-_ */
	functs[DEL] = MRDelChar;		/* DEL */
	}

finit2()			/* set up the Meta key bindings */
{
	int cnt;
	int MNotImpl(), MAbort(), MDelELin(), MRNewDsp(); 
	int MDoMacro(), MQryRplc();
	int MMakeDel(), MToStart(), MToEnd(), MBSent();
	int BWord(), MCapWord(), MDelWord(), MFSent();
	int FWord(), MMrkPara(), MDelSent(), MLowWord();
	int MMakMacro(), MFillPara(), MReplace(), MCntrLine(), MUpWord();
	int MPrevPage(), MCopyRgn(), MBPara(), MDelWhite();
	int MFPara(), MRDelWord(), MSetMrk(), MSwapWord();

	for (cnt=128; cnt<256; ++cnt) functs[cnt] = MNotImpl;
	TKbChk();
	functs[128+7] = MAbort;					/* M-C-G */
	functs[128+8] = MRDelWord;				/* M-C-H */
	functs[128+11] = MDelELin;				/* M-C-K */
	functs[128+12] = MRNewDsp;				/* M-C-L */
	functs[128+13] = MDoMacro;				/* M-C-M */
	functs[128+18] = MQryRplc;				/* M-C-R */
	functs[128+23] = MMakeDel;				/* M-C-W */
	functs[128+'<'] = MToStart;				/* M-< */
	functs[128+'>'] = MToEnd;				/* M-> */
	functs[128+' '] = MSetMrk;				/* M-  */
	functs[128+'A']=functs[128+'a'] = MBSent;	/* M-A */
	functs[128+'B']=functs[128+'b'] = BWord;	/* M-B */
	functs[128+'C']=functs[128+'c'] = MCapWord;	/* M-C */
	functs[128+'D']=functs[128+'d'] = MDelWord;	/* M-D */
	functs[128+'E']=functs[128+'e'] = MFSent;	/* M-E */
	functs[128+'F']=functs[128+'f'] = FWord;	/* M-F */
	functs[128+'H']=functs[128+'h'] = MMrkPara;	/* M-H */
	functs[128+'K']=functs[128+'k'] = MDelSent;	/* M-K */
	functs[128+'L']=functs[128+'l'] = MLowWord;	/* M-L */
	functs[128+'M']=functs[128+'m'] = MMakMacro;	/* M-M */
	functs[128+'Q']=functs[128+'q'] = MFillPara;	/* M-Q */
	functs[128+'R']=functs[128+'r'] = MReplace;	/* M-R */
	functs[128+'S']=functs[128+'s'] = MCntrLine;	/* M-S */
	functs[128+'T']=functs[128+'t'] = MSwapWord;	/* M-T */
	functs[128+'U']=functs[128+'u'] = MUpWord;	/* M-U */
	functs[128+'V']=functs[128+'v'] = MPrevPage;	/* M-V */
	functs[128+'W']=functs[128+'w'] = MCopyRgn;	/* M-W */
	functs[128+'['] = MBPara;				/* M-[ */
	functs[128+'\\'] = MDelWhite;				/* M-\ */
	functs[128+']'] = MFPara;				/* M-] */
	functs[128+DEL] = MRDelWord;				/* M-DEL */
	}

finit3()			/* set up the Control-X key bindings */
{
	int cnt;
	int MNotImpl(), MLstBuffs(), MExit(), MFindFile();
	int MAbort(), MSetTabs(), MDelMode(), MFileRead();
	int MFileSave(), MFileWrite(), MSwapMrk(), MSetIndent();
	int MPrintPos(), MSwitchTo(), MKillBuff(), MSetFill();
	int MAddMode(), MOneWind(), MTwoWind(), MSwpWind();
	int MNxtOthrWind(), MPrvOthrWind(), MGrowWind();
#ifdef UNIX
	int MUnix();
#endif

	for (cnt=256; cnt<384; ++cnt) functs[cnt] = &MNotImpl;
	TKbChk();
	functs[256+2] = MLstBuffs;				/* C-X C-B */
	functs[256+3] = MExit;					/* C-X C-C */
	functs[256+6] = MFindFile;				/* C-X C-F */
	functs[256+7] = MAbort;					/* C-X C-G */
	functs[256+9] = MSetTabs;				/* C-X Tab */
	functs[256+13] = MDelMode;				/* C-X C-M */
	functs[256+18] = MFileRead;				/* C-X C-R */
	functs[256+19] = MFileSave;				/* C-X C-S */
	functs[256+22] = MNxtOthrWind;			/* C-X C-V */
	functs[256+23] = MFileWrite;				/* C-X C-W */
	functs[256+24] = MSwapMrk;				/* C-X C-X */
	functs[256+26] = MPrvOthrWind;			/* C-X C-Z */
	functs[256+'.'] = MSetIndent;				/* C-X . */
#ifdef UNIX
	functs[256+'!'] = MUnix;					/* C-X ! */
#endif
	functs[256+'='] = MPrintPos;				/* C-X = */
	functs[256+'1'] = MOneWind;				/* C-X 1 */
	functs[256+'2'] = MTwoWind;				/* C-X 2 */
	functs[256+'B']=functs[256+'b'] = MSwitchTo;	/* C-X B */
	functs[256+'K']=functs[256+'k'] = MKillBuff;	/* C-X K */
	functs[256+'F']=functs[256+'f'] = MSetFill;	/* C-X F */
	functs[256+'M']=functs[256+'m'] = MAddMode;	/* C-X M */
	functs[256+'O']=functs[256+'o'] = MSwpWind;	/* C-X O */
	functs[256+'^'] = MGrowWind;				/* C-X ^ */
	}

SetModes()			/* Set the modes according to the bmodes array */
{
	int cnt;
	int MFillChk();
/*	These are for the (unnecessary) page mode
	int MOverwrite(), MPBegLine(), MInsert();
	int MPBChar(), MPEndLine(), MPFChar(), MPBackSpace();
	int MPNextLine(), MPPrevLine(), MTrimWhite(), MPTab(); */
#ifdef LARGE
	int MNewLin(), MCIndent(), MSInsert();
#endif

	*mode='\0';
	finit1();					/* Set up the key bindings */
	finit2();
	finit3();
	for (cnt=MAXMODES-1; cnt>=0; --cnt) {
		switch (buffs[cbuff].bmodes[cnt]) {

#ifdef LARGE
		case 'c':
			functs[13] = MCIndent;
			strcat(mode,"C ");
			break;
		case 's':
			Rebind(MInsert,MSInsert);
			strcat(mode,"Save ");
			break;
#endif
		case 'f':
			functs[' '] = MFillChk;
			strcat(mode,"Fill ");
			break;
/*		case 'p':
			Rebind(MInsert,MOverwrite);
			functs[1] = MPBegLine;
			functs[2] = MPBChar;
			functs[5] = MPEndLine;
			functs[6] = MPFChar;
			functs[8] = MPBackSpace;
			functs[9] = MPTab;
			functs[14] = MPNextLine;
			functs[16] = MPPrevLine;
			functs[256+'\\'] = MTrimWhite;
			strcat(mode,"Page ");
			break;	*/
			}
		}
	if (!*mode) strcpy(mode,"Normal ");
	mode[strlen(mode)-1]='\0';
	ModeLine();
	}

UInit()				/* Do user code initilization */
	/* This function is called once at the very begining. It may be used
	to do any initilization required for user code */
{
	}

MIncSearch()		/* Forward Incremental String Search */
{
	int tmp,cnt;
	if (!GetArg("Incremental Search <ESC>: ",ESC,strarg,STRMAX)){
		arg=0;
		return;
		}
	for (cnt = 0; strarg[cnt] != '\0'; cnt++){
		tmp = strarg[cnt + 1];	/* save the char */
		strarg[cnt + 1] = '\0';	/* and truncate srtarg to length I */
		tmark = BCreMrk();		/* Current point */ 
		if (!StrSrch(strarg,FORWARD)){
			BPntToMrk(tmark);
			BMove(cnt);		/* Put point after last found chars */
			Echo ("Not Found");
			arg=0;
			return;
			}
		strarg[cnt + 1] = tmp;	/* restore strarg */
		IncrDsp();			/* force redisplay */
		BMove(-cnt - 1);		/* get before the truncated strarg 
							   so that if this is the complete
							   strarg we will find it */
		BKillMrk(tmark);
		}
	Bmove(cnt);				/* Put point after last found chars */
	}

MIncRSearch()		/* Reverse Incremental String Search */
{
	int tmp,cnt;
	if (!GetArg("Reverse Incremental Search <ESC>: ",ESC,strarg,STRMAX)){
		arg=0;
		return;
		}
	for (cnt = 1; strarg[cnt] != '\0'; cnt++){
		tmp = strarg[cnt + 1];	/* save the char */
		strarg[cnt + 1] = '\0';	/* and truncate srtarg to length I */
		tmark = BCreMrk();		/* Current point */ 
		if (!StrSrch(strarg,BACKWARD)){
			BPntToMrk(tmark);
			BMove(-cnt);		/* Put point after last found chars */
			Echo ("Not Found");
			arg=0;
			return;
			}
		strarg[cnt + 1] = tmp;	/* restore strarg */
		IncrDsp();			/* force redisplay */
		BMove(cnt + 1);		/* get before the truncated strarg 
							   so that if this is the complete
							   strarg we will find it */
		BKillMrk(tmark);
		}
	BMove(-cnt);				/* Put point after last found chars */
	}

MNewDsp()			/* move down arg lines and redisplay*/
{
	if (argp){
		if (!VMovCmnd(lfunct)) lcol=BGetCol();
		while(arg-- > 0) NLSrch();
		BMakeCol(lcol);
		}
	arg=0;
	NewDsp();
	}

MRNewDsp()			/* move arg lines previous and redisplay */
{
	if (argp){
		if (!VMovCmnd(lfunct)) lcol=BGetCol();
		while (arg-- > 0) RNLSrch();
		BMakeCol(lcol);
		}
	arg = 0;
	NewDsp();
	}

MMakMacro()
{
	char *macroptr;

	macroptr = spare;
	GetMacro("Macro <C-^>:",'\035',strarg,20);
	strcpy(macroptr,strarg);
	Echo(macroptr);
	}

MDoMacro()
{
	char *macroptr;
	
	macroptr = spare;
	TSetPoint(TMaxRow()-1,0);
	while (*macroptr != '\0'){
		TPrntChar(*macroptr);
		if (!QFull(&kbdq)) QShove(*macroptr, &kbdq);
		else TBell();
		macroptr++;
		}
	}

GetMacro(mesg,term,str,len)	/* input a string argument */
	char *mesg, term, *str;
	int len;
{
	char stemp[80], inpt, *nstr, tcol;

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
		switch (inpt) {
			case CR:
				TPrntStr("<NL>");
				*nstr++=NL;
				break;
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

/* END OF BINDINGS.C - set up key bindings */
