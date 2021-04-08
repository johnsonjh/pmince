/* -*-c,save-*- */
/*
 * ELECC.C - Electric C mode code
 * Robert Heller. Created: Tue Sep 24, 1985 21:45:45.71
 * Last Mod: 
 */
/*
 * globals (ala declare-global)
 */
#include "mince.gbl"

static int ec_spldc = 0,	/* &elec-c-split-declaractions */
/*	   ec_indwd = 4,	/* &elec-c-indent-width */
/*	   ec_righm = 75,	/* &elec-c-right-margin */
	   ec_bordc = '-',	/* &elec-c-border-char */
	   ec_bords = 75;	/* &elec-c-border-size */

#define ec_indwd indentcol	/* use Mince's global */
#define ec_righm fillwidth	/* dito */

static char ec_compfx[32] = "  "; /* &elec-c-comment-prefix */

static int ec_comm = 0,		/* &elec-c-comment */
	   ec_boxc = 0,		/* &elec-c-box-comment */
	   insdqut = 0,		/* inside-double-quote */
	   incsqut = 0,		/* inside-single-quote */
	   main_p  = 0;		/* flag tells us whether argc is special or not */

CAbrvInit()
{
    int ec_main(),ec_argc(),ec_if(),ec_else(),ec_for(),ec_while(),ec_do(),
    	ec_switch(),ec_case(),ec_default();

    CAbrv[0].Abrev = "main";
    CAbrv[0].Expand = "main";
    CAbrv[0].Abrevfun = ec_main;

    CAbrv[1].Abrev = "argc";
    CAbrv[1].Expand = "argc";
    CAbrv[1].Abrevfun = ec_argc;

    CAbrv[2].Abrev = "if";
    CAbrv[2].Expand = "if";
    CAbrv[2].Abrevfun = ec_if;

    CAbrv[3].Abrev = "else";
    CAbrv[3].Expand = "else";
    CAbrv[3].Abrevfun = ec_else;

    CAbrv[4].Abrev = "while";
    CAbrv[4].Expand = "while";
    CAbrv[4].Abrevfun = ec_while;

    CAbrv[5].Abrev = "do";
    CAbrv[5].Expand = "do";
    CAbrv[5].Abrevfun = ec_do;

    CAbrv[6].Abrev = "switch";
    CAbrv[6].Expand = "switch";
    CAbrv[6].Abrevfun = ec_switch;

    CAbrv[7].Abrev = "case";
    CAbrv[7].Expand = "case";
    CAbrv[7].Abrevfun = ec_case;

    CAbrv[8].Abrev = "default";
    CAbrv[8].Expand = "default";
    CAbrv[8].Abrevfun = ec_default;

    CAbrv[9].Abrev = "for";
    CAbrv[9].Expand = "for";
    CAbrv[9].Abrevfun = ec_for;

    CAbrv[10].Abrev = "#i";
    CAbrv[10].Expand = "#include";
    CAbrv[10].Abrevfun = 0L;

    CAbrv[11].Abrev = "#d";
    CAbrv[11].Expand = "#define";
    CAbrv[11].Abrevfun = 0L;

    CAbrv[12].Abrev = 0L;
    CAbrv[12].Expand = 0L;
    CAbrv[12].Abrevfun = 0L;


