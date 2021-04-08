/* -*-C,save,tab:8,indent:4-*- */
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

	This file contains the function table initializers and mode set up functions */

#include "mince.gbl"

finit1()			/* initialize the control and self insert key bindings */
{
	int tcnt;
	int MInsert(), MSetMrk(), MBegLin(), MPrevChar();
	int MNotImpl(), MDelChar(), MEndLin(), MNextChar();
	int MAbort(), MDelLin(), NewDsp(), MNewLin();
	int MNextLin(), MOpenLin(), MPrevLin(), MQuote();
	int MRSearch(), MSearch(), MSwapChar(), MArg();
	int MNextPage(), MDelRgn(), MCtrlX(), MYank();
	int MPrevPage(), MMeta(), MDelIndent(), MRDelChar();
	int MIndent();


	for (tcnt=0; tcnt<128; ++tcnt) functs[tcnt] = MInsert;
	TKbChk();
	functs[0] = MSetMrk;		/* C-@ */
	functs[1] = MBegLin;		/* C-A */
	functs[2] = MPrevChar;		/* C-B */
	functs[3] = MNotImpl;		/* C-C */
	functs[4] = MDelChar;		/* C-D */
	functs[5] = MEndLin;		/* C-E */
	functs[6] = MNextChar;		/* C-F */
	functs[7] = MAbort;			/* C-G */
	functs[8] = MRDelChar;		/* C-H */
							/* C-I */
	functs[10] = MIndent;		/* C-J */
	functs[11] = MDelLin;		/* C-K */
	functs[12] = NewDsp;		/* C-L */
	functs[13] = MNewLin;		/* C-M */
	functs[14] = MNextLin;		/* C-N */
	functs[15] = MOpenLin;		/* C-O */
	functs[16] = MPrevLin;		/* C-P */
	functs[17] = MQuote;		/* C-Q */
	functs[18] = MRSearch;		/* C-R */
	functs[19] = MSearch;		/* C-S */
	functs[20] = MSwapChar;		/* C-T */
	functs[21] = MArg;			/* C-U */
	functs[22] = MNextPage;		/* C-V */
	functs[23] = MDelRgn;		/* C-W */
	functs[24] = MCtrlX;		/* C-X */
	functs[25] = MYank;			/* C-Y */
	functs[26] = MPrevPage;		/* C-Z */
	functs[ESC] = MMeta;		/* ESC */
	functs[28] = MDelIndent;		/* C-\ */
	functs[29] = MNotImpl;		/* C-] */
	functs[30] = MNotImpl;		/* C-^ */
	functs[31] = MNotImpl;		/* C-_ */
	functs[DEL] = MRDelChar;		/* DEL */
	}

finit2()			/* set up the Meta key bindings */
{
	int tcnt;
	int MNotImpl(), MAbort(), MDelELin(), MQryRplc();
	int MMakeDel(), MToStart(), MToEnd(), MBSent();
	int BWord(), MCapWord(), MDelWord(), MFSent();
	int FWord(), MMrkPara(), MDelSent(), MLowWord();
	int MFillPara(), MReplace(), MCntrLine(), MUpWord();
	int MPrevPage(), MCopyRgn(), MBPara(), MDelWhite();
	int MFPara(), MRDelWord(), MSetMrk(), MSwapWord();

	for (tcnt=128; tcnt<256; ++tcnt) functs[tcnt] = MNotImpl;
	TKbChk();
	functs[128+7] = MAbort;					/* M-C-G */
	functs[128+8] = MRDelWord;				/* M-C-H */
	functs[128+11] = MDelELin;				/* M-C-K */
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
	int tcnt;
	int MNotImpl(), MLstBuffs(), MExit(), MFindFile();
	int MAbort(), MSetTabs(), MDelMode(), MFileRead();
	int MFileSave(), MFileWrite(), MSwapMrk(), MSetIndent();
	int MPrintPos(), MSwitchTo(), MKillBuff(), MSetFill();
	int MAddMode(), MOneWind(), MTwoWind(), MSwpWind();
	int MNxtOthrWind(), MPrvOthrWind(), MGrowWind();
#ifdef UNIX
	int MUnix();
#endif
#ifdef KBMACRO
	int MStrtRem(),MStpRem(),MExcKBMc();
#endif
#ifdef STRIDE
	int MInsTime();
#endif
#ifdef GENHEAD
	int MGenPro();
#endif
#ifdef AUTOMODE
	int InitMode();
#endif

	for (tcnt=256; tcnt<384; ++tcnt) functs[tcnt] = MNotImpl;
	TKbChk();
	functs[256+2] = MLstBuffs;				/* C-X C-B */
	functs[256+3] = MExit;					/* C-X C-C */
	functs[256+6] = MFindFile;				/* C-X C-F */
	functs[256+7] = MAbort;					/* C-X C-G */
	functs[256+9] = MSetTabs;				/* C-X Tab */
	functs[256+13] = MDelMode;				/* C-X C-M */
#ifdef GENHEAD
	functs[256+16] = MGenPro;
#endif
	functs[256+18] = MFileRead;				/* C-X C-R */
	functs[256+19] = MFileSave;				/* C-X C-S */
#ifdef STRIDE
	functs[256+20] = MInsTime;				/* C-X C-T */
#endif
	functs[256+22] = MNxtOthrWind;			/* C-X C-V */
	functs[256+23] = MFileWrite;				/* C-X C-W */
	functs[256+24] = MSwapMrk;				/* C-X C-X */
	functs[256+26] = MPrvOthrWind;			/* C-X C-Z */
	functs[256+'.'] = MSetIndent;				/* C-X . */
#ifdef UNIX
	functs[256+'!'] = MUnix;					/* C-X ! */
#endif
	functs[256+'='] = MPrintPos;				/* C-X = */
#ifdef KBMACRO
	functs[256+'('] = MStrtRem;				/* C-X ( */
	functs[256+')'] = MStpRem;				/* C-X ) */
#endif
	functs[256+'1'] = MOneWind;				/* C-X 1 */
	functs[256+'2'] = MTwoWind;				/* C-X 2 */
	functs[256+'B']=functs[256+'b'] = MSwitchTo;	/* C-X B */
#ifdef KBMACRO
	functs[256+'E']=functs[256+'e'] = MExcKBMc;	/* C-X E */
#endif
	functs[256+'K']=functs[256+'k'] = MKillBuff;	/* C-X K */
	functs[256+'F']=functs[256+'f'] = MSetFill;	/* C-X F */
#ifdef AUTOMODE
	functs[256+'I']=functs[256+'i'] = InitMode;	/* C-X I */
#endif
	functs[256+'M']=functs[256+'m'] = MAddMode;	/* C-X M */
	functs[256+'O']=functs[256+'o'] = MSwpWind;	/* C-X O */
	functs[256+'^'] = MGrowWind;				/* C-X ^ */
	}

