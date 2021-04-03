/* PENCIL.C		Generalized random-file printer

	Copyright (C) 1981 by Mark of the Unicorn Inc.
	Created 81.7.31 Gyro

This program prints an ASCII file using the definitions in CONFIG.DAT. */


#include "crayon.h"



#define MAXTABS	32		/* max # of tabs */
struct {
	MICA tabcolumns[MAXTABS];/* where tabs are */
	MICA headerspace;		/* vertical space to leave after header */
	MICA footerspace;		/* and before footer */
	char prefjust;			/* preferred justification style
							'b' both  or  'l' left */
	char footstyle;		/* where footnotes go:  'i' inline,
							'b' bottom, 'e' end */
	MICA indent;			/* amount to indent paragraphs */
	MICA spread;			/* number of lines between paragraphs */
	MICA linespacing;
	FLAG footpush;			/* footnote references */
	FLAG scriptpush;		/* do supper/sub scripts get more space */
	FLAG ischapter;		/* does @chapter command exist */
	MICA stop;			/* margins */
	MICA sleft;
	MICA sright;
	MICA sbottom;
	MICA above;			/* space to leave before an environment */
	MICA below;			/* space to leave after an environment */
	FLAG levindent;		/* do nested Level environments indent? */
	FLAG levhang;			/* are Level paragraphs outdented? */
	int endsize;			/* space to reserve for enddata */
	} scribrec;


char *devname;				/* device to print on */
int headingp;				/* print heading on each page? */
int leftmar, rightmar;		/* text margins */
int cleftmar, crightmar;		/* margins as entered from command line */
int perfskip;				/* amount to skip around page perfs */



