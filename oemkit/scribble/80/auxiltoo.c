/* AUXILTOO.C -- More Scribble auxilliary routines

	written January 1981 by Craig A. Finseth
	Copyright (c) 1981 by Mark of the Unicorn Inc.
*/
	
#include "scribble.gbl"

#ifdef UNIX
MICA Width();
char MatchOpen();
FLAG IsNum(), IsType();
#endif

HeaderInit()				/* initialize pageheadings  */
{
	headeven=SCToS("\203\222\214");
	headodd=SCToS("\203\222\214");
	footeven=SCToS("\203- @Value(Page) -\222\214");
	footodd=SCToS("\203- @Value(Page) -\222\214");
	}

PrintNotes()				/* print any trailing footnotes */
{
	int ParaStan();

	if (!notehead) return;

	NormalEnv("notes",FALSE,TRUE,sty.prefjust);
	env.preveleft=env.eleft=env.cureleft=pag.pleft;
	env.eright=pag.pright;

	BreakLine();
	PPutVert(LINEHEIGHT);
	LPutToken(SCToS("----------"));
	BreakLine();

	while (notehead) {
		PPutVert(LINEHEIGHT);
		PutNum(NULL,notehead->notenum,".  ");
		IUse(SSToC(notehead->noteptr),"endnotes");
		Scribble();
		BreakLine();
		notehead=notehead->notenextptr;
		}
	BreakLine();
	EPop();
	}

#ifdef LARGE
PrintIndex()				/* print the index */
{
	int ParaStan();

	if (!indhead) return;

	TPuts("\n\nIndex\n\n     ");

	PNewPage();
	PPutVert(4*LINEHEIGHT);

	NormalEnv("index",TRUE,FALSE,'c');
	env.preveleft=env.cureleft=env.eleft=pag.pleft;
	env.eright=pag.pright;
	PutStr("Index");
	BreakLine();

	PPutVert(2*LINEHEIGHT);
	env.justifytype='l';

	while (indhead) {
		PutInit();
		if (indhead->indname) {
			BreakLine();
			PutStr(SSToC(indhead->indname));
			Put(SP);
			}
		else Put(',');
		PutNum(" ",indhead->indpage,NULL);
		PutFini();
		indhead=indhead->indnextptr;
		}
	BreakLine();
	EPop();
	}
#endif

TableOfContents()			/* generate the table of contents */
{
	char type, oldtype;
	MICA LGetCol();

	if (!titlehead) return;

	TPuts("\n\nTable of Contents\n\n     ");

	PNewPage();
	PPutVert(4*LINEHEIGHT);

	NormalEnv("contents",FALSE,FALSE,'c');
	env.preveleft=env.eleft=pag.pleft;
	env.cureleft=env.preveleft+3*INCH/2;
	env.eright=pag.pright-INCH;
	PutStr("Table of Contents");
	BreakLine();

	PPutVert(2*LINEHEIGHT);
	env.justifytype='l';

	oldtype='c';
	while (titlehead) {
		type=titlehead->enttype;
		if (oldtype!=type || type=='c' || type=='a') {
			BreakLine();
			PPutVert(LINEHEIGHT);
			}
		oldtype=type;
		env.eleft=env.preveleft;
		switch(type) {

		case 'c':
			PutStr("Chapter ");
			chapter->value=titlehead->entnum;
			break;
		case 's':
			LPutWhite(3*CHARWIDTH);
			section->value=titlehead->entnum;
			break;
		case 'b':
			LPutWhite(6*CHARWIDTH);
			subsection->value=titlehead->entnum;
			break;
		case 'p':
			LPutWhite(9*CHARWIDTH);
			paragraph->value=titlehead->entnum;
			break;
		case 'a':
			PutStr("Appendix ");
			appendix->value=titlehead->entnum;
			break;
		case 'n':
			LPutWhite(3*CHARWIDTH);
			asection->value=titlehead->entnum;
			}
		SecStart(type);
		Template(type);
		BreakTok();
		PutStr(SSToC(titlehead->entname));
		BreakTok();
		if (LGetCol()>env.eright-HALFINCH) BreakLine();
		LPutWhite(env.eright+HALFINCH-LGetCol());
		env.iswrap=FALSE;
		PutNum(NULL,titlehead->entpage,NULL);
		BreakLine();
		env.iswrap=TRUE;
		titlehead=titlehead->entnextptr;
		}
	EPop();
	}