SetModes()			/* Set the modes according to the bmodes array */
{
	int tcnt;
	int MFillChk(), MInsert(), MOverwrite(), MPBegLine();
	int MPBChar(), MPEndLine(), MPFChar(), MPBackSpace();
	int MPNextLine(), MPPrevLine(), MTrimWhite(), MPTab();
#ifdef LARGE
	int MNewLin(), MCIndent(), MSInsert();
#endif
#ifdef FLASHER
	int MFlash();
#endif
#ifdef LISPMODE
	int MLspNLI(), MLspInd(), MForPar(), MBckPar();
#endif
#ifdef GENHEAD
	int MGenCPr();
#ifdef LISPMODE
	int MGenLPr();
#endif
#endif

	*mode='\0';
	finit1();					/* Set up the key bindings */
	finit2();
	finit3();
#ifdef KEYPAD
/* If KEYPAD is set, set up keypad bindings */
	KeyPad();
#endif
#ifdef WY50
	wy50_finit();	/* WY-50 function key bindings - RPH */
#endif
#ifdef FLASHER
	SynCopy(CurSyn,NormSyn,MAXCHARS);
#endif
#ifdef ELECTRIC
	ECpyAbrv(CurAbr,NormAbr,MAXABREVS);
#endif
	for (tcnt=MAXMODES-1; tcnt>=0; --tcnt) {
		switch (buffs[cbuff].bmodes[tcnt]) {

#ifdef LARGE
		case 'c':
			functs[13] = MCIndent;
#ifdef FLASHER
			SynCopy(CurSyn,CSyn,MAXCHARS);
			functs[')'] = MFlash;
			functs['}'] = MFlash;
#endif
#ifdef GENHEAD
			functs[256+16] = MGenCPr;
#endif
#ifdef ELECTRIC
			CElecInit();
#endif
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
		case 'p':
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
			break;
#ifdef LISPMODE
		case 'l':
			functs[10] = MLspNLI;
			functs[9] = MLspInd;
#ifdef FLASHER
			SynCopy(CurSyn,LispSyn,MAXCHARS);
			functs[')'] = MFlash;
			functs[128+')'] = MForPar;
			functs[128+'('] = MBckPar;
#endif
#ifdef GENHEAD
			functs[256+16] = MGenLPr;
#endif
#ifdef ELECTRIC
			LElecInit();
#endif
			strcat(mode,"Lisp ");
			break;
#endif
			}
		}
	if (!*mode) strcpy(mode,"Normal ");
	mode[strlen(mode)-1]='\0';
	ModeLine();
#ifdef AUTOMODE
/*	Debug("mvars[cbuff].mfill=",mvars[cbuff].mfill);*/
/*	Debug("mvars[cbuff].mindent=",mvars[cbuff].mindent);*/
/*	Debug("mvars[cbuff].mtab=",mvars[cbuff].mtab);*/
	if (mvars[cbuff].mfill > 0) fillwidth = mvars[cbuff].mfill;
	if ((mvars[cbuff].mindent >= 0) &&
	    (mvars[cbuff].mfill > 0)) indentcol = mvars[cbuff].mindent;
	if (mvars[cbuff].mtab > 0) tabincr = mvars[cbuff].mtab;
#endif
	}

