/* COMM2.C	This is part two of the mince command set

The seller of this software hereby disclaim any and all
guarantees and warranties, both express and implied.  No
liability of any form shall be assumed by the seller, nor shall
direct, consequential, or other damages be assumed by the seller.
Any user of this software uses it at his or her own risk.

Due to the ill-defined nature of "fitness for purpose" or similar
types of guarantees for this type of product, no fitness for any
purpose whatsoever is claimed or implied.

The physical medium upon which the software is supplied is
guaranteed for one year against any physical defect.  If it
should fail, return it to the seller, and a new physical medium
with a copy of the purchased software shall be sent.

The seller reserve the right to make changes, additions, and
improvements to the software at any time; no guarantee is made
that future versions of the software will be compatible with any
other version.

The parts of this disclaimer are severable and fault found in any
one part does not invalidate any other parts.

	Copyright (c) 1981 by Mark of the Unicorn
	Created for version two  8/27/80  JTL
	Updated to version three  1/7/81  JTL

	This file contains the Meta command execution routines for
the mince editor. There are also a small number of subordinate
routines. */

#include "mince.gbl"		/* Only when seperate files */

MDelELin()			/* delete entire line */
{
	tmark=BCreMrk();
	ToBegLine();
	KillToMrk(tmark,BACKWARD);
	NLSrch();
	lfunct = MDelELin;
	KillToMrk(tmark,FORWARD);
	BKillMrk(tmark);
	}

MQryRplc()			/* Query replace string */
{
	DoReplace(TRUE);
	}

MMakeDel()			/* make the previous command a kill */
{
	}

MToStart()			/* move to the begining of the buffer */
{
	BMrkToPnt(mark);
	BToStart();
	}

MToEnd()				/* move to the end of the buffer  */
{
	BMrkToPnt(mark);
	BToEnd();
	}

MBSent()				/* move backwards one sentence */
{
	BWord();
	ToSentEnd(BACKWARD);
	if (!BIsStart()) ToWhite(FORWARD);
	ToNotWhite(FORWARD);
	}
	
BWord()				/* Move backwards a word of text */
{
	int IsToken();

	MoveTo(IsToken,BACKWARD);
	MovePast(IsToken,BACKWARD);
	}

MCapWord()			/* capitalize word */
{
	ToWord();
	if (BIsEnd()) return;
	BInsert(ToUpper(Buff()));
	BDelete(1);
	if (IsToken()) MLowWord();
	}
	
MDelWord()			/* delete the current word */
{
	tmark=BCreMrk();
	FWord();
	KillToMrk(tmark,FORWARD);
	BKillMrk(tmark);
	}

MFSent()				/* move forward a sentence */
{
	FWord();
	ToSentEnd(FORWARD);
	}

FWord()				/* Move foward a word of text */
{
	int IsToken();

	MoveTo(IsToken,FORWARD);
	MovePast(IsToken,FORWARD);
	}
	
MMrkPara()			/* set point and mark around paragraph */
{
	BMove(-1);
	MFPara();
	BMrkToPnt(mark);
	MBPara();
	}

MDelSent()			/* delete sentence */
{
	int tmark;

	tmark=BCreMrk();
	if (arg==0) MBSent();
	else MFSent();
	KillToMrk(tmark,arg!=0);
	BKillMrk(tmark);
	}

MLowWord()			/* lowercase a word */
{
	ToWord();
	while (!BIsEnd() && IsToken()) {
		BInsert(ToLower(Buff()));
		BDelete(1);
		TKbChk();
		}
	}
	
