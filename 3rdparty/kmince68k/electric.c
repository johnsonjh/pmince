/* -*-c,save-*- */
/*
 * electric.c - General Electric Code
 * Robert Heller. Created: Thr Sep 26, 1985 19:53:58.50
 * Last Mod: 
 * 
 */

#include "mince.gbl"

/* initialization routine */

EInitAbrv()
{
    register int i;

    NormAbr[0].Abrev = NULL;		/* no abrevs in the normal table */
    
    CAbrvInit();
#ifdef LISPMODE
    LispAbInit();
#endif
    }
ECpyAbrv(to,from,count)
register ABREVENT *to,*from;
register int count;
{
    while(count-- > 0) {
	to->Abrev = from->Abrev;
	to->Expand = from->Expand;
	to->Abrevfun = from->Abrevfun;
	to++; from++;
	}
    }
EDoAbrev(ch)
register char ch;
{
    register int tmpmrk;
    int IsAlpha();
    char buffer[32];
    register char *p;
    regster int i,c;

    tmpmrk = BCreMrk();
    MovePast(IsAlpha,BACKWARD);
    p = (&buffer[0]); c = 0;
    while (BIsBeforeMrk(tmpmrk) && c < 31) {
	*p++ = Buff();
	BMove(1); c++;
	}
    if (BIsBeforeMrk(tmpmrk)) {
	BPntToMrk(tmpmrk);
	BKillMrk(tmpmrk);
	BInsert(ch);
	}
    else {
	*p = '\0';
	i = findabr(buffer,CurAbr);
	if (i < 0) {
	    BPntToMrk(tmpmrk);
	    BKillMrk(tmpmrk);
	    BInsert(ch);
	    }
	else {
	    p = CurAbr[i].Expand;
	    if (p != NULL) {
		BMove(-c);
		BDelete(c);
		while (*p != '\0') BInsert(*p++);
	    (*(CurAbr[i].Abrevfun))(ch);
	    BKillMrk(tmpmrk);
	    }
	}
    }
static findabr(str,abrptr)
register char *str;
register ABREVENT *abrptr;
{
    register int i;

    for (i=0;i < MAXABREVS && abrptr->Abrev != NULL;i++) {
	if (strcmp(str,abrptr->Abrev) == 0) return(i);
	abrptr++;
	}
    return(-1);
    }

