#include "constant.inc"
#include "sys.inc"
#include "tmplt.inc"
#include "iovars.inc"

#define HELPFILE "gentmplt.hlp"

struct tmpltlist TMPLTLST;		/* The template list */

char BUFFER[2000]; 			/* Utility buffer */



#define FLDTYPES "Pure Alphabetic,Alphanumeric,Pure Numeric,Status,Composite"


#define MAXINT 32000			/* The maximum integer */
