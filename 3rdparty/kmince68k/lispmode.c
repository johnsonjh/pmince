/* -*-c,save-*- */
/*
 * LISPMODE.C - LISP-mode code for Mince
 * Robert Heller Created Tue Sep 24, 1985 19:48:46.06
 * Last Mod: Wed May 21, 1986 18:59:27.12
 */
#include "mince.gbl"

static int LspCommC = 49,	/* default comment column */
	   LspDefInd = 2;	/* default indent ammount */

/* #define DEBUG 1		/* debugging hackery */

MLspSCC()
{
    LspCommC = arg;
    arg = 0;
    }
MLspSDI()
{
    LspDefInd = arg;
    arg = 0;
    }
_LspSCC(col)
int col;
{
    LspCommC = col;
    }
_LspSDI(ind)
int ind;
{
    LspDefInd = ind;
    }
MLspCmn()
{
    int IsSemi(),IsWhite();

    ToBegLine();
    while (!BIsEnd() && Buff() != ';' && Buff() != NL) ;
    if (!BIsEnd() && Buff() == ';') {
	MovePast(IsSemi,FORWARD);
	MovePast(IsWhite,FORWARD);
	}
    else {
	ToEndLine();
	ForceCol(LspCommC,FORWARD);
	BInsert(';');
	BInsert(' ');
	}
    }
static int IsSemi()
{
    return(Buff() == ';');
    }
MDelLInd()
{
    int ch1,ch2;

    ToBegLine();
    BMove(-1);
    BDelete(1);
    MDelWhite();
    ch1 = Buff(); BMove(-1); ch2 = Buff(); BMove(1);
    if (!(ch1 == '(' || ch1 == ')' || ch2 == '(' || ch2 == ')')) BInsert(' ');
    }
MRDelCTab()
{
    int col;

    col = BGetCol() - 1;
    BMove(-1);
    BDelete(1);
    if (col>0) SIndent(col-BGetCol());
    }
MLspNLIn()
{
    MNewLin();
    MLspIndent();
    }
MLspIndent()
{
    int col,tmpmark;

    tmpmark = BCreMrk();
    ToBegLine();
    if (BIsStart()) col = 0;
    else {
	BMove(-1);
	col = LspIndCol();
	}
#ifdef DEBUG
    Debug("*** in LspIndent(): col = ",col);
#endif
    BPntToMrk(tmpmark);
    ToBegLine();
    if (IsSemi()) BMove(1);
    MDelWhite();
    TIndent(col);
    BPntToMrk(tmpmark);
    BKillMrk(tmpmark);
    while (BGetCol() < col) BMove(1);
    }
LspIndCol()
{
    int tmpmark,col;

    tmpmark = BCreMrk();
    col = LspInd1();
    BPntToMrk(tmpmark);
    BKillMrk(tmpmark);
    return(col);
    }
LspInd1()
{
    register int l,col;

    l = ParScan(1,0,1);
    col = 0;
#ifdef DEBUG
    Debug("***in LspInd1(): ParScan(1,0,0) = ",l);
#endif
    if (l == 0) {
	if (!blinep()) {
	    if (istwocmm()) {
		BDelete(1);
		BInsert(' ');
		col = getind();
		BMove(-1);
		BDelete(1);
		BInsert(';');
		}
	    else col = getind();
	    }
	else {
	    ToBegLine();
	    if (!BIsStart()) {
		BMove(-1);
		col = LspIndCol();
		}
	    }
	}
    else col = LspIArg();
    return(col);
    }
static LspIArg()
{
    int tmpmark,c;
    register int col;

    col = BGetCol() + LspDefInd;
#ifdef DEBUG
    Debug("***in LspIArg(): col (initial value) = ",col);
#endif
    tmpmark = BCreMrk();
    BMove(1);
    while (! ((BIsEnd()) || ((c = Buff()) == ' ') || (c == '\t') ||
	      (c == ';') || (c == 40) || (c == NL))) BMove(1);
    c = Buff();
    if (c == 40) col = BGetCol();
    else ToNotWhite(FORWARD);
#ifdef DEBUG
    Debug("***in LspIArg(): c = ",c);
#endif
    c = Buff();
    if (!BIsEnd() && c != NL && c != ';') col = BGetCol();
#ifdef DEBUG
    Debug("***in LspIArg(): col (final value) = ",col);
#endif
    BPntToMrk(tmpmark);
    BKillMrk(tmpmark);
    return(col);
    }
static istwocmm()
{
    int tmpmark,IsNL();

    tmpmark = BCreMrk();
    BMove(-1);
    if (BIsStart() || IsNL()) {
	if (!BIsStart()) BMove(1);
	if (BIsEnd()) {
	    BPntToMrk(tmpmark);
	    BKillMrk(tmpmark);
	    return(FALSE);
	    }
	if (Buff() == ';') {
	    ToEndLine();
	    BMove(1);
	    if (BIsEnd()) {
		BPntToMrk(tmpmark);
		BKillMrk(tmpmark);
		return(FALSE);
		}
	    else if (Buff() == ';') {
		BPntToMrk(tmpmark);
		BKillMrk(tmpmark);
		return(TRUE);
		}
	    else {
		BPntToMrk(tmpmark);
		BKillMrk(tmpmark);
		return(FALSE);
		}
	   }
	else {
	    BPntToMrk(tmpmark);
	    BKillMrk(tmpmark);
	    return(FALSE);
	    }
	}
    else {
	BPntToMrk(tmpmark);
	BKillMrk(tmpmark);
	return(FALSE);
	}
    }
static blinep()
{
    int tmpmark,IsNL(),IsWhite();

    tmpmark = BCreMrk();
    ToBegLine();
    if (!BIsEnd()) MovePast(IsWhite,FORWARD);
    if ((!BIsEnd()) && (Buff() == NL)) {
	BPntToMrk(tmpmark);
	BKillMrk(tmpmark);
	return(TRUE);
	}
    else {
	BPntToMrk(tmpmark);
	BKillMrk(tmpmark);
	return(FALSE);
	}
    }
static getind()
{
    int tmpmark,col,IsWhite();

    tmpmark = BCreMrk();
    ToBegLine();
    MovePast(IsWhite,FORWARD);
    col = BGetCol();
    BPntToMrk(tmpmark);
    BKillMrk(tmpmark);
#ifdef DEBUG
    Debug("***in getind(): col = ",col);
#endif
    return(col);
    }

