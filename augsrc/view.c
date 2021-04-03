/* VIEW.C -- View Mode
   Contributed to the AUG by Mark of the Unicorn 07/20/81 

The seller of this software hereby disclaims any and all
guarantees and warranties, both express and implied.  No
liability of any form shall be assumed by the seller, nor shall
direct, consequential, or other damages be assumed by the seller.
Any user of this software uses it at his or her own risk.

Due to the ill-defined nature of "fitness for purpose" or similar
types of guarantees for this type of product, no fitness for any
purpose whatsoever is claimed or implied.

The physical medium upon which the software is supplied is
guaranteed for one year against any physical defect.  If it
should fail, return it to Mark of the Unicorn, and a new physical
medium with a copy of the purchased software shall be sent.

The seller reserves the right to make changes, additions, and
improvements to the software at any time; no guarantee is made
that future versions of the software will be compatible with any
other version.

The parts of this disclaimer are severable and fault found in
any one part does not invalidate any other parts.

	Copyright (c) 1981 by Mark of the Unicorn
	Created for Version 2.3 10/4/80 JTL

	This file contains the mode set up function */


SetModes()			/* Set the modes according to the bmodes array */
{
	int cnt;
	int MInsert(), MNotImpl();
	
	*mode='\0';
	finit1();					/* Set up the key bindings */
	finit2();
	finit3();
	for (cnt=MAXMODES-1; cnt>=0; --cnt) {
		switch (buffs[cbuff].bmodes[cnt]) {

		case 'v':
			Rebind(&MInsert,&MNotImpl);
			functs[4] = &MNotImpl;			/* C-D */
			functs[9] = &MNotImpl;			/* C-I */
			functs[11] = &MNotImpl;			/* C-K */
			functs[13] = &MNotImpl;			/* C-M */
			functs[15] = &MNotImpl;			/* C-O */
			functs[17] = &MNotImpl;			/* C-Q */
			functs[20] = &MNotImpl;			/* C-T */
			functs[23] = &MNotImpl;			/* C-W */
			functs[25] = &MNotImpl;			/* C-Y */
			functs[DEL] = &MNotImpl;			/* DEL */
			functs[128+11] = &MNotImpl;		/* M-C-K */
			functs[128+18] = &MNotImpl;		/* M-C-R */
			functs[128+'C']=functs[128+'c'] = &MNotImpl;	/* M-C */
			functs[128+'D']=functs[128+'d'] = &MNotImpl;	/* M-D */
			functs[128+'K']=functs[128+'k'] = &MNotImpl;	/* M-K */
			functs[128+'L']=functs[128+'l'] = &MNotImpl;	/* M-L */
			functs[128+'Q']=functs[128+'q'] = &MNotImpl;	/* M-Q */
			functs[128+'R']=functs[128+'r'] = &MNotImpl;	/* M-R */
			functs[128+'S']=functs[128+'s'] = &MNotImpl;	/* M-S */
			functs[128+'U']=functs[128+'u'] = &MNotImpl;	/* M-U */
			functs[128+'\\'] = &MNotImpl;		/* M-\ */
			functs[128+DEL] = &MNotImpl;		/* M-DEL */
			functs[256+'.'] = &MNotImpl;		/* C-X . */
			functs[256+'F']=functs[256+'f'] = &MNotImpl;	/* C-X F */
			strcat(&mode,"View ");
/* other modes here if you still want them */
			break;
			}
		}
	if (!*mode) strcpy(&mode,"Normal ");
	mode[strlen(mode)-1]='\0';
	ModeLine();
	}

/* THIS IS THE END OF VIEW.C */
