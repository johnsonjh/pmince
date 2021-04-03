/* COMMANDS.C -- Scribble command interpeter

	written January 1981 by Craig A. Finseth
	Copyright (c) 1981 by Mark of the Unicorn Inc.
*/
	
#include "scribble.gbl"

#ifdef UNIX
FLAG CheckVar(), CheckEnv(), CheckSec();
#endif

SCommand(tkn)				/* process a command */
	char *tkn;
{
	STRING *token, *token2;
	char which;
	INDLIST *iptr, *iptr2, *iptrold;
	NOTELIST *nptr;
	MICA LGetCol(), amt;
	int CloseFoots();

	if (SCComp(tkn,"\\")) {
		if (SCComp(env.name,"description")) {
			BreakTok();
			if (LGetCol()>=env.cureleft) BreakLine();
			else LPutWhite(env.cureleft-LGetCol());
			}
		return;
		}

/* ENVIRONMENTS */

	if (CheckEnv(tkn)) return;

/* BEGIN and END */

	if (SCComp(tkn,"begin")) {
		token=GetOneArg(RETURN);
		if (SCompToC(token,"comment")) {
			SFree(token);
			FlushArg("comment");
			return;
			}
		openneeded=FALSE;
		if (CheckEnv(token)) env.howclose='e';
		else TError("Unknown environment '",SSToC(token),"'.");
		SFree(token);
		return;
		}
	if (SCComp(tkn,"end")) {
		token=GetOneArg(RETURN);
		if (env.howclose=='e' && SCComp(env.name,token)) (*env.closeproc)();
		else {
			TError("Incorrect Environment Closing.  Tried to end a '",
				SSToC(token),"' environment.");
			TPuts("A '");
			TPuts(env.name);
			TPuts("' environment is open, and it should be closed with '");
			if (env.howclose=='e') TPuts("@end");
 			else TPut(env.howclose);
			TPuts("'.\n");
			}
		SFree(token);
		return;
		}

/* INLINE Environment */

	if (InLine(tkn,"+",SUPERON,SUPEROFF)) return;
	if (InLine(tkn,"-",SUBON,SUBOFF)) return;
	if (InLine(tkn,"b",BOLDON,BOLDOFF)) return;
	if (InLine(tkn,"i",ITALON,ITALOFF)) return;
	if (InLine(tkn,"p",BITALON,BITALOFF)) return;
	if (InLine(tkn,"r",ROMANON,ROMANOFF)) return;
	if (InLine(tkn,"t",ROMANON,ROMANOFF)) return;
	if (InLine(tkn,"u",UNNBON,UNNBOFF)) return;
	if (InLine(tkn,"ux",UNALLON,UNALLOFF)) return;
	if (InLine(tkn,"un",UNANON,UNANOFF)) return;

/* I/O Commands */

	if (SCComp(tkn,"comment")) {
		GetOneArg(ARGFLUSH);
		return;
		}
	if (SCComp(tkn,"include")) {
		token=GetOneArg(RETURN);
		IInclude(SSToC(token));
		SFree(token);
		if (!MulComp(token,"con:","tty:")) {
			TPuts("\nIncluding ");
			TPuts(IGetFile());
			TNL();
			}
		Scribble();
		return;
		}
	if (SCComp(tkn,"device")) {
		DSetType('n',SSToC(token=GetOneArg(RETURN)));
		SFree(token);
		return;
		}
	if (SCComp(tkn,"message")) {
		TNL();
		GetOneArg(PRINT);
		return;
		}

/* HEADINGS, FOOTINGS, and FOOTNOTES */

	if (which=MulComp(tkn,"pageheading","pagefooting")) {
		DoHeading(which);
		return;
		}
	if (SCComp(tkn,"note") || (sty.footstyle=='e' && SCComp(tkn,"foot"))) {
		if (sty.footpush) {
			PutInit();
			Put(SUPERON);
			PutNum(NULL,++footnum,NULL);
			Put(SUPEROFF);
			PutFini();
			}
		else PutNum("[",++footnum,"]");
		MemSpace(EndSpace);
		nptr=MemGet(sizeof(*nptr));
		if (!notehead) notetail=notehead=nptr;
		else notetail->notenextptr=nptr;
		notetail=nptr;
		nptr->notenum=footnum;
		nptr->noteptr=GetOneArg(RETURN);
		nptr->notenextptr=NULL;
		MemSpace(PageSpace);
		return;
		}
	if (SCComp(tkn,"foot")) {
		if (sty.footstyle=='i') {
			EPush();
			env.closeproc= &CloseFoots;
			env.name="footnote";
			Put('[');
			return;
			}
		if (sty.footpush) {
			PutInit();
			Put(SUPERON);
			PutNum(NULL,++footnum,NULL);
			Put(SUPEROFF);
			PutFini();
			}
		else PutNum("[",++footnum,"]");
		IPushBuf();
		PFootnote();

		NormalEnv("footnote",FALSE,TRUE,sty.prefjust);
		env.closeproc= &CloseFoots;
		env.preveleft=env.eleft=env.cureleft=pag.pleft;
		env.eright=pag.pright;

		if (nofootnotes) {
			nofootnotes=FALSE;
			PPutVert(LINEHEIGHT);
			LPutToken(SCToS("----------"));
			BreakLine();
			}
		PPutVert(LINEHEIGHT);
		PutNum(NULL,footnum,".  ");
		if (!GetOpen()) {
			PNoFootnote();
			IPopBuf();
			}
		return;
		}
#ifdef LARGE
	if (SCComp(tkn,"index")) {
		MemSpace(EndSpace);
		iptr2=MemGet(sizeof(*iptr));
		iptr2->indpage=(pagenum==0)? 1 : pagenum;
		token=GetOneArg(RETURN);

		if (!indhead) {
			indhead=iptr2;
			iptr2->indnextptr=NULL;
			}
		else {
			iptr=iptrold=indhead;
			while (iptr && SCompGT(token,iptr->indname)) {
				iptrold=iptr;
				iptr=iptr->indnextptr;
				}
			if (iptr && SComp(token,iptr->indname)) {
				SFree(token);
				token=NULL;
				do {
					iptrold=iptr;
					iptr=iptr->indnextptr;
					} while (iptr && !iptr->indname);
				}
			if (iptr==indhead) {
				iptr2->indnextptr=indhead;
				indhead=iptr2;
				}
			else {
				iptr2->indnextptr=iptrold->indnextptr;
				iptrold->indnextptr=iptr2;
				}
			}
		iptr2->indname=token;
		MemSpace(PageSpace);
		return;
		}
#endif

/* CHAPTERS, SECTIONS, and SUBSECTIONS */

	if (CheckSec(tkn)) return;

/* VARIABLE STORING AND PRINTING */

	if (CheckVar(tkn)) return;

/* CASE */

#ifdef LARGE
	if (SCComp(tkn,"case")) {
		DoCase();
		return;
		}
#endif

/* STYLE */

#ifdef LARGE
	if (SCComp(tkn,"style")) {
		DoStyle();
		return;
		}
#endif

/* WHITESPACE COMMANDS */

	if (SCComp(tkn,"newpage")) {
		BreakLine();
		PNewPage();
		return;
		}
#ifdef LARGE
	if (SCComp(tkn,"blankpage")) {
		ArgOn("@BlankPage");
		blankpages+=GetNumArg();
		ArgOff("@BlankPage");
		return;
		}
#endif
#ifdef LARGE
	if (SCComp(tkn,"blankspace")) {
		BreakLine();
		ArgOn("@BlankSpace");
		amt=GetDimArg();
		ArgOff("@BlankSpace");
		PPutVert(amt);
		return;
		}
#endif
	if (SCComp(tkn,"w")) {
		GetOneArg(HOLD);
		return;
		}

/* COMMAND NOT FOUND */

	TError("Unknown command '",tkn,"'.");
	openneeded=FALSE;
	IAppTok();
	}

