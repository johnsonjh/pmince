/*****************************************************************
 *	VT100keys.C - key bindings for the VT100 keypad.
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

	int MVT100Arw();

	functs[128+'O'] = MKeyPad;	/* M-? (escape prefix from keypad) */

/* now arrow keys: */

	functs[128+'['] = MVT100Arw;	/* M-[ (arrow key prefix) */