main (argc, argv)
	int argc;
	char **argv;
{
	int ntimes, itab, iarg, tabwidth;
	char *optname;
	struct {
		char *fname;
		int fntimes;
		} files[20];
	int nfiles;
	
	puts ("Pencil Version 1.3, Copyright (C) 1981 by Mark of the Unicorn Inc.\n");
	ntimes = 1;
	nfiles = 0;
	pausep = FALSE;
	title = "";
	portname = DEFAULT;
	devname = DEFAULT;
	quietp = FALSE;
	startpage = 1;
	headingp = TRUE;
	cleftmar = crightmar = -1;
	perfskip = 3;
	tabwidth = DEFAULT;
	for (iarg = 1; iarg < argc; ++iarg) {
		if (*argv[iarg] == '-') {
			optname = argv[iarg] + 1;
			if (isdigit (*optname)) ntimes = atoi (optname);
			else if (match (optname, "Dev")) {
				devname = argv[++iarg];
				if (!quietp) printf ("Using the '%s' device.\n", devname);
				}
			else if (match (optname, "L")
				    ||  match (optname, "Left")) {
				cleftmar = atoi (argv[++iarg]);
				if (!quietp) printf("Left margin set to %d characters.\n",
								cleftmar);
				}
			else if (match (optname, "NH")
				    ||  match (optname, "NoHeading")) {
				if (!quietp) puts ("Not printing page headings.\n");
				headingp = FALSE;
				}
			else if (match (optname, "NS")
				    ||  match (optname, "NoSkip")) {
				if (!quietp) puts ("Not skipping page perforations.\n");
				perfskip = 0;
				}
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
			else if (match (optname, "R")
				    ||  match (optname, "Right")) {
				crightmar = atoi (argv[++iarg]);
				if (!quietp) printf ("Right margin set to %d characters.\n", crightmar);
				}
			else if (match (optname, "Tab")
				    ||  match (optname, "Tabs")) {
				tabwidth = atoi (argv[++iarg]);
				if (!quietp) printf ("Tab stops set every %d characters.\n", tabwidth);
				}
			else if (match (optname, "T")
				    ||  match (optname, "Title")) {
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
	if (((datfd = open ("config.dat", INPUT)) < 0
	    && (datfd = open ("a:config.dat", INPUT)) < 0))
		Fatal ("Can't open the system database file (CONFIG.DAT)\n");
	seek (datfd, 2, ABSOLUTE);
	read (datfd, tsect, 1);
	movmem (tsect, scribrec, sizeof (scribrec));
	close (datfd);
	if (tabwidth != DEFAULT)
		for (itab = 0; itab < MAXTABS; ++itab)
			scribrec.tabcolumns[itab] = itab * tabwidth;
	for (iarg = 0; iarg < nfiles; ++iarg)
		if (PrintFile (files[iarg].fname, files[iarg].fntimes) == ABORTALL)
			break;
	}


PrintFile (fname, ntimes)
	char *fname;
	int ntimes;
{
	char c;
	FLAG done;
	
	if (IOpen (fname, INPUT) < 0) {
		printf ("Can't open input file '%s'.\n", fname);
		return;
		}
	if (!quietp) {
		if (ntimes == 1)
			printf ("Printing '%s' -- OK? ", fname);
		else printf ("Printing %d copies of '%s' -- OK? ", ntimes, fname);
		if (!Ask ("")) {
			puts (" -- skipping file.\n\n");
			return (ABORTFILE);
			}
		}
	PInit (devname, portname, !pausep);
	if (cleftmar == -1) leftmar = PUnMica (scribrec.sleft, HORIZONTAL);
	else leftmar = cleftmar * PCharH();
	if (crightmar == -1) rightmar = PUnMica (scribrec.sright, HORIZONTAL);
	else rightmar = crightmar * PCharH();
	if (PVPos() != 0) {
		FileHeader();
		PFF();
		}
	done = FALSE;
	while (ntimes-- > 0  &&  !done) {
		curpage = 0;
		NextPage();
		DoChar (CR);
		while (!done  &&  IChar() != EOF) {
			DoChar (IChar());
			if (TKbRdy()) switch (c = TGetKb()) {
			
			case ABORTFILE:
				puts ("\nAborting this file.\n");
				done = c;
				break;
			case ABORTALL:
				puts ("\nAborting all files.\n");
				done = c;
				break;
			case EXIT:
				exit (1);
			default:
				break;
				}
			INext();
			}
		IRew();
		PNewPage();
		}
	PFini();
	IClose();
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
	PPrtStr ("Pencil version 1.2");	/* vsn of this prog. */
	PCR();
	}


NextPage()				/* process page boundary */
{
	int prevh;
	
	++curpage;
	if (!quietp) printf (" %d", curpage);
	prevh = PHPos();
	PCR();
	PNewPage();
	if (pausep) PagePause();
	if (headingp  &&  perfskip > 0) {
		if (perfskip >= 3) PLF();
		PageHeader();
		}
	PMoveV (perfskip * PLineV() - PVPos());
	PMoveH (prevh);
	}


DoChar (ochar)				/* process a char */
	char ochar;
{
	int itab;
	
	switch (ochar) {
	
	case BS:
		PMoveH (-PCharH());
		break;
	case TAB:
		for (itab = 0; itab < MAXTABS; ++itab)
			if (scribrec.tabcolumns[itab] * PCharH() + leftmar > PHPos()) {
				PMoveH (scribrec.tabcolumns[itab] * PCharH()
					   + leftmar - PHPos());
				break;
				}
		if (itab >= MAXTABS) PMoveH (PCharH());
		break;
	case FF:
		PFF();
		NextPage();
		break;
	case CR:
		PCR();
		PMoveH (leftmar);
		break;
	case LF:
		PLF();
		if (PVPos() >= PPageV() - perfskip * PLineV()) NextPage();
		break;
	default:
		if (PHPos() + PWidChar (ochar) > PPageH() - rightmar) {
			DoChar (CR);
			DoChar (LF);
			}
		PPrtChar (ochar);
		break;
		}
	}


PageHeader()				/* print the page header */
{
	char str[80], *c;
	int len;
	
	PMoveH (leftmar);
	PSetAttr (UNDERSCORE, UNNBON);
	sprintf (str, "File: %s", IName());
	PPrtStr (str);
	PMoveH ((PPageH() - PWidStr (title) - rightmar + leftmar) / 2 - PHPos());
	PPrtStr (title);
	sprintf (str, "Page: %d", curpage);
	PMoveH (PPageH() - PWidStr (str) - rightmar - PHPos());
	PPrtStr (str);
	PSetAttr (UNDERSCORE, FALSE);
	PCR();
	}


/* End of PENCIL.C  --  print a standard ASCII file */

