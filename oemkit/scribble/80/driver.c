/* DRIVER.C		Specialized printer drivers

	Copyright 1981 by Mark of the Unicorn, Inc.
	Created 81.6.1 Gyro

This file contains the drivers for various kinds of printers.  */

#include "crayon.h"


DrvInit (vpos)
	int vpos;
{
	OPutCmd (&dev.initstr);
	switch (dev.prtcode) {
	
	case VANILLA:
		VInit (vpos);
		break;
	case DIABLO:
		DbInit (vpos);
		break;
	case EPSON:
		EpInit (vpos);
		break;
	case IDS460:
		IDSInit (vpos);
		break;
	case SPINWRITER:
		SpInit (vpos);
		break;
/*	case CUSTOM:
		CusInit (vpos);
		break;
	case QUME:
		QmInit (vpos);
		break;
*/	case CEN737:
		CenInit (vpos);
		break;
		}
	}


DrvFini()
{
	switch (dev.prtcode) {
	
	case VANILLA:
		VFini();
		break;
	case DIABLO:
		DbFini();
		break;
	case EPSON:
		EpFini();
		break;
	case IDS460:
		IDSFini();
		break;
	case SPINWRITER:
		SpFini();
		break;
/*	case CUSTOM:
		CusFini();
		break;
	case QUME:
		QmFini();
		break;
*/	case CEN737:
		CenFini();
		break;
		}
	OPutCmd (dev.finistr);
	}


DrvPrtMove (c, hpos, vpos)		/* print c and move to hpos, vpos */
	int c, hpos, vpos;
{
	switch (dev.prtcode) {
	
	case VANILLA:
		VPrtMove (c, hpos, vpos);
		break;
	case DIABLO:
		DbPrtMove (c, hpos, vpos);
		break;
	case EPSON:
		EpPrtMove (c, hpos, vpos);
		break;
	case IDS460:
		IDSPrtMove (c, hpos, vpos);
		break;
	case SPINWRITER:
		SpPrtMove (c, hpos, vpos);
		break;
/*	case CUSTOM:
		CusPrtMove (c, hpos, vpos);
		break;
	case QUME:
		QmPrtMove (c, hpos, vpos);
		break;
*/	case CEN737:
		CenPrtMove (c, hpos, vpos);
		break;
		}
	}


FLAG
DrvInfo (what)				/* ask about printer */
	int what;
{
	switch (dev.prtcode) {
	
	case VANILLA:
		return (VInfo (what));
		break;
	case DIABLO:
		return (DbInfo (what));
		break;
	case EPSON:
		return (EpInfo (what));
		break;
	case IDS460:
		return (IDSInfo (what));
		break;
	case SPINWRITER:
		return (SpInfo (what));
		break;
/*	case CUSTOM:
		return (CusInfo (what));
		break;
	case QUME:
		return (QmInfo (what));
		break;
*/	case CEN737:
		return (CenInfo (what));
		break;
		}
	}


/**************** Vanilla driver ****************/

VInit (vpos)
	int vpos;
{
	if (dev.barecr) OPutChar (CR);
	drvhpos = 0;
	drvvpos = vpos;
	}


VFini()
{
	}


VPrtMove (c, hpos, vpos)
	int c, hpos, vpos;
{
	int delh, delv;
	
	if (c != NOCHAR) {
		OPutChar (c);
		drvhpos += DWidChar (c);
		}
	delh = hpos - drvhpos;
	delv = vpos - drvvpos;
	if (delh < 0) {
		if (delv) {
			OPutCmd (&dev.donewline);
			delv -= linev;
			delh = hpos;
			}
		else if (dev.barecr
			    &&  (!dev.backspace
			    ||  -delh > hpos + 1)) {
			OPutChar (CR);
			delh = hpos;
			}
		else if (dev.backspace) while (delh < 0) {
			OPutChar (BS);
			delh += spaceh;
			}
		}
	if (dev.formfeed) 					/* reconstruct form feeds */
		while (delv > (vpos % pagev)) {	/* believe it or not! */
			OPutChar (FF);
			delv -= pagev - (vpos - delv) % pagev;
			}
	while ((delv -= linev) >= 0) {
		OPutCmd (&dev.donewline);
		delh=hpos;
		}
	while ((delh -= spaceh) >= 0) OPutChar (' ');
	drvhpos = hpos;
	drvvpos = vpos % pagev;
	}


FLAG
VInfo (what)				/* ask about printer */
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
		return (dev.barecr  ||  dev.backspace);
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (FALSE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("VInfo: unknown request\n");
		}
	}


/**************** Diablo driver ****************/