/* -------------------------------------------------- */

TITLELIST *
AddToC()						/* add a Table of Contents entry */
{
	TITLELIST *tptr;

	MemSpace(EndSpace);
	tptr=MemGet(sizeof(*tptr));
	if (!titlehead) titletail=titlehead=tptr;
	else {
		titletail->entnextptr=tptr;
		titletail=tptr;
		}
	tptr->entnextptr=NULL;
	tptr->entname=GetOneArg(RETURN);
	MemSpace(PageSpace);
	return(tptr);
	}

BreakLine()
{
	BreakTok();
	LBreakLine();
	}

BreakTok()				/* process a token break */
{
	STRING *token;
	char *SSToC();

	if (*SSToC(token=IGetBuf())!=NUL) {
		IClearBuf();
		LPutToken(token);
		}
	else SFree(token);
	}

DoSent()					/* Process an sentence break if desired */
{
	STRING *token;
	char *SSToC();

	if (islower(*SSToC(token=IGetBuf()))) Put(SENTENCEBREAK);
	SFree(token);
	}

VALLIST *
FindVar(name)					/* find a variable */
	STRING *name;
{
	VALLIST *ptr;

	for (ptr=valhead; ptr && !SComp(ptr->valname,name); ptr=ptr->valnextptr);
	return(ptr);
	}

FlushWhite()				/* flush whitespace after paragraph breaks */
{
	do {
		INextTok();
		} while (IsType(WHITESPACE,NEWLINE,NUL));
	IUnGet();
	}

int
GetNumVal(name)				/* return a numeric value or zero */
	STRING *name;
{
	VALLIST *vptr;

	if (!(vptr=FindVar(name))) return(0);
	if (vptr->valtype=='s' && !IsNum(vptr->value)) {
		TError("Non-numeric value '",SSToC(name),"'.");
		return(0);
		}
	if (vptr->valtype=='s') return(SSToN(vptr->value));
	return(vptr->value);
	}

int
Index(token,str)			/* look up token in str */
	STRING *token;
	char *str;
{
	int indx;
	char *cptr;

	indx=0;
	while (*str--) {
		cptr=SSToC(token)-1;
		do {
			++str;
			++cptr;
			} while (tolower(*str)==tolower(*cptr));
		if (*str==SP && !*cptr) break;
		while (*str++ != SP);
		++indx;
		}
	return(indx);
	}

FLAG
IsNum(str)					/* does the string look like a number? */
	STRING *str;
{
	char *cptr;

	cptr=SSToC(str);
	if (*cptr=='+' || *cptr=='-') cptr++;
	while (*cptr && isdigit(*cptr)) cptr++;
	return(!*cptr);
	}

FLAG
IsType(args)				/* are we any of these? */
	int args;
{
	int *argp;

	for (argp= &args; *argp; ++argp) if (*argp==IGetType()) return(TRUE);
	return(FALSE);
	}

char
MatchOpen(a)				/* return the matching close character */
	char a;
{
	switch (a) {

	case '(':
		return(')');
	case '[':
		return(']');
	case '{':
		return('}');
	case '<':
		return('>');
	case '"':
		return('"');
	case '`':
	case '\'':
		return('\'');
	default:
		return(NUL);
		}
	}

ProcessSuffix(name,suffix,force)	/* munge a file name */
	char *name, *suffix;
	FLAG force;
{
	char *indx;

	for (indx=name; *indx && *indx!='.'; ++indx);
	if (force) *indx=NUL;
	if (*indx==NUL) strcat(name,suffix);
	}

