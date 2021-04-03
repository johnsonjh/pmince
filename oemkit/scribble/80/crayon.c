/* CRAYON.C		Top level of Scribble print driver

	Copyright (C) 1981 by Mark of the Unicorn Inc.
	Created 81.3.12 Gyro

This code interprets the command line and the Scribble output file, and
calls the low-level abstraction to do the printing.  */


#include "crayon.h"


main (argc, argv)
	int argc;
	char **argv;
{
	int ntimes, itime, iarg;
	char *optname;
	struct {
		char *fname;
		int fntimes;
		} files[20];
	int nfiles;
	
	puts ("Crayon Version 1.3, Copyright (C) 1981 by Mark of the Unicorn Inc.\n");
	ntimes = 1;
	nfiles = 0;
	pausep = FALSE;
	title = "";
	portname = NULL;
	quietp = FALSE;
	startpage = 1;
	for (iarg = 1; iarg < argc; ++iarg) {
		if (*argv[iarg] == '-') {
			optname = argv[iarg] + 1;
			if (isdigit (*optname)) ntimes = atoi (optname);
			else if (match (optname, "Page")) {
				startpage = atoi (argv[++iarg]);
				if (!quietp) printf ("Starting at page %d.\n", startpage);
				}
			else if (match (optname, "Pause")) {
				pausep = TRUE;
				puts ("Pausing for manual insertion of each page.\n");
				}
			else if (match (optname, "Port")) {
				portname = argv[++iarg];
				if (!quietp) 
					printf ("Using the '%s' port(s).\n", portname);
				}
			else if (match (optname, "Q")
				    ||  match (optname, "Quiet")) quietp = TRUE;
			else if (match (optname, "Title")
				    ||  match (optname, "T")) {
				title = argv[++iarg];
				if (!quietp) printf ("Title string is '%s'.\n", title);
				}
			else printf ("Unknown command line option: '%s'\n",
					   argv[iarg]);
			}
		else {
			files[nfiles].fname = argv[iarg];
			files[nfiles].fntimes = ntimes;
			++nfiles;
			}
		}
	for (iarg = 0; iarg < nfiles; ++iarg)
		if (PrintFile (files[iarg].fname, files[iarg].fntimes) == ABORTALL)
			break;
	}


Fatal (fmt, a1, a2, a3, a4, a5)	/* lose, lose */
	char *fmt;
	int a1, a2, a3, a4, a5;
{
	printf (fmt, a1, a2, a3, a4, a5);
	exit (1);
	}


PrintFile (fname, ntimes)
	char *fname;
	int ntimes;
{
	char devname[80], i, c, tfname[20];
	FLAG done;
	
	strcpy (&tfname, fname);
	dfltext (&tfname, "FIN");
	if (IOpen (&tfname, INPUT) < 0) {
		printf ("Can't open input file '%s'.\n", &tfname);
		return;
		}
	for (i = 32; i; --i) INext();
	if (IChar() != ':') 
		Fatal ("I only understand Scribble intermediate output files.");
	INext();
	for (i=0; INext() != EOF; ++i) devname[i] = IChar();
	devname[i-2] = NUL;
	INext();
	if (!quietp) {
		if (ntimes == 1)
			printf ("Printing '%s' on device '%s' -- OK? ",
				   &tfname, &devname);
		else printf ("Printing %d copies of '%s' on device '%s' -- OK? ",
				   ntimes, &tfname, &devname);
		if (!Ask ("")) {
			puts (" -- skipping file.\n\n");
			return (ABORTFILE);
			}
		}
	PInit (&devname, portname, !pausep);
	if (PVPos() != 0) {
		FileHeader();
		PFF();
		}
	done = FALSE;
	while (ntimes-- > 0  &&  !done) {
		curpage = 0;
		NextPage();
		AttrInit();
		while (!done  &&  DoLine())
			if (TKbRdy()) switch (c = TGetKb()) {
			
			case ABORTFILE:
				puts ("Aborting this file.\n");
				done = c;
				break;
			case ABORTALL:
				puts ("Aborting all files.\n");
				done = c;
				break;
			case EXIT:
				exit (1);
			default:
				break;
				}
		IRew();
		while (INext() != EOF);			/* skip file header */
		INext();
		PNewPage();
		}
	PFini();
	IClose ();
	return (done);
	}


