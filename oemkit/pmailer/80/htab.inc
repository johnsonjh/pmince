/*
		--------------------------
		|  Hash Table Structure  |
		--------------------------
*/
struct hashtable
 {char filename[MAXFNSZ];	/* The name of the hash table file */
  int fd;			/* File descriptor of hash table file */
  int nbuckets;    	/* The number of bucket slots in the hash table */
  int nextfree; 	/* Next free "collision" slot in hash table */
  char htabbuf[SECTORSZ];   /* Buffer holding the most recently read sector */
  int sectorno;		/* Number of the most recently read sector */
  };

/*
 *  Index Entry Structure
 */
struct htabentry
 {char memid[MEMIDSZ];	/* Member ID (which gets hashed) */
  int link;		/* Index of next link (used if collision) */
  int sector0;		/* First disk sector in which member is stored */
  } *entry();

#define ENTRYSZ 8	/* Number of bytes in a directory entry
				(MUST be a power of 2) */