FLAG
CheckVar(tkn)				/* do the variable processing commands */
	char *tkn;
{
	STRING *token, *token2, *sptr;
	char which, *secstr, *SSToC();
	int oldval, val;
	VALLIST *vptr;

	if (SCComp(tkn,"string")) {
		token=NULL;
		if (!ArgOn("@String") || !(token=GetArg(RETURN)) ||
			 !(token2=GetArg(RETURN))) {
			if (token) SFree(token);
			TError("@String requires two arguments.",NULL,NULL);
			return(TRUE);
			}
		if (argquoted) sptr=token2;
		else {
			if (!(vptr=FindVar(token2))) {
				TError("@String: Variable not found '",SSToC(token2),"'.");
				sptr=token2;
				}
			else {
				SFree(token2);
				if (vptr->valtype=='s') sptr=SCToS(SSToC(vptr->value));
				else sptr=SNToS(vptr->value);
				}
			}
		SetVar(SSToC(token),'s',sptr);
		SFree(token);
		ArgOff("@String");
		return(TRUE);
		}
	if (SCComp(tkn,"value")) {
		token=GetOneArg(RETURN);
		vptr=FindVar(token);
		if (!vptr) TError("@Value: Variable not found '",SSToC(token),"'.");
		else if (vptr->valtype=='s') PutStr(SSToC(vptr->value));
		else PutNum(NULL,vptr->value,NULL);
		SFree(token);
		return(TRUE);
		}
	if (SCComp(tkn,"set")) {
		token=NULL;
		if (!ArgOn("@Set") || !(token=GetArg(RETURN)) ||
			 !(token2=GetArg(RETURN))) {
			if (token) SFree(token);
			TError("@Set requires two arguments.",NULL,NULL);
			return(TRUE);
			}
		which= *SSToC(token2);
		if (which=='+' || which=='-') {
			sptr=SCToS(SSToC(token2)+1);
			SFree(token2);
			token2=sptr;
			}

		if (IsNum(token2)) val=SSToN(token2);
		else val=GetNumVal(token2);
		SFree(token2);
		oldval=GetNumVal(token);

		if (which=='+') oldval+=val;
		else if (which=='-') oldval-=val;
		else oldval=val;
		SetVar(SSToC(token),'n',oldval);
		SFree(token);
		ArgOff("@Set");
		return(TRUE);
		}
	secstr="chapter section subsection paragraph appendix appendixsection ";
	if (SCComp(tkn,"ref")) {
		val=Index((token=GetOneArg(RETURN)),secstr);
		if (val>5) {
			vptr=FindVar(token);
			if (!vptr) TError("@Ref: Variable not found '",
				SSToC(token),"'.");
			else if (vptr->valtype=='n') PutNum(NULL,vptr->value,NULL);
			else TError("@Ref: '",SSToC(token),
				"' does not contain a number.");
			}
		else Template("csbpan"[val]);
		SFree(token);
		return(TRUE);
		}
	if (SCComp(tkn,"title")) {
		vptr=NULL;
		switch (Index((token=GetOneArg(RETURN)),secstr)) {

		case 0:
			vptr=chaptitle;
			break;
		case 1:
			vptr=sectitle;
			break;
		case 2:
			vptr=subtitle;
			break;
		case 3:
			vptr=paratitle;
			break;
		case 4:
			vptr=apptitle;
			break;
		case 5:
			vptr=asectitle;
			break;
		default:
			TError("@Title: Unknown option '",SSToC(token),"'.");
			break;
			}
		if (vptr) PutStr(vptr->value);
		SFree(token);
		return;
		}
	}

/* END OF COMMANDS.C */