FileHeader()				/* print file header */
{
	int inch;
	
	inch = PUnMica (2540, HORIZONTAL);
	PMoveH (inch);
	PPrtStr (IName ());
	PMoveH (inch / 2);
	PPrtStr (title);
	PCR();
	PMoveV (PLineV());
	PMoveH (inch * 2);
	PPrtStr ("Crayon version 1.2");	/* vsn of this prog. */
	PCR();
	}


NextPage()				/* process page boundary */
{
	++curpage;
	if (curpage < startpage  &&  !quietp) printf (" [%d]", curpage);
	else {
		if (!quietp)  printf (" %d", curpage);
		PNewPage();
		if (pausep) PagePause();
		}
	}


DoLine()					/* process an input line */
{
	if (ReadLine()) {
		PrintLine();
		return (TRUE);
		}
	return (FALSE);
	}


ReadLine ()				/* read the line and build data structure */
{
	FLAG done;
	FLAG sbrkprev;				/* was previous char a SENTENCEBREAK? */
	char inchar, brklevel, IChar();
	struct token *temptok, *attrtok;
	int tdist;
	
	inchars = intokens = 0;
	superp = subp = FALSE;
	justp = FALSE;
	ForceTok();
	done = FALSE;
	while (!done) {
		PSendChk();
		switch (IChar()) {
		
		case LF:
			if (curpage >= startpage) PLF();
			break;
		case FF:
			NextPage();
			break;
		case VERTSPACE:
			tdist = GetDist (VERTICAL);
			if (curpage >= startpage) PMoveV (tdist);
			break;
		default:
			done = TRUE;
			break;
			}
		if (!done) INext();
		}
	done = sbrkprev = FALSE;
	repeat {
		PSendChk();
		inchar = IChar();
		switch (inchar) {
		
		case CR:
			INext();
		case LF:
		case FF:
			done = TRUE;
			break;
		case EOF:
			return (FALSE);
			break;
		case VERTSPACE:
			done = TRUE;
			break;
		case HORIZSPACE:
			ForceTok();
			temptok = GetTok();
			temptok->toklead += GetDist (HORIZONTAL);
			break;
		case BOLDON:
			attrtok = AttrPush();
			attrtok->tokbold = TRUE;
			ForceTok();
			break;
		case ROMANON:
			attrtok = AttrPush();
			attrtok->tokbold = FALSE;
			attrtok->tokital = FALSE;
			ForceTok();
			break;
		case UNNBON:
		case UNALLON:
		case UNANON:
			attrtok = AttrPush();
			attrtok->tokuscore = inchar;
			ForceTok();
			break;
		case ITALON:
			attrtok = AttrPush();
			attrtok->tokital = TRUE;
			ForceTok();
			break;
		case BITALON:
			attrtok = AttrPush();
			attrtok->tokbold = TRUE;
			attrtok->tokital = TRUE;
			ForceTok();
			break;
		case SUPERON:
			superp = TRUE;
			attrtok = AttrPush();
			attrtok->tokscript = inchar;
			ForceTok();
			break;
		case SUBON:
			subp = TRUE;
			attrtok = AttrPush();
			attrtok->tokscript = inchar;
			ForceTok();
			break;
		case EXITON:
			attrtok = AttrPush();
			attrtok->tokscript = FALSE;
			attrtok->tokuscore = FALSE;
			attrtok->tokbold = FALSE;
			attrtok->tokital = FALSE;
			attrtok->tokjust = FALSE;
			ForceTok();
			break;
		case SUBOFF:
		case SUPEROFF:
		case BITALOFF:
		case ITALOFF:
		case UNANOFF:
		case UNALLOFF:
		case UNNBOFF:
		case ROMANOFF:
		case BOLDOFF:
		case EXITOFF:
			AttrPop();
			ForceTok();
			break;
		case JUSTIFY:
			justp = TRUE;
			just.jlefttok = intokens;
			just.jspace = GetDist (HORIZONTAL);
			just.jnbrk = 0;
			break;
		case TOKENBREAK:
			ForceTok();
			temptok = GetTok();
			temptok->tokjust = TRUE;
			temptok->toklead += PWidChar (' ');
			++just.jnbrk;
			break;
		case SENTENCEBREAK:
			ForceTok();
			temptok = GetTok();
			temptok->toklead += PWidSent();
			break;
		default:
			temptok = GetTok();
			iline[inchars++] = inchar;
			temptok->tokwidth += PWidChar (inchar);
			break;
			}
		if (done) break;
		sbrkprev = inchar == SENTENCEBREAK;
		INext();
		}
	if (sbrkprev  &&  justp) just.jspace += PWidSent();
	itokens[intokens].tokbegin = inchars;
	return (TRUE);
	}