DbInit (vpos)
	int vpos;
{
	drvvpos = vpos;
	OPutChar (CR);
	drvhpos = 0;
	drvhpitch = drvvpitch = drvdirec = UNINITIALIZED;
	drvshiftp = TRUE;
	DbSHPitch (spaceh);
	DbSVPitch (linev);
	DbSDirec (FORWARD);
	DbSShift (FALSE);
	}


DbFini()
{
	DbSHPitch (spaceh);
	DbSVPitch (linev);
	DbSDirec (FORWARD);
	DbSShift (FALSE);
	}


DbPrtMove (c, hpos, vpos)	/* print c and move to hpos, vpos */
	int c, hpos, vpos;
{
	int delh;
	
	delh = hpos - drvhpos;
	if (c != NOCHAR) {
		if ((!DbSpcOk (delh)  &&  !DbTabOk (hpos)  &&  abs (delh) <= 125)
		    ||  (abs (delh) == spaceh  &&  !dev.isprop))
			DbSHPitch (abs (delh));
		DbSDirec ((delh < 0) ? BACKWARD : FORWARD);
		DbSShift ((c & 128) ? TRUE : FALSE);
		OPutChar (c & 127);
		drvhpos += drvhpitch * drvdirec;
		}
	DbMoveH (hpos);
	DbMoveV (vpos);
	}


DbMoveH (hpos)				/* horizontal move */
	int hpos;
{
	int delh;
	
	delh = hpos - drvhpos;
	if (!delh) return;
	drvhpos = hpos;
	if (DbSpcOk (delh)
	    &&  (abs (delh) / drvhpitch < 4  ||  !DbTabOk (hpos))) {
		while (delh)
			if (delh * drvdirec > 0) {
				OPutChar (' ');
				delh -= drvhpitch * drvdirec;
				}
			else {
				OPutChar (BS);
				delh += drvhpitch * drvdirec;
				}
		return;
		}
	if (abs (delh) > 125  ||  DbTabOk (hpos)) {
		if (drvhpitch == 0  ||  hpos / drvhpitch > 125)
			DbSHPitch ((hpos / spaceh <= 125) ? spaceh : 13);
		DbHTab (hpos / drvhpitch);
		delh = hpos % drvhpitch;
		}
	if (delh) {
		DbSHPitch (abs (delh));
		OPutChar ((delh * drvdirec < 0) ? BS : ' ');
		}
	}


DbSpcOk (delh)				/* can we move this far by spaces? */
	int delh;
{
	return (drvhpitch != 0
		   &&  abs (delh) % drvhpitch == 0
		   &&  abs (delh) / drvhpitch < 7);
	}


DbTabOk (hpos)				/* can we get here with abs horiz tab? */
	int hpos;
{
	return (drvhpitch != 0
		   &&  hpos % drvhpitch == 0
		   &&  hpos / drvhpitch <= 125);
	}


DbMoveV (vpos)				/* vertical move */
	int vpos;
{
	int delv;
	
	delv = vpos - drvvpos;
	if (!delv) return;
	drvvpos = vpos % pagev;
	while (delv) {
		DbSVPitch (min (delv, 125));
		OPutChar (LF);
		delv -= drvvpitch;
		}
	}


DbSHPitch (hpitch)			/* set horizontal pitch */
	int hpitch;
{
	if (hpitch == drvhpitch) return;
	OPutSeq (ESC, US, hpitch + 1, ENDLIST);
	drvhpitch = hpitch;
	}


DbSVPitch (vpitch)			/* set vertical pitch */
	int vpitch;
{
	if (vpitch == drvvpitch) return;
	OPutSeq (ESC, RS, vpitch + 1, ENDLIST);
	drvvpitch = vpitch;
	}


DbSDirec (direc)			/* set print direction */
	int direc;
{
	if (direc == drvdirec) return;
	OPutSeq (ESC, (direc == FORWARD) ? '5' : '6', ENDLIST);
	drvdirec = direc;
	}


DbHTab (pos)				/* abs tab to pos */
	int pos;
{
	OPutSeq (ESC, TAB, pos + 1, ENDLIST);
	}


DbSShift (shiftp)			/* shift on or off (only for NEC 5515/25) */
	FLAG shiftp;
{
	if (drvshiftp == shiftp) return;
	OPutChar (shiftp ? SO : SI);
	drvshiftp = shiftp;
	}


FLAG
DbInfo (what)
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (TRUE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("DbInfo: unknown request\n");
		}
	}


/**************** Epson driver ****************/

