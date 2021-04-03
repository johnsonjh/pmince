/* AUXIL.C -- Scribble auxilliary routines

	written January 1981 by Craig A. Finseth
	Copyright (c) 1981 by Mark of the Unicorn Inc.
*/
	
#include "scribble.gbl"

#ifndef CPM
STRING *GetArg();
unsigned GetNumArg();
char MulComp();
FLAG Test();
#endif

EnvInit()					/* initialize environment */
{
	int ParaStan(), CloseStan();

	env.name="text";
	env.howclose=NUL;
	env.preveleft=pag.pleft;
	env.eleft=env.cureleft=pag.pleft;
	env.eright=pag.pright;
	env.linespacing=sty.spacing;
	env.isfill=TRUE;
	env.iswrap=TRUE;
	env.justifytype=sty.prefjust;
	env.iswhiteintact=FALSE;
	env.paraproc=ParaStan;
	env.closeproc=CloseStan;
	env.enumcount=1;
	env.itemlevel=0;
	env.closechar=NUL;
	}

/* -------------------------------------------------- */

/* Argument routines */

FlushArg(name)				/* get rid of the argument */
	char *name;
{
	char *IGetTok();
	STRING *token;

	INextTok();
	repeat {
		while (IGetType()!=COMMAND)
			if (IGetType()==EOFT) {
				IUnGet();
				return;
				}
			else INextTok();
		INextTok();
		if (SCComp(IGetTok(),"end")) {
			if (SCompToC(token=GetOneArg(RETURN),name)) {
				SFree(token);
				return;
				}
			if (token) SFree(token);
			}
		INextTok();
		}
	}

FLAG
ArgOn(cmd)				/* start up a GetArg */
	char *cmd;
{
	char *IGetTok();

	if (getarginited) TIntError("ArgOnned");
	INextTok();
	if (IGetType()==EOFT) {
		IUnGet();
		return(FALSE);
		}
	if ((getargclose=MatchOpen(*IGetTok()))==NUL) {
		TError("Missing argument to '",cmd,"'.");
		return(FALSE);
		}
	getarginited=TRUE;
	INextTok();
	return(TRUE);
	}

ArgOff(cmd)				/* finish up a GetArg */
	char *cmd;
{
	if (getarginited && GetArg(ARGFLUSH)) TError("Extra arguments to '",cmd,"'.");
	getarginited=FALSE;
	}

STRING *
GetArg(what)			/* return an argument, token by token */
	char what;		/* FLUSH, RETURN, SAMPLE (return delim) */
{
	char *IGetTok();
	STRING *token;

	if (!getarginited) return(NULL);

	argquoted=FALSE;
	while (IsType(WHITESPACE,NEWLINE,HEADERCENTER,HEADERRIGHT,HEADERLINE,
		 NUL) || MulComp(IGetTok(),"=",",") || MulComp(IGetTok(),"/",";")) {
		INextTok();
		}
	if (IGetType()==EOFT) {
		IUnGet();
		getarginited=FALSE;
		return(NULL);
		}
	if (IGetType()==DELIM) {
		if (getargclose == *IGetTok()) {
			getarginited=FALSE;
			return(NULL);
			}
		if (what==SAMPLE) return(*IGetTok());
		argquoted=TRUE;
		IUnGet();
		token=GetOneArg(what);
		INextTok();
		return(token);
		}
	else if (what==SAMPLE) return(NUL);
	IPushBuf();
	while (IsType(TOKEN,COMMAND,DELIM,NUL) || (IGetType()==TOKENBREAK &&
		 !MulComp(IGetTok(),"=",",") && !MulComp(IGetTok(),"/",";"))) {
		if (IGetType()==DELIM && *IGetTok()==getargclose) break;
		if (what==RETURN) IAppTok();
		INextTok();
		}
	if (IGetType()==EOFT) {
		IUnGet();
		getarginted=FALSE;
		}
	token=IGetBuf();
	if (what==ARGFLUSH) SFree(token);
	IPopBuf();
	return(token);
	}

PushArg()				/* save GetArg's state */
{
	saveclose=getargclose;
	getarginited=FALSE;
	}

PopArg()				/* restore the state */
{
	getargclose=saveclose;
	getarginited=TRUE;
	}

