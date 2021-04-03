/* COMMTOO.C -- Scribble command interpeter, continued

	written March 1981 by Craig A. Finseth
	Copyright (c) 1981 by Mark of the Unicorn Inc.
*/

#include "scribble.gbl"

FLAG
CheckEnv(tkn)				/* deal with environments */
	char *tkn;
{
	char which;
	int *indx;
	int ParaOut(), ParaEnum(), ParaItem(), ParaStan(), CloseSec();
#ifdef LARGE
	int ParaLevel(), CloseLevel();
#endif

	if ((which=MulTest(tkn,"address","flushleft")) || Test(tkn,"closing")) {
		env.isfill=FALSE;
		env.iswrap=FALSE;
		env.iswhiteintact=TRUE;
		if (which!=2) env.preveleft=env.eleft=env.cureleft=
			(pag.pright+pag.pleft)/2;
		StartEnv();
		return(TRUE);
		}
	if (which=MulTest(tkn,"center","flushright")) {
		env.isfill=FALSE;
		env.iswrap=FALSE;
		env.iswhiteintact=TRUE;
		env.justifytype= (which==1)? 'c' : 'r';
		StartEnv();
		return(TRUE);
		}
	if (which=MulTest(tkn,"description","undent")) {
		if (which==1) {
			env.eleft=env.cureleft+=(env.eright-env.cureleft)/4;
			env.linespacing=LINEHEIGHT;
			}
		else env.eleft=env.cureleft+=sty.indent;
		env.justifytype=sty.prefjust;
		env.paraproc=ParaOut;
		StartEnv();
		return(TRUE);
		}
	if (MulTest(tkn,"display","example")) {
		env.eleft=env.cureleft+=HALFINCH;
		env.eright-=HALFINCH/2;
		env.isfill=FALSE;
		env.iswhiteintact=TRUE;
		StartEnv();
		return(TRUE);
		}
	if (Test(tkn,"enumerate")) {
		env.preveleft=env.cureleft+2*CHARWIDTH;
		env.eleft=env.cureleft+=6*CHARWIDTH;
		env.justifytype=sty.prefjust;
		env.enumcount=1;
		env.paraproc=ParaEnum;
		StartEnv();
		return(TRUE);
		}
	if (MulTest(tkn,"format","verbatim")) {
		env.linespacing=LINEHEIGHT;
		env.isfill=FALSE;
		env.iswrap=FALSE;
		env.iswhiteintact=TRUE;
		StartEnv();
		return(TRUE);
		}
	if (Test(tkn,"itemize")) {
		env.preveleft=env.cureleft+2*CHARWIDTH;
		env.eleft=env.cureleft+=5*CHARWIDTH;
		env.justifytype=sty.prefjust;
		env.paraproc=ParaItem;
		env.itemlevel++;
		StartEnv();
		return(TRUE);
		}
#ifdef LARGE
	if (Test(tkn,"level")) {
		for (indx= &level[0]; *indx; ++indx);
		if (sty.levindent && indx> &level[0]) {
			if (env.cureleft==env.preveleft) env.cureleft+=HALFINCH;
			env.preveleft=env.eleft=env.cureleft;
			}
		env.justifytype=sty.prefjust;
		env.paraproc=ParaStan;
		*indx=1;
		*(indx+1)=NULL;
		env.paraproc=ParaLevel;
		env.closeproc=CloseLevel;
		StartEnv();
		return(TRUE);
		}
#endif
	if (Test(tkn,"quotation")) {
		env.eleft=env.cureleft+=HALFINCH;
		env.eright-=HALFINCH;
		env.linespacing=LINEHEIGHT;
		env.justifytype=sty.prefjust;
		env.paraproc=ParaStan;
		StartEnv();
		return(TRUE);
		}
	if (Test(tkn,"text")) {
		env.justifytype=sty.prefjust;
		env.paraproc=ParaStan;
		env.linespacing=sty.spacing;
		StartEnv();
		return(TRUE);
		}
	if (Test(tkn,"verse")) {
		env.eleft=env.preveleft+=3*CHARWIDTH;
		env.cureleft=env.preveleft+2*CHARWIDTH;
		env.eright-=10*CHARWIDTH;
		env.linespacing=LINEHEIGHT;
		env.isfill=FALSE;
		env.paraproc=ParaOut;
		StartEnv();
		return(TRUE);
		}
	if ((which=MulTest(tkn,"unnumbered","majorheading")) ||
		 Test(tkn,"heading")) {
		env.isfill=FALSE;
		env.justifytype='c';
		env.closeproc=CloseSec;
		env.closechar=BOLDOFF;
		BreakLine();
		if (which==1) PNewPage();
		StartEnv();
		PPutVert((which ? 6 : 4)*LINEHEIGHT);
		Put(BOLDON);
		return(TRUE);
		}
	if (MulTest(tkn,"prefacesection","subheading")) {
		env.isfill=FALSE;
		env.closeproc=CloseSec;
		env.closechar=UNALLOFF;
		StartEnv();
		PPutVert(4*LINEHEIGHT);
		Put(UNALLON);
		return(TRUE);
		}
	return(FALSE);
	}
	
