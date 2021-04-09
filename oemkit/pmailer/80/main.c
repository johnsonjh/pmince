#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "subset.inc"
#include "term.inc"
#include "db.inc"


main(argc,argv)
 int argc;
 char **argv;
 {int (*ptrfn)();
		/* The terminal structure should be treated as external in 
		     UNIX implementations  (The pointer is used here to 
		     isolate the structure definition from modules that
		     do not need to know about it)  */
  struct terminal term;
  TERM = &term;
  initio();
  if (swapin("setup",OVERLAY) < 0) exit();
  ptrfn = OVERLAY;
  (*ptrfn)(argc,argv);     
  sysmenu();
			/* Sysmenu never returns.  The following functions
			    are included only to get them into the root    
			    segment */
  gettok();
  openform();
  getform();
  openss();
  getss();
  lnkdat();
  rdmbr0();
  }
   