SecStart(level)			/* initialize the sectioning counters */
	char level;
{
	if (level=='p') return;
	paragraph->value=0;
	SetVar("paragraphtitle",'s',SCToS(""));
	if (level=='b') return;
	subsection->value=0;
	SetVar("subsectiontitle",'s',SCToS(""));
	if (level=='s' || level=='n') return;
	asection->value=0;
	SetVar("appendixsectiontitle",'s',SCToS(""));
	section->value=0;
	SetVar("sectiontitle",'s',SCToS(""));
	}

VALLIST *
SetVar(name,type,val)			/* set a variable to a value, creating */
	char *name;				/* it if necessary */
	char type;
	int val;
{
	VALLIST *vptr;
	STRING *token;

	token=SCToS(name);
	if (!(vptr=FindVar(token))) {
		vptr=MemGet(sizeof(*vptr));
		if (!valhead) valtail=valhead=vptr;
		else {
			valtail->valnextptr=vptr;
			valtail=vptr;
			}
		vptr->valnextptr=NULL;
		vptr->valname=token;
		}
	else {
		if (vptr->valtype=='s' && vptr->value) SFree(vptr->value);
		SFree(token);
		}
	vptr->valtype=type;
	vptr->value=val;
	return(vptr);
	}

MICA
TabWidth(col)					/* return the width of a Tab */
	MICA col;
{
	int i;

	for (i=0; i<MAXTABS && sty.tabcolumns[i]<=col; i++);
		return((i>=MAXTABS) ? dev.stdhoriz : (sty.tabcolumns[i]-col));
	}

Template(what)					/* outputs a section number */
	char what;
{
	char *tmp;

	PutInit();
	if (sty.ischapter) {
		if (what=='a' || what=='n') Put('A'-1+appendix->value);
		else PutNum(NULL,chapter->value,NULL);
		tmp=".";
		}
	else tmp=NULL;

	if (what=='n') PutNum(tmp,asection->value,NULL);
	if (what=='s' || what=='b' || what=='p') PutNum(tmp,section->value,NULL);
	if (what=='b' || what=='p') PutNum(".",subsection->value,NULL);
	if (what=='p') PutNum(".",paragraph->value,NULL);
	PutFini();
	}

MICA
Width(chr)					/* return the width of the character */
	char chr;					/* chr cannot be a Tab */
{
	if (chr>=SP && chr<=DEL)
		if (dev.isprop) return(widthtable[chr]);
		else return(dev.stdhoriz);
	return(0);
	}

/* -------------------------------------------------- */

TBell()				/* ring the bell */
{
	TPut(BELL);
	}

TError(a,b,c)			/* print an error message */
	char *a, *b, *c;
{
	TPuts("\nError: ");
	if (a) TPuts(a);
	if (b) TPuts(b);
	if (c) TPuts(c);
	TPuts("\nerror was on line ");
	TPutn(IGetLine());
	TPuts(" of ");
	TPuts(IGetFile());
	TNL();
	}

TFini()				/* finish up writing things */
{
	}

char
TGetChr()				/* get a character */
{
	char chr;

#ifdef CPM
	chr=bios(3);
#endif
#ifdef UNIX
	chr=getchar();
#endif
	if (chr==3) exit(1);
	return(chr);
	}

TInit()				/* initialize the terminal */
{
	}

TIntError(msg)			/* print an internal error message */
	char *msg;
{
	TPuts("\nInternal Error:  ");
	TPuts(msg);
	TPuts("\nerror was on line ");
	TPutn(IGetLine());
	TPuts(" of ");
	TPuts(IGetFile());
	TNL();
	}

TNL()				/* send a Newline */
{
	putchar('\n');
	}

TPut(chr)				/* put a character */
	char chr;
{
	putchar(chr);
	}

TPutn(n)				/* print n on the terminal */
	unsigned n;
{
	if (n>9) TPutn(n/10);
	putchar(n%10+'0');
	}

TPuts(str)			/* put a string */
	char *str;
{
	while (*str) TPut(*str++);
	}

/* end of AUXILTOO.C */

