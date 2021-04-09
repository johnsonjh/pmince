
/*
 *         1.1100 struct templatfld.  (The Template Field)
 */
struct templatfld
  {char tag0[MAXTOKSZ];       		/* Variable tag */
   int mapindex;			/* Index into screen map buffer */
   char positflg;			/* Flag indicating where this field
					    should appear in relation to   
					    the previous template field */
   struct logfield lfield;		/* Logical field */
   };

  			
/*	
 *            1.1110 Position Flag Codes.
 */
#define ADJACENT 0		/* Adjacent to previous field */
#define NEWLINE 1		/* On the line after previous field */
#define DONTCARE 2		/* Wherever it fits */



/*
 *   1.1000 struct template.     
 */
#define MAXNTFLDS 7 	     /* Maximum number of fields allowed in template */
struct template
  {char tag0[MAXTOKSZ];       		/* Structure variable tag */
   int nfields;			    	/* Number of fields in template */   
   struct templatfld tmfld[MAXNTFLDS];	/* Template Fields */
   };

/*
 *   1.200 struct scrmap. (The Screen Map Buffer) 
 */
#define MAPBUFSZ 1024	     /* Size of the screen map buffer */
struct scrmap 
 {int nextmap;			 /* Index of next available buffer position */
  char buffer[MAPBUFSZ];         /* Buffer in which screen map is stored */
  };

/*
 *      1.210 Screen Map Buffer Codes
 *	   1.211 Characters from ' ' -> 'z' appear on forms entry screen
 *		 at half intensity protected.
 *	   1.212 Logical Field Type Codes ALPHA, ALPHANUM, NUMERIC and STATUS
 *		 represent data positions of the respective types.
 *	   1.213 FILLMAP.  Fill to length specified in logical field structure
 *	         with data positions of appropriate type. 
 */
#define FILLMAP 255 	/* Fill Map with data positions */
/*
 *	   1.214 EOM. 
 */
#define EOM 0 		/* End of map */







/*
 * 1.0000 struct TMPLTLST.  (The Template List Structure) 
 */
#define MAXNTMPLT 7	     /* Maximum number of templates */
struct tmpltlist   
 {int ntmplts;				/* Number of templates defined */
  struct template templat[MAXNTMPLT];	/* List of templates defined */
  struct scrmap mapbuf;			/* Screen map buffer */
  };

