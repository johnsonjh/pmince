



/*
		-------------------------- 
		| List Format Structures |
		--------------------------  
*/

#define MAXNHEADFIELD 20	/* Maximum number of fields in list heading */
#define MAXNMEMFIELD 40		/* Maximum number of fields in member format */
#define MINPAGESZ 1 		/* Minimun height of a page */
#define MAXPAGESZ 300		/* Maximum height of a page */  
#define MAXNSUBHEADFLDS 15	/* Maximum number of fields in subheading */
#define MAXNKEYS 10		/* Maximum number of sort keys */
#define LFRMDSCBUFSZ 400	/* Size of buffer holding list format 
				     descriptions */
#define STRBUFSZ 500		/* Size of the field buffer */




/*
		-------------------------------
		| List Format Field Structure |
		-------------------------------
*/
struct lffield
  {char type;	     /* STRING, LOGFIELD, TEMPLATE, SSNAME, PAGENUM or DATE */ 
   char dispmode;	/* Display mode (Underlining and Field Length) */
   int fldlen;    	/* Length of field */
   int unused;    	/* Spare */
   char posmode;	/* Position mode--CENTER, LEFTJUST,
				RIGHTJUST,or NOPOSIT */	
   int fldid;		/* Offset in field buffer if type == STRING;
			   Field number if type == LOGFIELD or TEMPLATE */
   };
 
/*
		-----------------------
		| List Heading Format |
		-----------------------
*/
struct headformat
  {int nheadfields;			/* Number of list format fields */
   struct lffield headfield[MAXNHEADFIELD];	/* Heading list format block */
   };


/*
		---------------
		| Page Format |
		---------------
*/
struct pageformat
 {struct headformat pagehead;   /* Page heading list format block */
  int formht;			/* Height of the form being used */
  int bottommarg;		/* Bottom margin */
  };
  



/*
		----------------------
		| List Member Format |
		----------------------
*/
struct memformat
  {int nmemfields;			   /* Number of member format fields */
   struct lffield memfield[MAXNMEMFIELD];   /* Output fields */
   }; 

/*
		------------
		| Sort Key |
		------------
*/
struct key0
  {char type;		/* LOGFIELD or TEMPLATE */
   char direction;	/* FORWARD or REVERSE */
   int fldno;		/* Field to sort by */
   int nsubhdflds;	/* Number of output fields in the sub-heading */
   struct lffield subhead[MAXNSUBHEADFLDS];  /* Subheading list format block */
   };

struct sortkeys
  {int nkeys;			/* Number of sort keys */
   struct key0 key[MAXNKEYS];	/* Sort keys */
   };



/*
		---------------
		| LIST FORMAT |
		---------------
*/
struct listformat
   {struct headformat listhead;	/* Format for list heading */
    struct pageformat page;     /* Page Format */ 
    struct memformat memform;   /* Format for individual members */
    struct sortkeys keys;       /* Sort keys (with subheadings) */
    char strbuf[STRBUFSZ];	/* Buffer for format strings */
    };




/*
		----------------------------------------
		| Output and Sort Key Field Type Codes |
		----------------------------------------
*/
#define STRING 0		/* Field is string from input by user */
#define LOGFIELD 1		/* Logical Field */
#define TEMPLATE 2 		/* Template */
#define TODAYDATE 3		/* Date of Listing */
#define SSNAME 4		/* Field is name of subset being listed */
#define PAGENUM 5		/* Field is the page number */


				/* Field Types */
#define FIELDTYPES "String,Logical Field,Template,Date of Report,Subset Name,Page Number"

				/* Sort Key Types */
#define KEYTYPES "Logical Field,Template"	

/*
		-----------------------
		| Sort Key Directions |
		-----------------------
*/
#define FORWARD 0		/* Sort in forward order */
#define REVERSE 1		/* Sort in reverse order */

#define SORTDIRS "Ascending,Descending"




/*
		-------------------------------
		| Output Field Position Flags |
		-------------------------------
*/
#define CENTER 0	/* Center field */
#define LEFTJUST 1	/* Left justify field */
#define RIGHTJUST 2	/* Right justify field */
#define NOPOSIT 3   	/* No Positioning  (Field length not specified) */

#define POSMODES "Centered,Left Justified,Right Justified"




/*
		+--------------------+
		| Display Mode Masks |
		+--------------------+
*/
#define UNDERMASK 1		/* Underline mode mask */
#define LENGTHMASK 2		/* Field length set mask (used only 
				     for toggle menu) */


#define LFDSPMARG 3		/* Margin used in List Format Display */








