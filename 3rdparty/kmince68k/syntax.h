/* -*-c,save-*- */

/*
 *	SYNTAX.H - Syntax table defs for Mince.
 *	The Syntax tables are used for such things as paren
 *	balancing in C and LISP modes
 *
 *	Robert Heller
 *	Last Mod. Sat Aug 24, 1985 21:00:17.44
 */

/* syntax table entry struct */
typedef struct {
    char s_kind;		/* kind of entry (ie word, paren, etc.) */
#define DULL	0		/* Dull boring characters */
#define WORD	1		/* characters that are part of words */
#define BEGP	2		/* beginning paren char */
#define ENDP	3		/* closing paren char */
#define PAIRQ	4		/* paired quote character */
#define PREQ	5		/* prefix quote character */
    char s_MP;			/* matching paren */
    } SyntaxEntry;

#define MAXCHARS 128

#define SynTab(name) SyntaxEntry name[MAXCHARS];