EpInit (vpos)
	int vpos;
{
	OPutChar (CR);
	drvhpos = 0;
	drvvpos = vpos;
	drvvpitch = UNINITIALIZED;
	}


EpFini()
{
	EpSVPitch (linev);
	}


EpPrtMove (c, hpos, vpos)
	int c, hpos, vpos;
{
	int delh, delv;
	
	if (c != NOCHAR) {
		OPutChar (c);
		drvhpos += DWidChar (c);
		}
	delh = hpos - drvhpos;
	delv = vpos - drvvpos;
	if (delh < 0  &&  !delv) {
		OPutChar (CR);
		delh = hpos;
		}
	while (delv) {
		EpSVPitch (min (delv, 85));
		OPutCmd (&dev.donewline);
		delv -= drvvpitch;
		delh = hpos;
		}
	while ((delh -= spaceh) >= 0) OPutChar (' ');
	drvhpos = hpos;
	drvvpos = vpos % pagev;
	}


EpSVPitch (vpitch)			/* set vertical pitch */
	int vpitch;
{
	if (drvvpitch == vpitch) return;
	OPutSeq (ESC, 'A', vpitch, ESC, '2', ENDLIST);
	drvvpitch = vpitch;
	}


FLAG
EpInfo (what)				/* ask about printer */
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
		return (TRUE);
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (FALSE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("EpInfo: unknown request\n");
		}
	}


/**************** IDS 460 driver ****************/


IDSInit (vpos)
	int vpos;
{
	drvvpos = vpos;
	drvhpos = 0;
	drvvpitch = UNINITIALIZED;
	OPutChar (CR);
	}


IDSFini()
{
	IDSSVPitch (linev);
	}


IDSPrtMove (c, hpos, vpos)
	int c, hpos, vpos;
{
	int delh, delv;
	
	if (c != NOCHAR) {
		OPutChar (c);
		drvhpos += DWidChar (c);
		}
	delh = hpos - drvhpos;
	delv = vpos - drvvpos;
	while (delv) {
		IDSSVPitch (min (delv, 255));
		OPutCmd (&dev.donewline);
		delv -= drvvpitch;
		delh = hpos;
		}
	if (delh < 0) {
		OPutChar (CR);
		delh = hpos;
		}
	if (delh % spaceh) {
		IDSHTab (hpos);
		delh = 0;
		}
	while (delh >= spaceh) {
		OPutChar (' ');
		delh -= spaceh;
		}
	drvhpos = hpos;
	drvvpos = vpos % pagev;
	}


IDSSVPitch (vpitch)
	int vpitch;
{
	if (drvvpitch != vpitch) {
		OUnInter (6);
		OPutChar (ESC);
		OPutChar ('B');
		OPutNum (vpitch);
		OPutChar (CR);
		drvvpitch = vpitch;
		}
	}


IDSHTab (pos)				/* abs horizontal tab to <pos> */
	int pos;
{
	OUnInter (7);
	OPutChar (ESC);
	OPutChar ('G');
	OPutNum (pos);
	OPutChar (CR);
	}


FLAG
IDSInfo (what)				/* ask about printer */
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
		return (TRUE);
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (FALSE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("IDSInfo: unknown request\n");
		}
	}



/**************** Spinwriter driver ****************/


SpInit (vpos)
	int vpos;
{
	drvvpos = vpos;
	drvhpos = 0;
	drvhpitch = drvvpitch = drvdirec = UNINITIALIZED;
	drvshiftp = TRUE;
	SpSHPitch (spaceh);
	SpSVPitch (linev);
	SpSDirec (FORWARD);
	SpSShift (FALSE);
	}


SpFini()
{
	SpSHPitch (spaceh);
	SpSVPitch (linev);
	SpSDirec (FORWARD);
	SpSShift (FALSE);
	}


SpPrtMove (c, hpos, vpos)	/* print c and move to hpos, vpos */
	int c, hpos, vpos;
{
	int delh;
	
	delh = hpos - drvhpos;
	if (c != NOCHAR) {
		if ((!SpSpcOk (delh)  &&  !SpTabOk (hpos)  &&  abs (delh) <= 15)
		    ||  (abs (delh) == spaceh  &&  !dev.isprop))
			SpSHPitch (abs (delh));
		SpSDirec ((delh < 0) ? BACKWARD : FORWARD);
		SpSShift ((c & 128) ? TRUE : FALSE);
		OPutChar (c & 127);
		drvhpos += drvhpitch * drvdirec;
		}
	SpMoveH (hpos);
	SpMoveV (vpos);
	}