UInit()				/* Do user code initilization */
	/* This function is called once at the very begining. It may be used
	to do any initilization required for user code */
{int i;

#ifdef FLASHER
    SynInit();
#endif
#ifdef ELECTRIC
    EInitAbrv();
#endif
#ifdef AUTOMODE
/*    Debug("fillwidth=",fillwidth);*/
/*    Debug("indentcol=",indentcol);*/
/*    Debug("tabincr=",tabincr);*/
    for (i=0;i<BUFFSMAX;i++) {
	mvars[i].mfill = fillwidth;
	mvars[i].mindent = indentcol;
	mvars[i].mtab = tabincr;
	}
    InitMode();
#endif
#ifdef KBMACRO
    remember = FALSE; KBexecuting = FALSE; GAbort = FALSE;
#endif
    }
#ifdef AUTOMODE
#define MODEIDENT "-*-"	/* mode indent strings */
InitMode()
{
	int mmark,	/* mode mark */
	    md;		/* holds mode */
	char mbuf[80],	/* mode name buffer */
	     *p,*q,	/* temp pointers */
	     sep;	/* separater character */

	BToStart();	/* beginning of buffer */
	NLSrch();	/* find first newline */
	if (!StrSrch(MODEIDENT,BACKWARD)) { /* find second MODEIDENT. if none, then */
		BToStart();	/* clean up, etc. */
		return;
		}
	mmark = BCreMrk();	/* save our place for later */
/*	Debug("After BCreMrk - ",mmark);*/
	/* find first MODEIDENT */
	if (!StrSrch(MODEIDENT,BACKWARD)) {
		BToStart();	/* clean up, etc. */
/*	Debug("About to kill mark (no -*-) ",mmark);*/
		BKillMrk(mmark);
		return;
		}
	else StrSrch(MODEIDENT,FORWARD); /* else backup to just before mode str */
	/* now copy from user's buffer to temp space */
	p = &mbuf[0];
/*	Debug("Before copy loop ",mmark);*/
	while (BIsBeforeMrk(mmark)) {
		*p++ = tolower(Buff());
		BMove(1);
		}
	*p++ = '\0'; *p++ = '\0'; /* eos. (put two in to be sure) */
	/* kill off mark */
/*	Debug("About to kill mark ",mmark);*/
	BKillMrk(mmark);
	BToStart();	/* reset position */
	p = q = &mbuf[0]; /* reset pointers */
	while (*p != '\0') {	/* while not eob */
		while(!isalpha(*q) && *q != '\0') q++; /* skip boring junk */
		if (*q == '\0') break;
		p = q;
		while(isalpha(*p)) p++; /* find end of interesting stuff */
		sep = *p;	/* save separater */
		*p++ = '\0';	/* terminate piece */
		md = CheckMode(q);	/* get mode id from mode name */
		if (md) insmode(md);	/* insert mode */
		else if (sep == ':') CheckVar(q,&p); /* else check for xxx:n stuff */
		q = p;		/* advance to next */
		}
/*	Debug("mvars[cbuff].mfill=",mvars[cbuff].mfill);*/
/*	Debug("mvars[cbuff].mindent=",mvars[cbuff].mindent);*/
/*	Debug("mvars[cbuff].mtab=",mvars[cbuff].mtab);*/
	SetModes();	/* set up selected modes */
	}
static insmode(md)
int md;
{
	int i;

	for (i=MAXMODES-1;i>=0;--i)
		if (!buffs[cbuff].bmodes[i]) {
			buffs[cbuff].bmodes[i] = md;
			return;
			}
	}
static CheckVar(name,next)
char *name,**next;
{
    char *p,*q;
    static char *names[] = {"fillw","indent","tab",NULL};
    char **n2;
    static int *vars[] = {NULL,NULL,NULL,NULL};
    int i,j;

    vars[0] = (&mvars[cbuff].mfill);
    vars[1] = (&mvars[cbuff].mindent);
    vars[2] = (&mvars[cbuff].mtab);
    i = 0;
    for (n2=(&names[0]);*n2!=NULL;n2++)
        if (strcmp(name,*n2) == 0) {
	    p = q = *next;
	    while(isdigit(*p)) p++;
	    *p++ ='\0';
	    *next = p;
	    j = atoi(q);
/*	    Debug(q,j);*/
	    *vars[i] = j;
	    return;
	    }
	else i++;
    }
#endif

/* END OF BINDINGS.C - set up key bindings */

