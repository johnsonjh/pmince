/* -*-c,save-*- */
/*
 * ELECTRIC.H - Electric Code header file
 * Robert Heller. Created: Thr Sep 26, 1985 19:57:58.48
 * Last Mod: 
 * 
 */

/* abreviation table entry */
typedef struct {
    char *Abrev;		/* abreviation */
    char *Expand;		/* full form */
    int (*Abrevfun)();		/* function to call on expandsion */
    } ABREVENT;

#define MAXABREVS 64		/* modest sized table.  i'll see how things go */

/* macro to define an abreviation table */
#define AbrevTab(NAME) ABREVENT NAME[MAXABREVS];