SpMoveH (hpos)				/* horizontal move */
	int hpos;
{
	int delh;
	
	delh = hpos - drvhpos;
	if (!delh) return;
	drvhpos = hpos;
	if (SpSpcOk (delh)
	    &&  (abs (delh) / drvhpitch < 4  ||  !SpTabOk (hpos))) {
		while (delh)
			if (delh * drvdirec > 0) {
				OPutChar (' ');
				delh -= drvhpitch * drvdirec;
				}
			else {
				OPutChar (BS);
				delh += drvhpitch * drvdirec;
				}
		return;
		}
	if (abs (delh) > 15  ||  SpTabOk (hpos)) {
		SpHTab (hpos / (dev.isprop ? 10 : charh));
		delh = hpos % (dev.isprop ? 10 : charh);
		}
	if (delh) {
		SpSHPitch (abs (delh));
		OPutChar ((delh * drvdirec < 0) ? BS : ' ');
		}
	}


SpSpcOk (delh)				/* can we move this far by spaces? */
	int delh;
{
	return (drvhpitch != 0
		   &&  abs (delh) % drvhpitch == 0
		   &&  abs (delh) / drvhpitch < 10);
	}


SpTabOk (hpos)				/* can we get here with abs horiz tab? */
	int hpos;
{
	return (hpos % (dev.isprop ? 10 : charh) == 0);
	}


SpMoveV (vpos)				/* vertical move */
	int vpos;
{
	int delv;
	
	delv = vpos - drvvpos;
	if (!delv) return;
	drvvpos = vpos % pagev;
	/* this ought to do abs vertical tab ??? */
	while (delv) {
		SpSVPitch (min (delv, 16));
		OPutChar (LF);
		delv -= drvvpitch;
		}
	}


SpSHPitch (hpitch)			/* set horizontal pitch */
	int hpitch;
{
	if (hpitch == drvhpitch) return;
	OPutSeq (ESC, ']', hpitch + '@', ENDLIST);
	drvhpitch = hpitch;
	}


SpSVPitch (vpitch)			/* set vertical pitch */
	int vpitch;
{
	if (vpitch == drvvpitch) return;
	OPutSeq (ESC, ']', vpitch + 'O', ENDLIST);
	drvvpitch = vpitch;
	}


SpSDirec (direc)			/* set print direction */
	int direc;
{
	if (direc == drvdirec) return;
	OPutSeq (ESC, (direc == FORWARD) ? '>' : '<', ENDLIST);
	drvdirec = direc;
	}


SpHTab (pos)				/* abs tab to pos */
	int pos;
{
	OPutSeq (ESC, pos / 32 + 'P', pos % 32 + '@', ENDLIST);
	}


SpSShift (shiftp)			/* shift on or off */
	FLAG shiftp;
{
	if (drvshiftp == shiftp) return;
	OPutChar (shiftp ? SO : SI);
	drvshiftp = shiftp;
	}


FLAG
SpInfo (what)
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (TRUE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("SpInfo: unknown request\n");
		}
	}



/**************** Centronics driver ****************/

CenInit (vpos)
	int vpos;
{
	OPutChar (CR);
	drvhpos = 0;
	drvvpos = vpos;
	}


CenFini()
{
	}


CenPrtMove (c, hpos, vpos)
	int c, hpos, vpos;
{
	int delh, delv;
	
	if (c != NOCHAR) {
		OPutChar (c);
		drvhpos += DWidChar (c);
		}
	delh = hpos - drvhpos;
	delv = vpos - drvvpos;
	while (delv >= linev) {
		OPutChar (LF);
		delv -= linev;
		}
	if (delv == linev / 2) OPutSeq (ESC, 28, ENDLIST);/* half line feed */
	if (delh < 0) {
		OPutChar (CR);
		delh = hpos;
		}
	while (delh >= spaceh) {
		OPutChar (' ');
		delh -= spaceh;
		}
	while (delh > 0) {
		OPutSeq (ESC, min (delh, 6), ENDLIST);
		delh -= min (delh, 6);
		}
	drvhpos = hpos;
	drvvpos = vpos % pagev;
	}


FLAG
CenInfo (what)				/* ask about printer */
	int what;
{
	switch (what) {
	
	case OVP:					/* can it overprint? */
		return (TRUE);
	case CENTCHAR:				/* does it center characters? */
	case BIDIREC:				/* does it need explicit bidirection? */
		return (FALSE);
	case BOLD:				/* can it boldface innately? */
		return (dev.boldon.len != 0);
	case ITALIC:				/* can it italic innately? */
		return (dev.italon.len != 0);
	default:
		Fatal ("CenInfo: unknown request\n");
		}
	}


/* End of DRIVER.C  --  specialized printer drivers */