MFillPara()			/* fill paragraph */
{
	int IsWhite();

	if (argp) fillwidth=arg;
	arg=0;
	tmark=BCreMrk();
	BMove(-1);
	MFPara();
	BMove(-1);
	if (BIsEnd() || IsGray()) {
		BPntToMrk(tmark);
		BKillMrk(tmark);
		return;
		}
	BMove(1);
	tmp=BCreMrk();
	MBPara();

	while (BIsBefore(tmp)) {
		ToWhite(FORWARD);
		if (BGetCol() > fillwidth) {
			ToWhite(BACKWARD);
			MDelWhite();
			BInsert(NL);
			SIndent(indentcol);
			ToWhite(FORWARD);
			}
		MovePast(IsWhite,FORWARD);
		if (IsNL() && BIsBefore(tmp)) {
			BDelete(1);
			MDelWhite();
			BInsert(' ');
			}
		}

	BPntToMrk(tmark);
	BKillMrk(tmark);
	BKillMrk(tmp);
	}

MReplace()			/* replace string */
{
	DoReplace(FALSE);
	}

MCntrLine()			/* center the line */
{
	if (argp) fillwidth=arg;
	else arg=fillwidth;
	if ((arg -= indentcol) < 1) return;
	ToBegLine();
	MDelWhite();
	ToEndLine();
	tmp=BGetCol();
	if (tmp<=arg) {
		ToBegLine();
		TIndent(indentcol+(arg-tmp)/2);
		ToEndLine();
		}
	arg=0;
	}

MSwapWord()			/* transpose words */
{
	int IsToken();

	MoveTo(IsToken,FORWARD);
	if (BIsEnd()) return;
	tmark=BCreMrk();
	MovePast(IsToken,FORWARD);
	tmp=BCreMrk();
	BPntToMrk(tmark);
	MoveTo(IsToken,BACKWARD);
	BlockMove(tmark,tmp);

	MovePast(IsToken,BACKWARD);
	BlockMove(tmark,tmp);
	BKillMrk(tmark);
	BPntToMrk(tmp);
	BKillMrk(tmp);
	}

MUpWord()				/* uppercase a word */
{
	ToWord();
	while (!BIsEnd() && IsToken()) {
		BInsert(ToUpper(Buff()));
		BDelete(1);
		TKbChk();
		}
	}
	
MPrevPage()			/* move to the previous page */
{
	BPntToMrk(sstart);
	for (cnt=WHeight()-PrefLine()-2; cnt>0; --cnt) RNLSrch();
	ToBegLine();
	ScrnRange();
	}

MCopyRgn()			/* copy region to kill buffer */
{
	CopyToMrk(mark,BIsBefore(mark));
	}

MBPara()				/* move backward one paragraph */
{
	ToNotWhite(BACKWARD);
	while (RNLSrch()) {
		BMove(1);
		if (IsParaEnd()) break;
		BMove(-1);
		TKbChk();
		}
	ToNotWhite(FORWARD);
	}

MDelWhite()			/* delete spaces and tabs around point */
{
	while (!BIsEnd() && IsWhite()) {
		BDelete(1);
		TKbChk();
		}
	while (!BIsStart()) {
		BMove(-1);
		if (IsWhite()) BDelete(1);
		else {
			BMove(1);
			break;
			}
		TKbChk();
		}
	}

MFPara()				/* move forward one paragraph */
{
	ToNotWhite(FORWARD);
	while (NLSrch() && !IsParaEnd()) TKbChk();
	ToNotWhite(BACKWARD);
	}

MRDelWord()			/* delete the previous word */
{
	tmark=BCreMrk();
	BWord();
	KillToMrk(tmark,BACKWARD);
	BKillMrk(tmark);
	}

MNotImpl()			/* Command not implemented */
{
	Error("Unknown command");
	arg=0;
	}

	/* Fill mode commands */
MFillChk()			/* check to see if we are beyond fillwidth
					   and do something about it */
{
	if (BGetCol() < fillwidth) {
		BInsert(' ');
		}
	else {
		tmark=BCreMrk();
		while (BGetCol() > fillwidth) {
			ToWhite(BACKWARD);
			ToNotWhite(BACKWARD);
			}
		MDelWhite();
		tmp = !BIsAtMrk(tmark);		/* does trailing spaces */
		BInsert(NL);
		SIndent(indentcol);
		if (tmp) {
			BPntToMrk(tmark);
			BInsert(' ');
			}
		BKillMrk(tmark);
		}
	}

/* END OF COMM2.C - Mince command routines */

