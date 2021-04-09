#define HELPFILE "list.hlp"

#define MAXLISTSZ  512		/* Maximum number of list elements [in core] */
#define KEYBUFSZ 4700		/* Size of the buffer that holds sort keys */

#define EOK 1			/* Flag indicates end of a single sort key */


#define INCORE 0    		/* Sort keys are in core */
#define INFILE 1		/* Sort keys are in file */

#define SECTDISPCT 12		/* The number of sectors which must be read
				   between displays of "Reading Sector ..." */



/*
		+-------------------+
		| List Target Masks |
		+-------------------+
*/
#define CRTMASK 1		/* Mask to target CRT for list output */
#define HDCPYMASK 2		/* Mask to target Printer for list output */
#define FILEMASK 4		/* Mask to target Disk File for list output */
#define IMAGEMASK 8		/* Mask to produce Sort Image File */


/*
			---------------------------- 
			| Sort Parameter Structure |
			----------------------------
*/
struct listparams
 {int iformat;			/* List format number */
  int iss;			/* Subset number */
  char ssdesc[MAXDESCSZ];	/* Subset description */
  char target;			/* List targets */
  char listfile[MAXFNSZ];	/* Out list file */
  char imagefile[MAXFNSZ];	/* Output sort image file */
  int nfiles;			/* Number of files to be merged */
  };



#define PARAMFILE "sort.par"	/* File in which sort parameters are passed
				     to external sort routine */

#define DEFIMAGEFILE "sort.img"  /* The default sort image file */

