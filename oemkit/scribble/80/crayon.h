/* CRAYON.H		Global declarations for CRAYON.C and drivers

	Copyright (C) 1981 by Mark of the Unicorn, Inc.
	Created 81.3.12 Gyro

This file contains the global declarations for the Scribble printer
driver.  */



#define TRUE			(-1)
#define FALSE			0
#define NULL			0
#define ERROR			(-1)

#define NUL			0x0		/* ^@ */
#define ETX			0x3		/* ^C */
#define ACK			0x6		/* ^F */
#define BEL			0x7		/* ^G */
#define BS			0x8		/* ^H */
#define TAB			0x9		/* ^I */
#define LF			0xA		/* ^J */
#define FF			0xC		/* ^L */
#define CR			0xD		/* ^M */
#define SO			0xE		/* ^N */
#define SI			0xF		/* ^O */
#define DC1			0x11		/* ^Q */
#define DC3			0x13		/* ^S */
#define ESC			0x1B		/* ESC */
#define RS			0x1E		/* ^^ */
#define US			0x1F		/* ^_ */
#define DEL			0x7F		/* DEL */
#define EOF			0x1A		/* CP/M end of text marker */
#define ABORTFILE		0x7		/* ^G */
#define ABORTALL		0x1A		/* ^Z */
#define EXIT			0x3		/* ^C */

#define FLAG			char
#define MICA			unsigned
#define repeat			while (1)
#define ENDLIST		(-1)

#define INPUT			0
#define ABSOLUTE		0
#define DEFAULT		0


/* Special command characters */

#define SENTENCEBREAK	'\177'	/* ^? */
#define TOKENBREAK		'\240'	/* ~  */

#define BOLDON			'\302'	/* ~B */
#define BOLDOFF		'\342'	/* ~b */
#define ROMANON		'\322'	/* ~R */
#define ROMANOFF		'\362'	/* ~r */
#define UNNBON			'\325'	/* ~U underscore tokens */
#define UNNBOFF		'\365'	/* ~u */
#define UNALLON		'\301'	/* ~A underscore everything */
#define UNALLOFF		'\341'	/* ~a */
#define UNANON			'\316'	/* ~N underscore alphanumeric */
#define UNANOFF		'\356'	/* ~n */
#define ITALON			'\311'	/* ~I italics */
#define ITALOFF		'\351'	/* ~i */
#define BITALON		'\320'	/* ~P bold italics */
#define BITALOFF		'\360'	/* ~p */
#define SUPERON		'\323'	/* ~S superscripts */
#define SUPEROFF		'\363'	/* ~s */
#define SUBON			'\324'	/* ~T subscripts (why not?) */
#define SUBOFF			'\364'	/* ~t */
#define EXITON			'\330'	/* ~X eXit unusual attributes */
#define EXITOFF		'\370'	/* ~x */

#define JUSTIFY		'\212'	/* ~^J */
#define HORIZSPACE		'\210'	/* ~^H */
#define VERTSPACE		'\226'	/* ~^V */

#define MAXMICA	48000	/* largest possible mica measurement */


/* Printing options */
FLAG pausep;					/* pause for page changes? */
FLAG quietp;					/* turns off user interaction */
int startpage,					/* page to start printing on */
    curpage;					/* current page */


/* For communication with driver */
#define BOLD			'B'
#define UNDERSCORE		'U'
#define ITALIC			'I'
#define HORIZONTAL		'H'
#define VERTICAL		'V'
#define OVP			'O'
#define BIDIREC		'D'
#define CENTCHAR		'C'


/* Input line buffer */
#define ILINEMAX		256		/* chars in input line buffer */
#define ITOKENMAX		128		/* tokens in input line */
char iline[ILINEMAX];
struct token {
	int tokbegin;				/* index in line */
	int tokwidth;				/* width of token */
	int toklead;				/* leading whitespace */
	char tokscript;			/* normal, super, or sub */
	char tokuscore;			/* underscored? */
	FLAG tokbold;				/* boldface? */
	FLAG tokital;				/* italic? */
	char tokjust;				/* justify on left of this token? */
	} itokens[ITOKENMAX];
int inchars, intokens;
FLAG forcetok;					/* force new token on next char? */
FLAG superp, subp;				/* super- or sub-script? */

#define ATTRMAX		16		/* max depth of attribute stack */
struct token attrstack[ATTRMAX];
int attrsp;					/* stack pointer */

struct justdesc {
	int jlefttok; 				/* token on which justification begins */
	int jspace;				/* amount of whitespace to add */
	int jnbrk;				/* number of token breaks */
	} just;					/* this will have to be a list to get .. */
FLAG justp;					/* .. two columns right */


#define BUFBLKS		64		/* 4K input buffer */
struct fbuf {
	char fname[20];
	int fchan, fnleft;
	char *fnextp, fbuff[BUFBLKS][128];
	} inputbuf;


char *title, *portname;


/**************** Printer globals ****************/

int phpos, pvpos;				/* current position */
int pageh, pagev;				/* page size */
int charh, linev;				/* std character cell */
int spaceh;					/* width of ' ' */
FLAG hackdone;					/* paper hack done? */

#define FORWARD		1
#define BACKWARD		(-1)

FLAG warned;					/* you have been warned */