FLAG
CheckSec(tkn)			/* deal with sectioning commands */
	char *tkn;
{
	TITLELIST *tptr;
	STRING *token;
	char which;

	if (which=MulComp(tkn,"chapter","appendix")) {
		if (!sty.ischapter) {
			TError("@Chapter and @Appendix have been disabled.",NULL,NULL);
			return(TRUE);
			}
		tptr=AddToC();

		if (which==1) {
			tptr->enttype='c';
			tptr->entnum= ++(chapter->value);
			SetVar("chaptertitle",'s',SCToS(SSToC(tptr->entname)));
			}
		else {
			tptr->enttype='a';
			tptr->entnum= ++(appendix->value);
			SetVar("appendixtitle",'s',SCToS(SSToC(tptr->entname)));
			}
		SecStart(tptr->enttype);

		BreakLine();
		PNewPage();
		PPutVert(6*LINEHEIGHT);

		tptr->entpage=pagenum;

		TPuts(" (");
		if (which==1) TPutn(chapter->value);
		else TPut('A'-1+appendix->value);
		TPut(')');

		EPush();
		env.isfill=FALSE;
		env.justifytype='c';
		PutInit();
		Put(BOLDON);
		if (which==1) {
			PutStr("Chapter ");
			Template('c');
			}
		else {
			PutStr("Appendix ");
			Template('a');
			}
		PutFini();
		BreakLine();
		PPutVert(LINEHEIGHT);
		PutInit();
		PutStr(SSToC(tptr->entname));
		Put(BOLDOFF);
		PutFini();
		BreakLine();
		PPutVert(3*LINEHEIGHT);
		EPop();
		return(TRUE);
		}
	if (which=MulComp(tkn,"section","appendixsection")) {
		tptr=AddToC();
		if (which==1) {
			tptr->enttype='s';
			tptr->entnum= ++(section->value);
			SetVar("sectiontitle",'s',SCToS(SSToC(tptr->entname)));
			}
		else {
			tptr->enttype='n';
			tptr->entnum= ++(asection->value);
			SetVar("appendixsectiontitle",'s',SCToS(SSToC(tptr->entname)));
			}
		SecStart(tptr->enttype);

		BreakLine();
		PPutVert(4*LINEHEIGHT);
		EPush();
		env.isfill=FALSE;
		env.justifytype='l';
		PutInit();
		Put(UNALLON);
		Template((which==1) ? 's' : 'n');
		Put(SP);
		PutStr(SSToC(tptr->entname));
		Put(UNALLOFF);
		PutFini();
		BreakLine();

		tptr->entpage=pagenum;

		TPuts(" (");
		if (which==1) TPutn(chapter->value);
		else TPut('A'-1+appendix->value);
		TPut('.');
		TPutn((which==1) ? section->value : asection->value);
		TPut(')');

		PPutVert(2*LINEHEIGHT);
		EPop();
		return(TRUE);
		}
	if (which=MulComp(tkn,"subsection","paragraph")) {
		tptr=AddToC();
		if (which==1) {
			tptr->enttype='b';
			tptr->entnum= ++(subsection->value);
			SetVar("subsectiontitle",'s',SCToS(SSToC(tptr->entname)));
			paragraph->value=0;
			}
		else {
			tptr->enttype='p';
			tptr->entnum= ++(paragraph->value);
			SetVar("paragraphtitle",'s',SCToS(SSToC(tptr->entname)));
			}

		BreakLine();
		PPutVert(2*LINEHEIGHT);
		EPush();
		env.isfill=FALSE;
		env.justifytype='l';
		PutInit();
		Template((which==1) ? 'b' : 'p');
		Put(SP);
		PutStr(SSToC(tptr->entname));
		PutFini();
		BreakLine();

		tptr->entpage=pagenum;

		PPutVert(LINEHEIGHT);
		EPop();
		return(TRUE);
		}
	return(FALSE);
	}

