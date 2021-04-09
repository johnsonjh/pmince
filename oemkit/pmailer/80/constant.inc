/*
			------------------------------
			| System Dependent Constants |
			------------------------------
*/
#define OVERLAY 0x3400		/* Start of overlay segment */
#define SECTORSZ 128		/* Size of a disk sector */
#define EXTENTSZ 16384		/* Size of one disk extent */
#define MAXDIRSZ 2		/* Maximum size of directory [here only
				     a disk drive designation] */
#define MAXNDIG 5		/* The maximum number of digits in an
				     integer */




/*
			-----------------------
			| Terminal Dimensions |
			-----------------------
*/
#define SCREENHT 24		/* Height of terminal screen */
#define SCREENWD 80		/* Width of terminal screen */


/*
			----------------------------
			| Storage Allocation Sizes |
			----------------------------
*/

#define MAXTOKSZ 15		/* Maximum size of a token */
#define MAXTAGSZ 25		/* Maximum size of a field tag */
#define MAXFNSZ 17 		/* Maximum file name size */
#define MAXDESCSZ 58		/* Maximum size of a description 
				    (that will go in a menu option list) */
#define LINBUFSZ 133		/* Size of a buffer that will hold at most
				      one line of text */
#define SYSBUFSZ 512		/* Size of a general purpose system buffer */



/*
			----------------
			| System Flags |
			----------------
*/

#define DISCARD 0		/* Return flag for discard input */
#define SAVE 1			/* Return flag for save input */

#define EOB 255			/* Flag for End of Buffer */


#define EQUALS 0		/* Return value for equals */
#define LESSTHAN 1 		/* Return value for less than */
#define GREATERTHAN 2		/* Return value for greater than */
 

#define NEWENTRY -1		/* Flag indicating member is a new entry */
#define DELETED 255		/* Flag indicating member has been deleted */

#define LOGFIELD 0		/* Flag indicating entity is a logical field */
#define TEMPLATE 1		/* Flag indicating entity is a template */
#define TODAYDATE 2		/* Flag indicating entity is today's date */


/*
			-------------------
			| Other Constants |
			------------------- 
*/
#define STRMAX  "zzz" 		/* Alphabetically large string */
/*
			-------------------------
			| Setmode Display Codes |
			-------------------------
*/
#define CLEAR 0  		/* Clear screen */
#define NORMALDSP 1  		/* Normal Display  */
#define HALFDSP 2  		/* Display half intensity */ 
#define UNDERDSP 3   		/* Underline Display */
#define BOLDDSP 4		/* Boldface Display (Reverse Video on tube) */
#define CRSLEFT 5  		/* Cursor left */

/*
			---------------------------
			| Setmode Targeting Codes |
			---------------------------
*/
#define CRTON 9			/* CRT on */
#define CRTOFF 10		/* CRT off */
#define HDCPYON 11		/* Hardcopy on */
#define HDCPYOFF 12		/* Hardcopy off */
#define FILEON 13		/* File on */
#define FILEOFF 14		/* File off */








/*			----------------------------- 
			| Ascii Character Constants | 
			-----------------------------  
*/
#define BELL 7			/* Ascii Bell character [Control G] */
#define BACK 8			/* Back Arrow [Control H] */
#define TAB 9			/* Tab [Control I] */
#define LF 10			/* Line Feed [Control J] */
#define UP 11			/* Up arrow [Control K] */
#define FORWARD 12		/* Forward arrow [Control L] */
#define CR 13			/* Carriage return [Control M] */
#define EOF 26			/* End of file mark [Control Z] */
#define ESC 27  		/* Ascii code for escape */       
#define DELETE 127		/* Delete character */
#define CTLQ 17			/* control q */
#define CTLX 24			/* control x */






/*
		------------------
		| Template Types |
		------------------

*/
#define DATE 0				/* Date template */
#define TIME 1				/* Time template */
#define ADDRESS 2			/* Address template */
#define PHONE 3				/* Phone template */
#define NAME 4				/* Name template */
#define SOCIALSECURITY 5		/* Social Security template */
#define MONEY 6  			/* Cost template */


