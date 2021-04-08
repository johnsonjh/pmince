/*****************************************************************
 *	VT52keys.C - key bindings for the VT52 keypad.
 *	Robert Heller (created: 10/25/84)
 *	NOTE:  DO NOT TRY TO COMPILE THIS FILE DIRECTLY.  IT IS
 *	CONDITIONALLY INCLUDED IN KEYPAD.C.
 *
 *-------------------------------------------------------------------------
 *
 *	(C) copyright 1984, by Robert Heller.  This document may be copied by
 *	anyone for any non-profit purpose as long as the following 2 conditions
 *	are observed:
 *	1.  Partial reproduction or duplication of the Keypad Editor for MINCE
 *	    is strictly forbidden.  DOCUMENTATION IS INCLUDED IN THIS
 *	    RESTRICTION.  Only the complete package may be copied, stored,
 *	    reproduced, or transmitted.
 *	2.  This copyright notice MUST accompany any and all parts of said
 *	    package as is exists in this document.  Any modification of this
 *	    copyright notice shall constitute violation of copyright.
 *
 *-------------------------------------------------------------------------
 *
 */

	functs[128+'?'] = MKeyPad;	/* M-? (escape prefix from keypad) */

/* now arrow keys: */

	functs[128+'B'] = MNextLin;	/* M-B (Down arrow key) */
	functs[128+'D'] = BWord;	/* M-D (Left arrow key) */
	functs[128+'C'] = FWord;	/* M-C (Right arrow key) */
	functs[128+'A'] = MPrevLin;	/* M-A (Up arrow key) */

/* "function" keys */

	functs[128+'P'] = MNotImpl;	/* M-P (Blue key) */
	functs[128+'Q'] = MQryRplc;	/* M-Q (Red key) */
	functs[128+'R'] = MReplace;	/* M-R (White key) */

	functs[128+'S'] = MNotImpl;	/* M-S (F1-key) */
	functs[128+'T'] = MNotImpl;	/* M-T (F2-key) */
	functs[128+'U'] = MNotImpl;	/* M-U (F3-key) */
	functs[128+'V'] = MNotImpl;	/* M-V (F4-key) */
	functs[128+'W'] = MNotImpl;	/* M-W (F5-key) */