#ifdef LARGE
MICA
GetDimArg()			/* process a dimensioned, numeric argument */
{
	unsigned n;
	MICA m;
	STRING *token;

	n=GetNumArg();
	if (!(token=GetArg(RETURN))) {
		TError("Missing dimension.",NULL,NULL);
		return(0);
		}
	switch (Index(token,"in inch inches cm mm point points pica picas em ems char chars character characters line lines micas ")) {

	case 0:	case 1:	case 2:
		m=INCH;
		break;
	case 3:
		m=CENTIMETER;
		break;
	case 4:
		m=CENTIMETER/10;
		break;
	case 5:	case 6:
		m=POINT;
		break;
	case 7:	case 8:
		m=PICA;
		break;
	case 9:	case 10:
		m=Width('m');
		break;
	case 11:	case 12:	case 13:	case 14:
		m=CHARWIDTH;
		break;
	case 15:	case 16:
		m=LINEHEIGHT;
		break;
	case 17:
		m=1;
		break;
	default:
		TError("Unknown unit of measure '",SSToC(token),"'.");
		m=0;
		}
	m*=n;
	SFree(token);
	return(m);
	}
#endif

#ifdef LARGE
FLAG
GetFlagArg()				/* process an "Yes" or "No" argument */
{
	STRING *token;
	FLAG arg;

	token=GetArg(RETURN);
	if (token && SCompToC(token,"yes")) arg=TRUE;
	else if (token && SCompToC(token,"no")) arg=FALSE;
	else {
		TError("'Yes' or 'No' argument required. '",SSToC(token),
			"' was given.");
		arg=FALSE;
		}
	if (token) SFree(token);
	return(arg);
	}
#endif

#ifdef LARGE
unsigned
GetNumArg()				/* process a number in the argument */
{
	unsigned n;
	STRING *token;

	if (token=GetArg(RETURN)) {
		n=SSToN(token);
		SFree(token);
		return(n);
		}
	else {
		TError("Missing numeric argument.",NULL,NULL);
		return(0);
		}
	}
#endif

STRING *
GetOneArg(arg)				/* return the argument as a single token */
	char arg;				/* HOLD, PRINT, RETURN, ARGFLUSH */
{
	char howend;
	char *IGetTok();
	STRING *token;

	INextTok();
	howend=MatchOpen(*IGetTok());
	if (howend==NUL) {
		TError("Missing single argument.",NULL,NULL);
		return(SCToS(IGetTok()));
		}
	if (arg==RETURN || arg==PRINT) IPushBuf();
	INextTok();
	newl=FALSE;
	while (IGetType()!=EOFT && (IGetType()!=DELIM || *IGetTok()!=howend)) {
		if (IGetType()==NEWLINE) {
			if (newl) {
				TError("Warning: Argument terminated by paragraph break.",
					NULL,NULL);
				break;
				}
			newl=TRUE;
			}
		else newl=FALSE;
		if (arg!=ARGFLUSH) IAppTok();
		INextTok();
		}
	newl=FALSE;
	if (IGetType()==EOFT) IUnGet();
	if (arg==RETURN) {		/* return the argument */
		token=IGetBuf();
		IPopBuf();
		return(token);
		}
	else if (arg==PRINT) {	/* print it on the console */
		token=IGetBuf();
		IPopBuf();
		TPuts(SSToC(token));
		SFree(token);
		}
	return(TRUE);
	}

/* -------------------------------------------------- */

/* standard environment routines */

FLAG
GetOpen()					/* pick up the opening delimiter */
{
	char *IGetTok();

	openneeded=FALSE;
	INextTok();
	if (IGetType()!=DELIM || (env.howclose=MatchOpen(*IGetTok()))==NUL) {
		TError("Bad opening character '",IGetTok(),"'.");
		EPop();
		return(FALSE);
		}
	return(TRUE);
	}

FLAG
InLine(token,trialname,onchar,offchar)		/* do an inline environment */
	char *token;
	char *trialname;
	char onchar, offchar;
{
	int CloseInLine();

	if (SCComp(token,trialname)) {
		Put(onchar);
		EPush();
		env.name=trialname;
		env.closeproc=CloseInLine;
		env.closechar=offchar;
		if (!GetOpen()) Put(offchar);
		return(TRUE);
		}
	return(FALSE);
	}

