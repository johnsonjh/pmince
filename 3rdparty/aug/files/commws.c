/*	CommX.C - Additional Commands for Mince.

	Dave W. Smith, 450 N. Mathilda, #O-103, Sunnyvale, CA 94086

	Caveat:	Adding these to Mince requires that you recompile the
	 sources  with -e8100, and link with LMince, LUtil, etc. (See
         the section 'Compiling and Linking Mince' (p. 1-16 of the
         Program Logic Manual) for more information).

	The changes to Bindings.C are left as an exercise to the user.  */

#include "mince.gbl"		/* pick up global definitions */

MToglC()	/* (C-^)  Toggle the case of a  character */
{
  tmp = Buff();
  if (tmp >= 'a' && tmp <= 'z') tmp = tmp - 040;
  else if ( tmp >= 'A' && tmp <= 'Z') tmp = tmp + 040;
  if ( tmp != Buff()){
    BDelete(1);
    BInsert(tmp);
  } else BMove(1);
}

/*  Routine to search for a matching delimiter, skipping over pairs of 
    delimiters.  (Find me my open paren, boss.) Generalized somewhat
    to work with a vareity of delimiters.                               */

MDMatch()	/* M-(, M-), M-{, M-}, etc. */
{
  int Dir, Delim, MDelim, Count;
  tmark = BCreMrk();
  Delim = cmnd & 0177;
  if      ( Delim == '(' ) {MDelim = ')' ; Dir = BACKWARD; }  
  else if ( Delim == ')' ) {MDelim = '(' ; Dir = FORWARD;  }  
  else if ( Delim == '{' ) {MDelim = '}' ; Dir = BACKWARD; }  
  else if ( Delim == '}' ) {MDelim = '{' ; Dir = FORWARD;  }  
  else if ( Delim == '[' ) {MDelim = ']' ; Dir = BACKWARD; }  
  else if ( Delim == ']' ) {MDelim = '[' ; Dir = FORWARD;  }  
  Count = (Buff() == MDelim ) ? 1 : -1;
  while ( 1 ) {
    TKbChk();
    BMove( Dir ? 1 : -1);
    if (Dir && BIsEnd()) break;
    if ( Buff() == MDelim ) Count = (Count < 0) ? 2 : Count + 1;
    else if ( Buff () == Delim ) Count = (Count < 0) ? 0 : Count - 1;
    if ( !Dir && BIsStart()) break;
    if ( !Count ) break;
  }
  if ( Count ) {
    Echo( "No Match" );
    BPntToMrk( tmark );
    arg = 0; 
    }
  BKillMrk( tmark );
}

MFini()		/* (C-X C-F) Write File and Exit */
{
	MFileSave();
	MExit();
}

/* End of CommX.C  - Additional Commands for Mince.   */
