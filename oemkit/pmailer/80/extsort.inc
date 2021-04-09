/*
			------------------------------
			| Heap Sort Buffer Structure |
			------------------------------
*/

#define MERGEORDER 4	   /* The "merge order" of the heap sort [ie- the
			       number of files that can be merged at once] */
#define MAXKEYSZ 150	   /* The maximum size of a sort key */ 
#define FILEBUFSZ 950  	   /* The number of bytes to read from the file */
#define HEAPBUFSZ 1100 	   /* The size of one heap buffer ---
				(FILEBUFSZ + MAXKEYSZ) */


struct heapbuffer 
 {char keybuf[HEAPBUFSZ];	/* Buffer to hold the sort keys */
  char *key;     		/* Pointer to the next key in keybuf */
  char *eob;			/* The last character in the buffer */
  int bufct;			/* Counter of how many buffers have been
				     read in from disk file */
  int fd;			/* File descriptor of the file being merged */
  int fileno;			/* The number of the merge file */
  } HEAPBUFFER[MERGEORDER];
   