#ifdef LARGE
DoCase()				/* process the @Case command */
{
	STRING *token;
	VALLIST *vptr;
	char dlm;
	int CloseCase();

	if (!ArgOn("@Case")) return;
	vptr=FindVar(token=GetArg(RETURN));
	if (!vptr || vptr->valtype!='s') {
		TError("Undefined or non-string selector variable for @Case '",
			SSToC(token),"'.");
		SFree(token);
		ArgOff("@Case");
		return;
		}
	SFree(token);
	while (token=GetArg(RETURN)) {
		if ((SCompToC(vptr->value,"") && SCompToC(token,"null")) ||
			 SComp(token,vptr->value) || SCompToC(token,"else")) {
			SFree(token);
			if (dlm=GetArg(SAMPLE)) {
				PushArg();
				EPush();
				env.name="case";
				env.howclose=MatchOpen(dlm);
				env.closeproc=CloseCase;
				return;
				}
			else {
				if (!(token=GetArg(RETURN))) {
					TError("Unexpected end of @Case argument",NULL,NULL);
					return;
					}
				while (GetArg(ARGFLUSH));
				IUse(SSToC(token),"case");
				Scribble();
				SFree(token);
				return;
				}
			}
		else if (!GetArg(ARGFLUSH)) return;
		SFree(token);
		}
	}
#endif

