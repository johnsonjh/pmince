#include "constant.inc"
#include "sys.inc"
#include "db.inc"
#include "tmplt.inc"
#include "term.inc"


#define HELPFILE "gendb.hlp"




char TAG[MAXNLFLDS][MAXTAGSZ];  /* Buffer for field tags */



/*
			-----------------
			| Template List |
			-----------------
*/

struct tmpltlist TMPLTLST;		 




/*
			-----------------------------
			| Default System Parameters |
			-----------------------------
*/

#define PROGDIR0 "a"
#define SORTDIR0 "b"
#define HELPDIR0 "a"
#define DBSIZE0 500
#define DIRBAKUP0 5




#define FLDTYPES "Alphabetic Field,Alphanumeric Field,Numeric Field,Status Field"


#define MAXINT 32000				/* Maximum Integer */
#define MAXDBSZ 32000 				/* Maximum DB Size */	

