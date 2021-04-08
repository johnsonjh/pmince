/*****************************************************************
 *	H19keys.C - key bindings for the H-/Z-19 keypad.
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

/* now shifted (destructors) keypad keys (1-9) */

	functs[128+'L'] = MRDelLin;	/* M-L (Shifted 1-key) */
	functs[128+'B'] = MYank;	/* M-B (Shifted 2-key) */
	functs[128+'M'] = MDelLin;	/* M-M (Shifted 3-key) */
	functs[128+'D'] = MRDelWord;	/* M-D (Shifted 4-key) */
	functs[128+'H'] = MSetMrk;	/* M-H (Shifted 5-key) */
	functs[128+'C'] = MDelWord;	/* M-C (Shifted 6-key) */
	functs[128+'@'] = MRDelCHar;	/* M-@ (Shifted 7-key) */
	functs[128+'A'] = MDelRgn;	/* M-A (Shifted 8-key) */
	functs[128+'N'] = MDelChar;	/* M-N (Shifted 9-key) */

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
