/* -*-c,save-*- */
/* 
 * PROGHEAD.C - Program head generator functions for Mince
 * Robert Heller Created: Tue Sep 24, 1985 19:50:37.36
 * Last Mod Tue Sep 24, 1985 22:02:06.94
 */

#include "mince.gbl"
#define PRFILE1 "PROFILE.HED"	/* file which contains profile info */
#define PRFILE2 "A:PROFILE.HED"	/* (secondary) file which contains profile
				   info */
/* Profile data structure */

static struct {
    char PerName[32];		/* personal name */
    char CopyRight[256];	/* copyright notice */
    } ProFile;
static int ProRead = FALSE;	/* init flag */
static char *linepfx = "";	/* line prefix text */

#define CMODESTR "/* -*-c,save-*- */"	/* text to put at top of C file */
#define SMODESTR "* -*-save-*-"		/* text to put at top of S file */
#ifdef LISPMODE
#define LMODESTR "; -*-lisp,save-*-"	/* text to put at top of LSP file */
#endif

/* MGenCPr - generate a C program head (like the one above) */

MGenCPr()
{
    static char descr[65];

    chkprofl();
    linepfx = " * ";
    BToStart();
    insstr(CMODESTR);
    BInsert(NL);
    insstr("/*");
    BInsert(NL);
    insstr(" * ");
    insstr(buffs[cbuff].fname);
    insstr(" - ");
    GetArg("Description: ",CR,descr,64);
    insstr(descr);
    BInsert(NL);
    insstr(" * ");
    insstr(ProFile.PerName);
    insstr(". Created: ");
#ifdef STRIDE
    MInsTime();
#else
    GetArg("Current Date: ",CR,descr,32);
    insstr(descr);
#endif
    BInsert(NL);
    insstr(" * Last Mod: ");
    BInsert(NL);
    insstr(" * ");
    BInsert(NL);	
    if (ProFile.CopyRight[0] != '\0' && Ask("Copyrighted? ")) {
	insstr(" * ");
	insstr(ProFile.CopyRight);
	BInsert(NL);
	insstr(" * ");
	BInsert(NL);
	}
    insstr(" */");
    BInsert(NL);
    arg = 0;
    }

/* MGenSPr - generate an S program head */

MGenSPr()
{
    static char descr[65];

    chkprofl();
    linepfx = "* ";
    BToStart();
    insstr(SMODESTR);
    BInsert(NL);
    insstr("*");
    BInsert(NL);
    insstr("* ");
    insstr(buffs[cbuff].fname);
    insstr(" - ");
    GetArg("Description: ",CR,descr,64);
    insstr(descr);
    BInsert(NL);
    insstr("* ");
    insstr(ProFile.PerName);
    insstr(". Created: ");
#ifdef STRIDE
    MInsTime();
#else
    GetArg("Current Date: ",CR,descr,32);
    insstr(descr);
#endif
    BInsert(NL);
    insstr("* Last Mod: ");
    BInsert(NL);
    insstr("* ");
    BInsert(NL);	
    if (ProFile.CopyRight[0] != '\0' && Ask("Copyrighted? ")) {
	insstr("* ");
	insstr(ProFile.CopyRight);
	BInsert(NL);
	insstr("* ");
	BInsert(NL);
	}
    insstr("*");
    BInsert(NL);
    arg = 0;
    }
#ifdef LISPMODE
/* MGenLPr - generate a LISP program head */

MGenLPr()
{
    static char descr[65];

    chkprofl();
    linepfx = ";;; ";
    BToStart();
    insstr(LMODESTR);
    BInsert(NL);
    insstr(";;;");
    BInsert(NL);
    insstr(";;; ");
    insstr(buffs[cbuff].fname);
    insstr(" - ");
    GetArg("Description: ",CR,descr,64);
    insstr(descr);
    BInsert(NL);
    insstr(";;; ");
    insstr(ProFile.PerName);
    insstr(". Created: ");
#ifdef STRIDE
    MInsTime();
#else
    GetArg("Current Date: ",CR,descr,32);
    insstr(descr);
#endif
    BInsert(NL);
    insstr(";;; Last Mod: ");
    BInsert(NL);
    insstr(";;; ");
    BInsert(NL);	
    if (ProFile.CopyRight[0] != '\0' && Ask("Copyrighted? ")) {
	insstr(";;; ");
	insstr(ProFile.CopyRight);
	BInsert(NL);
	insstr(";;; ");
	BInsert(NL);
	}
    insstr(";;;");
    BInsert(NL);
    arg = 0;
    }
#endif
/* MGenOPr - generate a program head (misc. catch all) */

MGenOPr()
{
    static char descr[65];

    chkprofl();
    linepfx = "";
    BToStart();
    insstr(buffs[cbuff].fname);
    insstr(" - ");
    GetArg("Description: ",CR,descr,64);
    insstr(descr);
    BInsert(NL);
    insstr(ProFile.PerName);
    insstr(". Created: ");
#ifdef STRIDE
    MInsTime();
#else
    GetArg("Current Date: ",CR,descr,32);
    insstr(descr);
#endif
    BInsert(NL);
    insstr("Last Mod: ");
    BInsert(NL);
    if(ProFile.CopyRight[0] != '\0' && Ask("Copyrighted? ")) {
	insstr(ProFile.CopyRight);
	BInsert(NL);
	BInsert(NL);
	}
    arg = 0;
    }
static insstr(str)
register char *str;
{
    while (*str != '\0') {
	BInsert(*str++);
	if (*(str-1) == NL) insstr(linepfx);
	}
    }
static chkprofl()
{
    FILE *pfile,*fopen();
    char *index();
    register int c,n;
    register char *p;

    if (ProRead) return;
    pfile = fopen(PRFILE1,"r");
    if (pfile == NULL) pfile = fopen(PRFILE2,"r");
    if (pfile == NULL) {
	strcpy(ProFile.PerName,"Anonymous");
	ProFile.CopyRight[0] = '\0';
	}
    else {
	fgets(ProFile.PerName,31,pfile);
	p = index(ProFile.PerName,'\n'); *p = '\0';
	n = 0; p =(&ProFile.CopyRight[0]);
	while ((c = fgetc(pfile)) != EOF && ++n < 256)
	    if (c == '\n') *p++ = NL;
	    else *p++ = c;
	fclose(pfile);
	}
    ProRead = TRUE;
    }
MGenPro()
{
    char modetyp[10];

    if (GetArg("Which mode :",CR,modetyp,9)) {
	LowCase(modetyp);
	if (strcmp(modetyp,"c") == 0) MGenCPr();
	else if (strcmp(modetyp,"s") == 0) MGenSPr();
#ifdef LISPMODE
	else if (strcmp(modetyp,"lisp") == 0) MGenLPr();
#endif
	else MGenOPr();
	}
    }