ForceTok()				/* break current token */
{
	forcetok = TRUE;
	}


GetTok()					/* make a new token if needed */
{
	if (forcetok) {
		movmem (&attrstack[attrsp], &itokens[intokens],
			   sizeof(attrstack[0]));
		itokens[intokens].tokbegin = inchars;
		itokens[intokens].tokwidth = 0;
		itokens[intokens].toklead = 0;
		++intokens;
		forcetok = FALSE;
		}
	return (&itokens[intokens-1]);
	}


GetDist (direc)			/* get a distance from the input stream */
	int direc;
{
	unsigned tdist;
	
	tdist = (INext() << 8) + INext();
	if (tdist > MAXMICA)
		Fatal ("Invalid mica value in intermediate file.\n");
	return (PUnMica (tdist, direc));
	}


AttrInit()				/* initialize the attribute stack */
{
	attrsp = 0;
	attrstack[0].tokscript
	 = attrstack[0].tokuscore
	 = attrstack[0].tokbold
	 = attrstack[0].tokital
	 = attrstack[0].tokjust
	 = FALSE;
	}


AttrPush()				/* push attribute stack */
{
	if (attrsp >= ATTRMAX)
		Fatal ("Character attributes nested too deeply.\n");
	else {
		movmem (&attrstack[attrsp], &attrstack[attrsp+1],
			   sizeof(attrstack[0]));
		++attrsp;
		}
	return (&attrstack[attrsp]);
	}


AttrPop()
{
	if (attrsp <= 0) Fatal ("AttrPop: stack underflow\n");
	--attrsp;
	}


PagePause()				/* pause, get fresh page from user */
{
	PForce();
	puts ("\nInsert fresh page; type any character when ready --> ");
	getchar();
	putchar ('\n');
	}


PrintLine()				/* output the accumulated line */
{
	int tvert;
	
	if (curpage < startpage) return;
	if (justp) JustLine();
	tvert = max (1, PLineV() / 2);
	if (superp) {
		PMoveV (-tvert);
		PrintScript (SUPERON);
		PCR();
		PMoveV (tvert);
		}
	PrintScript (FALSE);
	PCR();
	if (subp) {
		PMoveV (tvert);
		PrintScript (SUBON);
		PCR();
		PMoveV (-tvert);
		}
	}


PrintScript (script)		/* print all of super, normal, or subscripts */
	char script;
{
	int thmove, itok;
	
	thmove = 0;
	for (itok=0; itok < intokens; ++itok) {
		if (itokens[itok].tokscript == script) {
			PMoveH (thmove);
			PrintTok (itok, script);
			PSendChk();
			thmove = 0;
			}
		else thmove += itokens[itok].tokwidth + itokens[itok].toklead;
		}
	}


PrintTok (itok, script)		/* print a token */
	int itok;
	char script;
{
	int ichar;
	
	if (itok != 0  &&  !script
				&&  itokens[itok-1].tokuscore == UNALLON
				&&  itokens[itok].tokuscore == UNALLON)
		PSetAttr (UNDERSCORE, UNALLON);
	PMoveH (itokens[itok].toklead);
	PSetAttr (UNDERSCORE, itokens[itok].tokuscore);
	PSetAttr (BOLD, itokens[itok].tokbold);
	PSetAttr (ITALIC, itokens[itok].tokital);
	for (ichar = itokens[itok].tokbegin; ichar < itokens[itok+1].tokbegin;
								  ++ichar)
		PPrtChar (iline[ichar]);
	PSetAttr (UNDERSCORE, FALSE);
	}


