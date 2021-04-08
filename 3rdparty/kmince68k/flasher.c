/* -*-c,save-*- */
/*
 * FLASHER.C - code for paren flashing, etc.
 * Robert Heller
 * Last Mod. Mon Sep 23, 1985 21:49:30.31
 */
#include "mince.gbl"
#include <ctype.h>

/* #define DEBUG 1		/* debugging hackery */


SynInit()
{
    register int i;
    register SyntaxEntry *s1,*s2
#ifdef LISPMODE
			 ,*s3
#endif
	;

    s1 = NormSyn;
    s2 = CSyn;
#ifdef LISPMODE
    s3 = LispSyn;
#endif
    for (i = 0; i< MAXCHARS; i++) {
#ifdef LISPMODE
	s3->s_kind = 
#endif
	s2->s_kind =
	s1->s_kind = (isalnum(i))?WORD:DULL;
	s1++; s2++;
#ifdef LISPMODE
	s3++;
#endif
	}
    CSyn['{'].s_kind = BEGP;
    CSyn['{'].s_MP = '}';
    CSyn['}'].s_kind = ENDP;
    CSyn['}'].s_MP = '{';
    CSyn['('].s_kind = BEGP;
    CSyn['('].s_MP = ')';
    CSyn[')'].s_kind = ENDP;
    CSyn[')'].s_MP = '(';
    CSyn['"'].s_kind = PAIRQ;
    CSyn['\''].s_kind = PAIRQ;
    CSyn['\\'].s_kind = PREQ;
#ifdef ELECTRIC
    CSyn['['].s_kind = BEGP;
    CSyn['['].s_MP = ']';
    CSyn[']'].s_kind = ENDP;
    CSyn[']'].s_MP = '[';
#endif
#ifdef LISPMODE
    LispSyn['('].s_kind = BEGP;
    LispSyn['('].s_MP = ')';
    LispSyn[')'].s_kind = ENDP;
    LispSyn[')'].s_MP = '(';
    LispSyn['"'].s_kind = PAIRQ;
    LispSyn['\\'].s_kind = PREQ;
#endif
    }

SynCopy(to,from,count)
register SyntaxEntry *to,*from;
register int count;
{
    while(count-- > 0) {
	to->s_kind = from->s_kind;
	to->s_MP = from->s_MP;
	to++; from++;
	}
    }

ParScan(StpAtNL,forward,ParLevel)
register int StpAtNL,forward,ParLevel;
{
    register char c,pc;
    char parstack[200];
    int InString = 0;
    char MatchQuote = 0;
    register char k;
    register int on_on = 1;
    UWORD BLocation();

    {
	register int i;
	for (i = 0; i <= ParLevel; i++) parstack[i] = 0;
	}
    if (StpAtNL) {
	if (forward) {
	    while((!BIsEnd()) &&
		  ((c = Buff()) == ' ' || c == '\t' || c == NL)) {
#ifdef DEBUG
		Debug("*** in ParScan(): scanning over whitespace ",c);
#endif
		BMove(1);
		}
	    }
	else {
	    while((!BIsStart()) &&
		  ((c = Buff()) == ' ' || c == '\t' || c == NL))
		BMove(-1);
	    }
	}
    while (on_on) {
	if (forward) {
	    if (BIsEnd()) return(0);
	    BMove(1);
	    }
	if (BLocation() > 1) {
	    BMove(-2);
	    pc = Buff();
	    BMove(2);
	    }
	else {
	    pc = 0;
	    if (BLocation() < 1) return(0);
	    }
	BMove(-1); c = Buff(); BMove(1);
	k = CurSyn[toascii(c)].s_kind;
	if (CurSyn[toascii(pc)].s_kind == PREQ) k = WORD;
#ifdef DEBUG
	Debug("In ParScan loop. char is ",k);
#endif
	if ((!InString || c == MatchQuote) && k == PAIRQ) {
	    InString = !InString;
	    MatchQuote = c;
	    }
	if (StpAtNL && c == NL && ParLevel == 0) return(0);
	if (!InString && (k == ENDP || k == BEGP)) {
	    if ((forward == 0) == (k == ENDP)) {
		ParLevel++;
		parstack[ParLevel] = CurSyn[toascii(c)].s_MP;
#ifdef DEBUG
	Debug("Stacking a paren. stacked char is ",parstack[ParLevel]);
#endif
		}
	    else {
#ifdef DEBUG
	Debug("Unstacking a paren. stacked char is ",parstack[ParLevel]);
#endif
		if (ParLevel > 0 && parstack[ParLevel] && 
				    parstack[ParLevel] != c) {
/*		    Error("Parenthesis mismatch.");*/
		    return(0);
		    }
		ParLevel--;
		}
	    if (ParLevel < 0 || (ParLevel == 0 && !StpAtNL)) on_on = 0;
	    }
	if (!forward) BMove(-1);
	}
    return(1);
    }
MFlash()
{
    register int hmark,scan,dc,dly;
    
    BInsert(toascii(cmnd));
    hmark = BCreMrk();
/*    BMove(-1);*/
    scan = ParScan(0,0,0);
/*    Debug("ParScan returns. Scan is ",scan);*/
    if (!scan) {
	BPntToMrk(hmark);
	IncrDsp();
	Error("No Match");
	BKillMrk(hmark);
	}
    else {
	IncrDsp();
	for (dc=terminal.delaycnt*terminal.mhz*10; dc; --dc)
	    if (TKbRdy()) break;
	BPntToMrk(hmark);
	BKillMrk(hmark);
	IncrDsp();
	}	
    }
#ifdef LISPMODE
MForPar()
{
    int lev;

    if (!getnum("Paren Level: ",&lev)) return;
    ParScan(0,1,lev);
    }
MBckPar()
{
    ParScan(0,0,0);
    }
MBckPNL()
{
    return(ParScan(1,0,0));
    }
static getnum(pmt,result)
char *pmt;
int *result;
{
    char nbuff[20];

    if (!GetArg(pmt,CR,&nbuff[0],20)) return(FALSE);
    *result = atoi(nbuff);
    return(TRUE);
    }

#endif

