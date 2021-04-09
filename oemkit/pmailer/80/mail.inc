/*
		-------------------------
		| Mail System Constants |
		-------------------------
*/
#define PARINDENT 5	   /* Number of spaces to indent paragraphs */
#define MAXNACROSS 7 		/* Maximum number of labels across */
#define MINFORMHT 6		/* Minimum height of form */
#define MAXFORMHT 300		/* Maximum height of form */
#define MAXLMARG 200		/* Maximum left margin */
#define MINRMARG 20 		/* Minimum right margin */
#define MAXRMARG 200		/* Maximum right margin */



/*			-------------------------
			| Mail Run Type Options |  
			-------------------------
*/
#define LETTERS 0		/* Letter type */
#define ENVELOPES 1		/* Envelope Type */
#define LABELS 2		/* Label Type */
#define MAILTYPES "Letters,Envelopes,Labels"



/*
			---------------------
			| Extraction Option |
			---------------------
*/
#define INDIVIDUAL 0		/* Extract by individual Member ID */
#define SUBSET 1 		/* Extract by subset */
#define SORTIMAGE 2		/* Recipients from sort image */
#define PREVIOUSRUN 3		/* Same recipients as previous run */
#define EXTROPTS "Individually by Member ID,By Subset,From Sort Image"



/*
			----------------------------
			| Continuouts Forms Option |
			----------------------------
*/
#define CONTINUOUS 0
#define SEPARATE 1
#define CONTOPTS "Continuous Forms,Separate Sheets"


/*
			---------------------
			| List Target Masks |
			---------------------
*/
#define CRTMASK 1			/* Mask for CRT output */
#define HDCPYMASK 2			/* Mask for Hard Copy output */
#define FILEMASK 4			/* Mask for file output */




/*
			-----------------------
			| Formatting Commands |
			-----------------------
*/
/*
 *	LOGFIELD, TEMPLATE and TODAYDATE defined elsewhere 
 */
#define KEYWORD 3		/* Keyword command */
#define SALUTATION 4		/* Salutation command */


/*
			-----------------
			| Keyword Flags |
			-----------------
*/
#define FORMAT 1		/* Begin Formatting */
#define UNFORMAT 2		/* Stop Formatting */
#define PARAGRAPH 3		/* Start a new paragraph */
#define NEWLINE 4		/* Start a new line */
#define UNDER 5			/* Begin underlining */
#define NOUNDER 6		/* End underlining */


#define TITLES "Ms.,Mr.,Mrs.,Miss,Dr.,Judge,Professor,Reverend,Mayor,Senator,Governor"