/* Underline map */
struct {
	int ulon, uloff;
	} ulmap[128];
int nuls;
FLAG ulonp;
FLAG boldp, italicp, ulitalp;

/* Output line buffer */
struct {
	char ochar;
	int ohpos;
	FLAG obold, oital;
	} outline[200];
int olen;


/**************** Device globals ****************/

int datfd;

/* CONFIG.DAT header and data records */

#define PORT		2
#define MICRO 		3
#define PRINT		4


struct header {
			/* Input/output port descriptions */
	int fport;			/* the first port record */
	int coninp;			/* console input port */
	int conoutp;			/* console output port */
	int prtinp;			/* Printer input(??) port */
	int prtoutp;			/* printer output port */
	int modinp;			/* modem input port */
	int modoutp;			/* modem output port */
			/* Terminal type descriptions */
	int fterm;			/* first terminal record */
	int ourterm;			/* the default terminal type */
	int simterm;			/* terminal type to simulate */
			/* Printer descriptions */
	int fprint;			/* first printer record */
	int ourprint;			/* the default printer type */
	int conprint;			/* the printer type of the console */
			/* Character width tables */
	int fmicro;			/* the first micro-spacing table record */
	int maxmicro;			/* the number of micro spacing records */
			/* Character translation tables */
	int ftran;			/* the first translation record */
			/* The size of the data file */
	int hmax;				/* number of records including header */

	int cpuspeed;			/* cpu speed in 100KHz */
	} hdr;


#define SSPACE	128-54
struct printdesc {		/* describes the output device */
	char dname[16];		/* name of the device */
	MICA dwidth;			/* width of the printing surface */
	MICA dheight;			/* height of the print surface */
	MICA micahoriz;		/* size of smallest horizontal increment */
	MICA micavert;			/* size of smallest vertical increment */
	FLAG isprop;			/* does it have a proportional width font? */
	char fontnum;			/* if isprop, which width table to use */
	MICA stdhoriz;			/* width of a canonical character */
	MICA stdvert;			/* height of a single spaced line */
	FLAG isfile;			/* do plain file output? */
	char prtcode;			/* printer type code for crayon */
	MICA paperoffset;		/* height paper is assumed to start at */
	FLAG backspace;		/* Does backspace work */
	FLAG barecr;			/* does bare CR work */
	FLAG formfeed;			/* does form feed work */
	char synctype;			/* Type of waiting to do: NONE, ETX/ACK, ^S^Q */
	FLAG needtran;			/* Do character translation on output? */
	char trannum;			/* Translation table number */
	struct str {			/* Various strings to send */
		char idx;					/* index into space */
		char len;					/* length */
		} initstr,			/* ... set things up */
			finistr,			/* ... clean up after */
			donewline,		/* ... do a newline */
			boldon,			/* ... turn on boldface */
			boldoff,			/* turn off boldface */
			italon,			/* turn on italics */
			italoff;			/* turn off italics */
	char strspc[SSPACE];	/* where strings are stored */
	} dev;


MICA widtab[128];			/* Character width table */

char trantbl[128];			/* Character translation table */

char tsect[128];

int dhpos, dvpos;
int dbold, dital;

#define NOCHAR		(-1)
int dbufchar;


/* Control code types */
#define VANILLA		1		/* no microspacing, standard controls */
#define DIABLO			2		/* 1620 & anything compatible */
#define EPSON			3		/* MX-80 (microvert) */
#define IDS460			4		/* IDS 460 proportional */
#define SPINWRITER		5		/* Spinwriter & friends */
#define CUSTOM			6		/* user custom subroutines */
#define QUME			7		/* Qume & friends */
#define CEN737			8		/* Centronics 737 */



/**************** Output globals ****************/

struct portdesc {
	char pname[16];		/* Zero terminated port name */
	FLAG dbiosp;			/* Use the bios call numbered by the data
                                   port if non-zero */
	char dataport;			/* Data port, or bios call number */
	char datamask;			/* Mask to AND data with */
	FLAG sbiosp;			/* Use the bios call numbered by the status
                                   port if non-zero */
	char statport;			/* Status port, or bios call number */
	char readymask;		/* Mask to AND status with */
	char polarity;			/* (Status AND mask == <this>) if ready */
	} iport, oport;


/* Output queue */
#define QUEUE_SIZE		192		/* Size of output queue */
int oqueue [QUEUE_SIZE + 4];

/* Synchronization control */
#define NOSYNC		1
#define ETX_ACK	2
#define XON_XOFF	3
#define SYNCBUFSIZE	60
#define SEQSTART	0x100
#define SEQEND		(SEQSTART + 0x20)

FLAG syncok;					/* for XON/XOFF: OK to send character? */
int synccnt;					/* for ETX/ACK: no of chars sent so far */
int syncnbufs;					/* for ETX/ACK: no of buffers sent */


/**************** Globals for printer drivers ****************/

#define UNINITIALIZED	0x8000

int drvhpos, drvvpos;			/* current horizontal, vertical position */

int drvhpitch, drvvpitch;		/* current pitch of printer */

int drvdirec;					/* current print direction */

FLAG drvshiftp;				/* is printer shifted? */


/* End of CRAYON.H  --  globals for Scribble print program */