char
MulComp(token,name1,name2)	/* do a compare on more than one thing */
	char *token;
	char *name1, *name2;
{
	if (SCComp(token,name1)) return(1);
	else if (SCComp(token,name2)) return(2);
	return(NUL);
	}

char
MulTest(token,name1,name2)	/* do a Test on more than one thing */
	char *token;
	char *name1, *name2;
{
	if (Test(token,name1)) return(1);
	else if (Test(token,name2)) return(2);
	return(NUL);
	}

NormalEnv(tname,whitep,fillp,just)	/* set up a "normal" environment */
	char *tname, just;
	FLAG whitep, fillp;
{
	int ParaStan();

	EPush();
	env.name=tname;
	env.howclose=NUL;
	env.iswhiteintact=whitep;
	env.isfill=fillp;
	env.iswrap=TRUE;
	env.justifytype=just;
	env.linespacing=LINEHEIGHT;
	env.paraproc=ParaStan;
	}

StartEnv()				/* start an environment */
{
	if (openneeded && !GetOpen()) return;
	BreakLine();
	PPutVert(sty.above);
	if (env.paraproc) (*env.paraproc)();
	if (env.isfill) FlushWhite();
	}

FLAG
Test(token,trialname)		/* test for an environment and do some set up */
	char *token;
	char *trialname;
{
	int CloseStan();

	if (SCComp(token,trialname)) {
		EPush();
		env.name=trialname;
		env.iswhiteintact=FALSE;
		env.isfill=TRUE;
		env.iswrap=TRUE;
		env.justifytype='l';
		env.paraproc=NULL;
		env.closeproc=CloseStan;
		env.preveleft=env.eleft;
		return(TRUE);
		}
	return(FALSE);
	}

/* -------------------------------------------------- */

/* routines to handle paragraph breaks */

ParaEnum()				/* enumerate paragraphs */
{
	MICA amt;
	char nums[10];

	env.eleft=env.preveleft;
	SNToC(env.enumcount++,nums);
	strcat(nums,".");
	PutStr(nums);
	amt=LGetLength(nums,env.preveleft);
	BreakTok();
	LPutWhite(env.cureleft-(env.preveleft+amt));
	}

ParaItem()				/* itemize paragraph */
{
	char chr;

	env.eleft=env.preveleft;
	chr= env.itemlevel&1 ? '-' : '*';
	Put(chr);
	BreakTok();
	LPutWhite(env.cureleft-(env.preveleft+Width(chr)));
	}

#ifdef LARGE
ParaLevel()				/* Level environment */
{
	int *indx;

	ParaOut();
	for (indx= &level[0]; *(indx+1); ++indx) PutNum(NULL,(*indx)-1,".");
	PutNum(NULL,(*indx)++,".");
	BreakTok();
	LPutWhite(2*CHARWIDTH);
	if (sty.levhang) env.cureleft=LGetCol();
	}
#endif

ParaOut()					/* outdented paragraphs */
{
	env.eleft=env.preveleft;
	LPutWhite(0);
	}

ParaStan()				/* standard paragraph breaker */
{
	LPutWhite(sty.indent);
	}

/* -------------------------------------------------- */

/* Routines that close out environments */

CloseCase()				/* close out a Case statement */
{
	INextTok();
	PopArg();
	while (GetArg(ARGFLUSH));
	EPop();
	}

CloseFoots()				/* close footnote environments */
{
	if (sty.footstyle=='i') Put(']');
	else {
		BreakLine();
		PNoFootnote();
		IPopBuf();
		}
	EPop();
	}

CloseInLine()				/* in-line environment closer */
{
	Put(env.closechar);
	EPop();
	}

#ifdef LARGE
CloseLevel()				/* Level environment closer */
{
	int *indx;

	for (indx= &level[0]; *indx; ++indx);
	if (indx> &level[0]) *(--indx)=NULL;
	CloseStan();
	}
#endif

CloseSec()				/* sectioning environment closer */
{
	Put(env.closechar);
	BreakLine();
	PPutVert(2*LINEHEIGHT);
	EPop();
	}

CloseStan()				/* standard environment closer */
{
	BreakLine();
	PPutVert(sty.below);
	EPop();
	}

/* END OF AUXIL.C */

