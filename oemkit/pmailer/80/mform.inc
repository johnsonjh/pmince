/*
		-------------------------
		| Mail System Constants |
		-------------------------
*/
#define MFRMDSCBUFSZ 200	/* Size of buffer to hold list of mail format
				     descriptions */

#define LETBUFSZ 3600	   /* Size of buffer that holds letters to be mailed */
#define HITBUFSZ 64 	   /* Size of buffer holding previous recipients--
				Must be a multiple of SECTORSZ/2 */
#define MAXLABHT 9  		/* Maximum height of label */
#define MINLABWD 10		/* Minimum label width */
#define MAXLABWD 130 		/* Maximum Label width */

/*
			----------------------   
			| Mail Run Structure |
			----------------------   
*/



struct mailformat
 {char unused;			/* Spare */
  char contopt;			/* CONTINUOUS or INDIVIDUALUAL sheets */
  int height;			/* Height of form (for CONTINUOUS) */
  int leftmargin;		/* Left margin (for LETTERS) */
  int rightmargin;		/* Right margin (for LETTERS) */
  char defsalut[MAXDESCSZ];	/* Default salutation */
  int labwidth;			/* Width of the label (for LABELS) */
  int nacross;			/* Number of labels across (for LABLES) */
  };


/*
			-------------------------
			| N-Across Label Buffer |
			-------------------------
*/
#define LABBUFHT MAXLABHT		/* The height of the label buffer */
#define LABBUFWD 132			/* The width of the label buffer */
char LABBUF[LABBUFHT][LABBUFWD];  	/* The label buffer */
int NACROSSCT;				/* Counter to keep track of position 
						in buffer */
int ILABPOS;				/* Current label buffer row */
int JLABPOS;				/* Current label buffer column */
int FORMATFLG;				/* Flag indicating whether formatting
					    is in effect */




/*
			--------------------------
			| Pre-processor Keywords |
			--------------------------
*/
#define NKEYWORDS 6		/* Number of keywords */
  
struct
  {char keytag[MAXTOKSZ];	/* The keyword */
   char keychar;		/* The character flag inserted in the text */
   } KEYWORDS[NKEYWORDS];






