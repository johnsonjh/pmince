/* Modifications Record:
		10/04/81	Moved in MCIndent and MSInsert from Comm1.C, so
				that they always get compiled but only get linked
                    in if they are referenced in CBIND.C (BAD). In order
				to get MSInsert to work, i added a new global in_cnt
				after the 1100 reserved integers in Mince.GBL .
                    The last few lines look like this now:
int spare[10];

char _reserved[1100];	/* Mark of the Unicorn's extra externals that we 
					   are not allowed to use any of. See AUG 1.4 for
					   more details. 10-3-81 BAD */

#ifdef LARGE
unsigned in_cnt;
#endif

/* END OF MINCE.GBL - Global variable definitions for Mince. */
-------------------------------------------------------------------------------
		07/23/81  Moved out MNewDsp, MSearch, MRSearch which all
                    appear in CBIND.C and moved all the Page
                    mode functions, for space. Moved functions appear
                    in COMM4.C, and are linked immediately AFTER the
                    -l option in L2.  Bad.
-------------------------------------------------------------------------------
		07/22/81	Added MNewDisp() which gets bound to C-L; the
                    purpose of MNewDsp() is to prevent any arguments to
                    a C-L from causing it to hang doing lots of senseless
                    work. I.E. the arg to MNewDsp is thrown away, and only
                    one NewDsp is ever done. BAD.

*/

/* COMM4.C	This is an optional part of the mince command set

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

	This file contains auxilliary (optional) command execution
routines for the mince editor. There are also a small number of
subordinate routines. */

#include "mince.gbl"

	/* Page mode commands */
MOverwrite()			/* overwrite instead of inserting */
{
	if (!BIsEnd() && !IsNL()) {
		MPFChar();
		BMove(-1);
		BDelete(1);
		}
	BInsert(cmnd);
	}

MPBegLine()			/* move to the first non-white character */
{
	int IsWhite();

	ToBegLine();
	MovePast(IsWhite,FORWARD);
	if (IsNL()) MDelWhite();
	}

MPBChar()				/* move to the left one character */
{
	ForceCol(BGetCol()-1,BACKWARD);
	}

MPFChar()				/* move to the right one character */
{
	ForceCol(BGetCol()+1,FORWARD);
	}

MPEndLine()			/* go to end of line less white space */
{
	ToEndLine();
	MDelWhite();
	}

MPBackSpace()			/* rub out previous character */
{
	if (BGetCol()==0) return;
	MPBChar();
	if (!BIsEnd() && !IsNL()) {
		MPFChar();
		BMove(-1);
		BDelete(1);
		}
	BInsert(' ');
	BMove(-1);
	}
	
MPTab()				/* move over a tab stop */
{
	ForceCol(BGetCol()+TWidth(BGetCol(),TAB),FORWARD);
	}

MPNextLine()			/* move to the next line in exact same column */
{
	MNextLin();
	ForceCol(lcol,FORWARD);
	}

MPPrevLine()			/* move to previous column in same column */
{
	MPrevLin();
	ForceCol(lcol,FORWARD);
	}

	/* End of Page mode commands*/ 

MNewDsp()				/* redisplay hack so that redisplay with an arg */
					/* only calls NewDsp() once. (BAD) */
{
	NewDsp();
	arg = 0;
	}

MRSearch()			/* reverse string search */
{
	if (!GetArg("Reverse Search <ESC>: ",ESC,strarg,STRMAX)) {
		arg=0;
		return;
		}
	tmark=BCreMrk();
	while (arg-- > 0) if (!StrSrch(strarg,BACKWARD)) {
		BPntToMrk(tmark);
		Error("Not Found");
		break;
		}
	BKillMrk(tmark);
	arg=0;
	ClrEcho();
	}

MSearch()				/* forward string search */
{
	if (!GetArg("Forward Search <ESC>: ",ESC,strarg,STRMAX)) {
		arg=0;
		return;
		}
	tmark=BCreMrk();
	while (arg-- > 0) if (!StrSrch(strarg,FORWARD)) {
		BPntToMrk(tmark);
		Error("Not Found");
		break;
		}
	BKillMrk(tmark);
	arg=0;
	ClrEcho();
	}


	/* C Mode commands */
MCIndent()			/* Indent for C code */
{
	int IsWhite(), wid;

	tmark=BCreMrk();
	ToBegLine();
	MovePast(IsWhite,FORWARD);
	if (BIsAfter(tmark)) BPntToMrk();
	wid=BGetCol();
	while (BIsBefore(tmark)) {
		if (Buff()=='{') wid += indentcol;
		else if (Buff()=='}') wid -= indentcol;
		BMove(1);
		TKbChk();
		}
	BPntToMrk(tmark);
	BKillMrk(tmark);
	BInsert(NL);
	TIndent(wid);
	}

	/* Auto save mode */
MSInsert()			/* Auto save self insert */
{
/*	static unsigned in_cnt;   i put this in mince.gbl 10-3-81 BAD */

	BInsert(cmnd);
	if (++in_cnt>256) {
		MFileSave();
		in_cnt=0;
		}
	}

/* END OF COMM4.C */
