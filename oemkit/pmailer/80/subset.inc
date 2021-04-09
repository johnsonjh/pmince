/*
			---------------------- 
			| Subset Use Options |
			----------------------
*/
#define DEFINESUBSETS 0		/* Define subsets */
#define USESUBSETS 1		/* Use subsets already defined */


#define SSOFFSET 15		/* Enough room for the description of
				     the default subset of "All Members" */

/*
			------------------------
			| Relational Operators |
			------------------------ 
*/
#define	EQ 0		/* Equal */
#define GT 1		/* Greater Than */
#define LT 2		/* Less Than */
#define GE 3		/* Greater or equal */
#define LE 4		/* Less or equal */
#define NE 5		/* Not Equal */



#define RELOPS "Equal,Greater,Less,Greater or Equal,Less or Equal,Not Equal"



/*
			--------------------------------
			| Subset Definition Structures |
			--------------------------------
*/

#define MAXNSS 21		/* Maximum number of subsets allowed */
#define NSSMEM 2		/* Number of subset selection criteria
				     members */
#define SSDSCLSTSZ 600		/* Size of subset descriptor list */




struct subset
  {char ssmem[NSSMEM][MEMBUFSZ];   /* Primary and limit members */
   char relop[NSSMEM][MAXNLFLDS];  /* Respective relational operator vectors */
   };




/*
			---------------------
			| Subset Disk Entry |
			---------------------
*/
struct ssentry0			
  {int sectorst;  		/* Start sector */
   int sectorct;		/* Number of sectors */
   };


		
/*
			--------------------
			| Subset Directory |
			--------------------
*/
struct ssdirectory
  {int nsubsets;			/* Number of subset defined */
   int nextsssect;			/* Next disk sector available */
   int ssfd;				/* File descriptor of subset file */ 
   struct ssentry0 ssentry[MAXNSS];	/* Subset Disk Entries */
   char ssdesclist[SSDSCLSTSZ];		/* Subset description list */
   };










