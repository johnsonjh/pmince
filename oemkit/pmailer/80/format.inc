/*
			------------------------------
			| Format Directory Structure |
			------------------------------
*/

struct formatdir
 {int formfd;		/* The file descriptor of the format file */
  char *desclist;	/* Pointer to the list of descriptions */
			/* Name of the file containing format descriptions */
  char descfile[MAXFNSZ];
  int formsz;		/* Number of bytes one format occupies */
  };
