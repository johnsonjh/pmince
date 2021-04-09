/*
			---------------------------
			| Help Screen Table Entry |
			---------------------------
*/

struct helpscr
 {int hsector;		/* The sector at which the help screen begins */
  int offset;		/* The offset into sector at which the screen begins */
  int nchars;		/* The number of characters in the help screen */
  };


#define HELPTABSZ 42	/* The number of entries in a help table--
			     i.e., the maximum number of help screens */
