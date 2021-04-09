#define MAXNTERMS 30		/* Maximum number of terminals */
#define MAXCTLCDSZ 5		/* Maximum size of control code sequence */
#define MAXLONGCCSZ 15		/* Maximum size of long control codes */ 
#define MAXTERMDSCSZ 20		/* Maximum size of terminal description */

#define TERMLSTFILE "termlist.sav"    /* File containing list of terminals */
#define TERMFILE "terminal.def"	      /* File containing terminal definition */

struct terminal	
 {char termdesc[MAXTERMDSCSZ];  /* Type of terminal */
  int screenht;			/* Number of lines on a screen */
  int screenwd;			/* Number of columns on a screen */  
  char clear[MAXCTLCDSZ];	/* Sequence to clear screen */
  int cleardelay;		/* Delay after screen clear */
  char underline[MAXCTLCDSZ];	/* Sequence to underline */
  char reverse[MAXCTLCDSZ];	/* Sequence to put into reverse video */ 
  char halfdsp[MAXCTLCDSZ];	/* Sequence to put into half intensity */
  char normaldsp[MAXLONGCCSZ];	/* Sequence to set normal display */ 
  char cursaddr[MAXLONGCCSZ];	/* Sequence to initiate direct cursor 
					addressing */
  int ioffset;			/* Offset to add to line number for direct
					cursor addressing */
  int joffset;			/* Offset to add to Column number for direct 
					cursor addressing */ 
  int cursdelay;		/* Delay after cursor address */
  char cursleft[4];		/* Sequence to move cursor left */
  };
 
#define XCOORDINATE 'x'+128    /* Flag indicating x coordiate in cursaddr
					control code */
#define YCOORDINATE 'y'+128	 /* Flag indicating y coordinate in cursaddr
					control code */

struct termlist
 {int nterms;
  struct terminal term[MAXNTERMS];
  };


