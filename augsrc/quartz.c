/* QUARTZ.C - Alternate Command Set (a mode)
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

	Copyright (c) 1981 by Mark of the Unicron
	Created for Version 2.3 10/4/80 Fin

	This file contains the routines and bindings necessary in
order to implement the alternate command set */

/*
	Do not forget to add MDelBegLine, MDelEndLine, MDBSent, and
MDFSent to the list in DelCmnd.

*/

#include "mince.gbl"

SetModes()			/* Set the modes according to the bmodes array */
{
	int cnt;
	int MBLine(), MDelBegLine(), MFLine(), MDelEndLine();
	int MPrevPage(), MMrkPara(), MDBSent(), BWord();
	int MBPara(), MFPara(), MSwitchTo(), MKillBuff();
	int MLstBuffs(), MToLastBuff(), MSetIndent();

	*mode='\0';
	finit1();					/* Set up the key bindings */
	finit2();
	finit3();
	for (cnt=MAXMODES-1; cnt>=0; --cnt) {
		switch (buffs[cbuff].bmodes[cnt]) {

		case 'q':
			functs[1] = &MBLine;		/* C-A */
			functs[2] = &MDelBegLine;	/* C-B */
			functs[5] = &MFLine;		/* C-E */
			functs[11] = &MDelEndLin;	/* C-K */
			functs[26] = &MPrevPage;		/* C-Z */
			functs[128+17] = &MMrkPara;				/* M-C-Q */
			functs[128+'B']=functs[128+'b'] = &MDBSent;	/* M-B */
			functs[128+'H']=functs[128+'h'] = &BWord;	/* M-H */
			functs[128+'N']=functs[128+'n'] = &MFPara;	/* M-N */
			functs[128+'P']=functs[128+'p'] = &MBPara;	/* M-P */
			functs[256+2] = &MSwitchTo;				/* C-X C-B */
			functs[256+11] = &MKillBuff;				/* C-X C-K */
			functs[256+12] = &MLstBuffs;				/* C-X C-L */
			functs[256+26] = &MToLastBuff;			/* C-X C-Z */
			functs[256+'I']=functs[256+'i'] = &MSetIndent;/* C-X I */
			strcat(&mode,"Quartz ");
			break;
/* other modes here if you still want them */
			}
		}
	if (!*mode) strcpy(&mode,"Normal ");
	mode[strlen(mode)-1]='\0';
	ModeLine();
	}

MBLine()				/* move to the beginning of the line */
{
	BMove(-1);
	ToBegLine();
	}

MDelBegLine()			/* delete to the beginning of the line */
{
	tmark=BCreMrk();
	MBLine();
	KillToMrk(tmark,BACKWARD);
	BKillMrk(tmark);
	}

MFLine()				/* move to the end of the line */
{
	BMove(1);
	ToEndLine();
	}

MDelEndLine()			/* delete to the end of the line */
{
	tmark=BCreMrk();
	MFLine();
	KillToMrk(tmark,FORWARD);
	BKillMrk(tmark);
	}

MBDSent()				/* backward delete sentence */
{
	int tmark;

	tmark=BCreMrk();
	MBSent();
	KillToMrk(tmark,BACKWARD);
	BKillMrk(tmark);
	}

MDelSent()			/* delete sentence */
{
	int tmark;

	tmark=BCreMrk();
	MFSent();
	KillToMrk(tmark,FORWARD);
	BKillMrk(tmark);
	}

MToLastBuff()			/* switch buffers */
{
	int cnt;

	arg=0;
	cnt=CFindBuff(namearg);
	if (cnt<0 && (!Ask("Create new buffer?") ||
		(cnt=CMakeBuff(namearg,"DELETE.ME"))<0)) return;
	CSwitchTo(cnt);
	ScrnRange();
	}

/* THIS IS THE END OF QUARTZ.C */
