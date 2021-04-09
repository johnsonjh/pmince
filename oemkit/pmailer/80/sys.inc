
#define DEFFILE "db.def"	/* Data base definition file  (generated
				     by gendb) */
#define TXTFILE "db.txt"	/* File containing textual description 
				     of data base */
#define TERMFILE "terminal.def" /* File containing terminal definition */
#define TAGBUFSZ 1024		/* Size of the buffer that holds the tags */
#define MEMBUFSZ 1024		/* Size of the buffer that will hold member */
#define MAXNLFLDS 70		/* Maximum number of logical fields */
#define MEMIDSZ 4 		/* The maximum size of the member ID */




/*
			--------------------------------------------
			| Member ID Character Definition Structure |
			--------------------------------------------
*/

struct memidchar
  {int idfld; 		/* The logical field containing the member ID char */
   int idpos;   	/* The position of the character in the field */
   };




		/* System variables (passed at the beginning 
		     of file DEFFILE) */
struct sysvars	
 {int nlflds;		   /* The number of logical fields */
  int dirbakno;		   /* The Directory backup interval */
  int nbucket0;		   /* The number of buckets */
			   /* The fields and positions of the member id */
  struct memidchar memberid[MEMIDSZ];
			   /* The message telling the user what the 
				 member ID is */
  char memidmsg[MAXDESCSZ];
  char progdir[MAXFNSZ];   /* The directory in which the programs are found */
  char sortdir[MAXFNSZ];   /* The directory in which files for external 
			        sorts will be written */
  char helpdir[MAXFNSZ];   /* The directory in which helpfiles are found */
  char version;            /* CP/M version number */
  char spare;              /* Spare */
  char dbname[MAXDESCSZ];  /* Data base description */
  };



/*
		--------------------
		| Screen Map Codes |
		--------------------
*/
#define ALPHA 1			/* Pure alphabetic */
#define ALPHANUM 2		/* Alphanumeric */
#define NUMERIC 3		/* Pure numeric */
#define STATUS 4		/* Status field */
#define COMPOSITE 5		/* Composite field */
/*
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
| Character codes  from ' ' to 'z'  |
| appear on forms entry screen at   |
| half intensity protected.	    |
'''''''''''''''''''''''''''''''''''''
*/

#define NOTEMPLATE 255	/* Flag indicating field is not part of a template */

/*		----------------------------
		|  Logical Field Structure |
		----------------------------
*/

struct logfield
  {char type;        /* Logical field type */
   char tmpltno;     /* If this field is part of a template-->number of 
			    that template;
			else  NOTEMPLATE  */
   char tmpltpos;    /* Position of this logical field within template */
   int length;       /* Length of data field on screen (and, hence, maximum
			   length of data item in logical field) */
   int maxval;	     /* Maximum value (if type == NUMERIC) */
   int minval;	     /* Minimum value (if type == NUMERIC) */
   char ipos;	     /* Line number of first field position */
   char jpos;	     /* Column number of first field position */
   char *tag;	     /* Pointer to tag of this field */
   };