JustLine()				/* justify the line */
{
	int totspace, njust;
	int itok, evenspace, oddstart, oddcnt;
	
	totspace = just.jspace;
	njust = just.jnbrk;
	if (!njust) return;
	evenspace = totspace / njust;
	oddstart = ((just.jspace * 17) & 0x7FFF) % njust; 	/* random num */
	oddcnt = totspace % njust;
	PSendChk();
	for (itok = just.jlefttok; itok < intokens; ++itok) 
		if (itokens[itok].tokjust) itokens[itok].toklead += evenspace;
	PSendChk();
	for (itok = just.jlefttok; oddstart > 0; ++itok)
		if (itokens[itok].tokjust) --oddstart;
	PSendChk();
	for (; oddcnt > 0; ++itok) {
		if (itok >= intokens) itok = just.jlefttok;
		if (itokens[itok].tokjust) {
			--oddcnt;
			++itokens[itok].toklead;
			}
		}
	PSendChk();
	}


/* Buffered file input routines */

IOpen (name, mode)			/* open file <name> in <mode> */
	char *name;
	int mode;
{
	int tfd;
	struct fbuf *buf;
	
	buf = &inputbuf;			/* someday this might allocate */
	tfd = open (name, mode);
	if (tfd < 0) return (ERROR);
	buf->fchan = tfd;
	strcpy (&buf->fname, name);
	buf->fnleft = 0;
	INext (buf);
	return (buf);
	}


IChar()					/* return current char */
{
	struct fbuf *buf;
	
	buf = &inputbuf;
	if (!buf->fchan) return (ERROR);
	return (*(buf->fnextp));
	}


INext()					/* get next char & return */
{
	struct fbuf *buf;
	int nread;
	
	buf = &inputbuf;
	if (!buf->fchan) return (ERROR);
	++(buf->fnextp);
	if (--(buf->fnleft) <= 0) {
		nread = read (buf->fchan, &buf->fbuff, BUFBLKS);
		if (nread < 0) {
			IClose();
			return (ERROR);
			}
		buf->fnleft = nread * 128;
		buf->fnextp = &buf->fbuff;
		}
	return (*(buf->fnextp));
	}


IName()					/* return file name */
{
	struct fbuf *buf;
	
	buf = &inputbuf;
	if (!buf  ||  !buf->fchan) return ("");
	return (&buf->fname);
	}


IRew()					/* rewind file */
{
	struct fbuf *buf;
	
	buf = &inputbuf;
	if (!buf  ||  !buf->fchan) return;
	seek (buf->fchan, 0, ABSOLUTE);
	buf->fnleft = 0;
	INext (buf);
	}


IClose()					/* close the file */
{
	struct fbuf *buf;
	
	buf = &inputbuf;
	if (!buf  ||  !buf->fchan) return (ERROR);
	close (buf->fchan);
	buf->fchan = NULL;
	}


match (s1, s2)				/* case-independent string equality */
	char *s1, *s2;
{
	while (TRUE) {
		if (toupper (*s1) != toupper (*s2)) return (FALSE);
		if (!*s1++ || !*s2++) return (TRUE);
		}
	}


Ask (q)					/* ask a yes/no question */
	char *q;
{
	puts (q);
	repeat {
		switch (toupper (TGetKb())) {
		
		case ' ':
		case 'Y':
			puts ("Yes\n");
			return (TRUE);
		case DEL:
		case BS:
		case BEL:
		case 'N':
			puts ("No\n");
			return (FALSE);
		default:
			puts ("\nAnswer 'Y' or 'N' --> ");
			break;
			}
		}
	}


dfltext (fn, ext)			/* default fn's extension to ext */
	char *fn, *ext;
{
	while (*fn) if (*fn++ == '.') return;
	*fn++ = '.';
	strcpy (fn, ext);
	}


/* "Terminal abstraction" */


TKbRdy()
{
	return (bios (2));
	}


TGetKb()
{
	return (bios (3));
	}



/* End of CRAYON.C  --  Scribble print driver top level */