#ifdef LARGE
DoStyle()				/* process the @Style command */
{
	STRING *token, *token2;

	ArgOn("@Style");
	while (token=GetArg(RETURN)) {
		switch (Index(token,"above below bottommargin chapters footerspacing footpush headerspacing indent indentation justification leftmargin levelhang levelindent linewidth notes paperlength paperwidth rightmargin scriptpush spacing spread topmargin ")) {

		case 0:					/* above */
			sty.above=GetDimArg();
			break;
		case 1:					/* below */
			sty.below=GetDimArg();
			break;
		case 2:					/* bottom margin */
			pag.pbottom=dev.dheight-(sty.sbottom=GetDimArg());
			break;
		case 3:					/* chapter */
			sty.ischapter=GetFlagArg();
			break;
		case 4:					/* footer spacing */
			sty.footerspace=GetDimArg();
			break;
		case 5:					/* foot push */
			sty.footpush=GetFlagArg();
			break;
		case 6:					/* header spacing */
			sty.headerspace=GetDimArg();
			break;
		case 7:	case 8:			/* indent */
			sty.indent=GetDimArg();
			break;
		case 9:					/* justification */
			env.justifytype=sty.prefjust=(GetFlagArg() ? 'b' : 'l');
			break;
		case 10:					/* left margin */
			env.preveleft=env.eleft=env.cureleft=pag.pleft=GetDimArg();
			break;
		case 11:					/* level hang */
			sty.levhang=GetFlagArg();
			break;
		case 12:					/* level indent */
			sty.levindent=GetFlagArg();
			break;
		case 13:					/* line width */
			env.eright=pag.pright=pag.pleft+GetDimArg();
			break;
		case 14:					/* notes */
			if (!(token2=GetArg(RETURN))) break;
			switch (Index(token2,"endnote footnote inline ")) {

			case 0:
				sty.footstyle='e';
				break;
			case 1:
				sty.footstyle='b';
				break;
			case 2:
				sty.footstyle='i';
				break;
			default:
				TError("Unknown value for Note clause of @Style '",
					SSToC(token2),"'.");
				break;
				}
			SFree(token2);
			break;
		case 15:					/* paper length */
			pag.pbottom=(dev.dheight=GetDimArg())-sty.sbottom;
			break;
		case 16:					/* paper width */
			env.eright=pag.pright=(dev.dwidth=GetDimArg())-sty.sright;
			break;
		case 17:					/* right margin */
			env.eright=pag.pright=dev.dwidth-(sty.sright=GetDimArg());
			break;
		case 18:					/* script push */
			sty.scriptpush=GetFlagArg();
			break;
		case 19:					/* spacing */
			env.linespacing=sty.spacing=GetDimArg();
			break;
		case 20:					/* spread */
			sty.spread=GetDimArg();
			break;
		case 21:					/* top margin */
			pag.ptop=GetDimArg();
			break;
		default:
			TError("Unknown argument to @Style '",SSToC(token),"'.");
			ArgOff("@Style");
			break;
			}
		SFree(token);
		}
	}
#endif

DoHeading(which)		/* process a page heading command */
	char which;
{
	STRING *token, *lhead, *chead, *rhead, *nhead, *ptrs;
	char where, *ptr;

	where='b';
	lhead=SCToS("");
	rhead=SCToS("");
	chead=SCToS("");
	nhead=SCToS("");
	ArgOn(which==1 ? "@PageHeading" : "@PageFooting");

	while (token=GetArg(RETURN)) {
		switch (Index(token,"left line right center even odd ")) {

		case 0:
			SFree(lhead);
			lhead=GetArg(RETURN);
			break;
		case 1:
			SFree(nhead);
			nhead=GetArg(RETURN);
			break;
		case 2:
			SFree(rhead);
			rhead=GetArg(RETURN);
			break;
		case 3:
			SFree(chead);
			chead=GetArg(RETURN);
			break;
		case 4:
			where='e';
			break;
		case 5:
			where='o';
			break;
		default:
			TError("Unknown argument to @PageHeading or @PageFooting '",
				SSToC(token),"'.");
			SFree(token);
			ArgOff(which==1 ? "@PageHeading" : "@PageFooting");
			break;
			}
		SFree(token);
		}

	ptr=MemGet(SLength(lhead)+SLength(chead)+SLength(rhead)+SLength(nhead)+
		(3+1));
	strcpy(ptr,SSToC(lhead));
	strcat(ptr,"\203");
	strcat(ptr,SSToC(chead));
	strcat(ptr,"\222");
	strcat(ptr,SSToC(rhead));
	strcat(ptr,"\214");
	strcat(ptr,SSToC(nhead));

	SFree(lhead);
	SFree(chead);
	SFree(rhead);
	SFree(nhead);

	if (which==1) {
		if (where=='b' || where=='e') {
			SFree(headeven);
			headeven=SCToS(ptr);
			}
		if (where=='b' || where=='o') {
			SFree(headodd);
			headodd=SCToS(ptr);
			}
		}
	else {
		if (where=='b' || where=='e') {
			SFree(footeven);
			footeven=SCToS(ptr);
			}
		if (where=='b' || where=='o') {
			SFree(footodd);
			footodd=SCToS(ptr);
			}
		}
	MemFree(ptr);
	return;
	}

/* END OF COMMTOO.C */

