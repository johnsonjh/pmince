/* PRINTER.C		Printer abstraction and support routines

	Copyright (C) 1981 by Mark of the Unicorn, Inc.
	Created 81.3.18 Gyro

This code implements a generalized printer abstraction for use by
Crayon or Pencil. */


#include "crayon.h"



FLAG
PInit (device, portname, hackp)	/* initialize printer */
	char *device, *portname;
	FLAG hackp;				/* do paper hack? */
{
	DInit (device, portname, hackp);
	phpos = 0;
	if (hackp) pvpos = PUnMica (dev.paperoffset, VERTICAL);
	else pvpos = 0;
	hackdone = hackp;
	ulonp = FALSE;
	nuls = 0;
	ulmap[0].uloff = -1;
	boldp = italicp = FALSE;
	ulitalp = !DrvInfo (ITALIC);
	olen = 0;
	warned = FALSE;
	}


PFini()					/* turn things off */
{
	PCR();
	PNewPage();
	if (hackdone) PMoveV (PUnMica (dev.paperoffset, VERTICAL));
	PForce();
	DFini();
	}


PPrtChar (c)				/* print a character */
	char c;
{
	FLAG ulreally;
	
	if ((ulonp  ||  italicp)
	    &&  (ulonp == UNALLON
		    ||  ((ulonp == UNNBON  ||  (italicp && ulitalp))  &&  c != ' ')
		    ||  (ulonp == UNANON  &&  (isalpha (c) || isdigit (c))))) {
		ulreally = TRUE;			/* underscore italics for now */
		if (phpos != ulmap[nuls].uloff)
			ulmap[++nuls].ulon = phpos;
		}
	else ulreally = FALSE;
	if (c >= ' '  &&  c <= '~') {
		if (c != ' ') {
			outline[olen].ochar = c;
			outline[olen].ohpos = phpos;
			outline[olen].obold = boldp;
			outline[olen].oital = italicp && !ulitalp;
			++olen;
			}
		phpos += PWidChar (c);
		}
	else if (c & 0x80) {
		PPrtChar ('~');		/* not supporting full meta convention */
		PPrtChar (c & 0x7F);
		}
	else {
		PPrtChar ('^');
		PPrtChar (c ^ 0x40);
		}
	if (ulreally) ulmap[nuls].uloff = phpos;
	PSendChk();
	}


PWidChar (c)				/* width of c when printed */
	char c;
{
	if (c >= ' '  &&  c <= '~')
		return (DWidChar (c));
	else if (c & 0x80) return (1 + PWidChar (c & 0x7F));
	else return (1 + PWidChar (c ^ 0x40));
	}


PPrtStr (s)				/* print a NUL-term string */
	char *s;
{
	while (*s) PPrtChar (*s++);
	}


PWidStr (s)				/* width of string */
	char *s;
{
	int wid;
	
	for (wid = 0; *s; ++s) wid += PWidChar (*s);
	return (wid);
	}


PWidSent()				/* width of end-of-sentence */
{
	return (DWidChar (DEL));
	}


PMoveH (dh)				/* create horizontal whitespace */
	int dh;
{
	if (ulonp == UNALLON  &&  ulmap[nuls].uloff != phpos)
		ulmap[++nuls].ulon = phpos;
	phpos += dh;
	if (ulonp == UNALLON) ulmap[nuls].uloff = phpos;
	}


PMoveV (dv)				/* vertical move */
	int dv;
{
	PFlush();
	pvpos += dv;
	}


PForce()					/* actually get the carriage there */
{
	PFlush();
	DSetH (phpos);
	DSetV (pvpos);
	pvpos %= pagev;
	DForce();
	}


int
PUnMica (mica, direc)		/* convert mica to internal units */
	unsigned mica, direc;
{
	unsigned tstd, temp;
	
	if (dev.micahoriz == 21  &&  direc == HORIZONTAL) {
		temp = mica - (mica >> 2);		/* 1/120 in */
		temp += temp >> 7;				/* mica * 6 / 127 */
		temp = (temp + (temp & 8)) >> 4;
		return (temp);
		}
	if (dev.micavert == 53  &&  direc == VERTICAL) {
		temp = mica + (mica >> 2)		/* 1/48 in */
				  - (mica >> 5)		/* mica * 12 / 635 */
				  - (mica >> 7)
				  - (mica >> 9);
		temp = (temp + (temp & 32)) >> 6;
		return (temp);
		}
	if (dev.micavert == 35  &&  direc == VERTICAL) {
		temp = mica - (mica >> 4)		/* 1/72 in. (Epson) */
				  - (mica >> 5)
				  + (mica >> 10);
		temp = (temp + (temp & 16)) >> 5;
		return (temp);
		}
	if (direc == HORIZONTAL) tstd = dev.micahoriz;
	else if (direc == VERTICAL) tstd = dev.micavert;
	else Fatal ("PUnMica: bad direc?");
	if (mica > 32767) temp = mica / (tstd >> 1);
	else temp = (mica << 1) / tstd;
	return ((temp >> 1) + (temp & 1));
	}


PSetAttr (attr, state)		/* turn on or off a character attribute */
	char attr;
	FLAG state;
{
	switch (attr) {
	
	case UNDERSCORE:
		if (DrvInfo (OVP)) ulonp = state;
		else PWarnOnce();
		break;
	case BOLD:
		if (dev.boldon.len  ||  DrvInfo (OVP)) boldp = state;
		else PWarnOnce();
		break;
	case ITALIC:
		if (dev.italon.len  ||  DrvInfo (OVP)) italicp = state;
		else PWarnOnce();
		break;
	default:
		Fatal ("PSetAttr: unknown attribute\n");
		break;
		}
	}


PCR()					/* do carriage return */
{
	PFlush();
	phpos = 0;
	}


PLF()					/* do a line feed */
{
	PMoveV (linev);
	}


PFF()					/* do a form feed */
{
	PMoveV (pagev - (pvpos % pagev));
	}


PNewPage()				/* FF if not at TOF */
{
	if (pvpos % pagev) PFF();
	}


PHPos()					/* where are we? */
{
	return (phpos);
	}


PVPos()					/* where are we? */
{
	return (pvpos % pagev);
	}


PCharH()					/* width of canonical char */
{
	return (charh);
	}


PLineV()					/* height of line */
{
	return (linev);
	}


PPageH()					/* width of page */
{
	return (pageh);
	}


PPageV()					/* height of page */
{
	return (pagev);
	}


PSendChk()				/* maybe send output character */
{
	if (OStKnown()) OSendChk();
	}


/******** Internal calls ********/


PWarnOnce()				/* warn user about losing device */
{
	if (warned) return;
	puts ("This device, as currently defined, cannot overprint.\n");
	warned = TRUE;
	}


PFlush()					/* output line, with underline and bold */
{
	int iul, i, starti, uswidth, direc;
	
	if (!olen) return;
	DSetV (pvpos);
	pvpos %= pagev;
	direc = PPickDirec (outline[0].ohpos, outline[olen-1].ohpos);
	starti = (direc == FORWARD) ? 0 : olen - 1;
	for (i = starti; i >= 0  &&  i < olen; i += direc) {
		DSetH (outline[i].ohpos);
		DSetAttr (BOLD, outline[i].obold);
		DSetAttr (ITALIC, outline[i].oital);
		DPrtChar (outline[i].ochar);
		if (outline[i].obold  &&  !DrvInfo (BOLD)  &&  DrvInfo (CENTCHAR)) {
			DMoveH (direc);		/* bold with small offset */
			DPrtChar (outline[i].ochar);
			}
		PSendChk();
		}
	if (!DrvInfo (BOLD)  &&  !DrvInfo (CENTCHAR)) PReBold();
	PUnderScr();
	olen = nuls = 0;
	}


int
PPickDirec (leftend, rightend)	/* pick optimal direction */
	int leftend, rightend;
{
	if (DrvInfo (BIDIREC)
	    &&  abs (DHPos() - rightend) < abs (DHPos() - leftend))
			return (BACKWARD);				/* nb: that's -1 */
	else return (FORWARD);				/* and that's +1 */
	}


PReBold()					/* bold via reprint */
{
	int bstart, bend, starti, i, direc;
	
	for (bstart = 0; bstart < olen; ++bstart)
		if (outline[bstart].obold) break;
	if (bstart >= olen) return;
	for (bend = olen; !outline[bend-1].obold; --bend);
	direc = PPickDirec (outline[bstart].ohpos, outline[bend-1].ohpos);
	starti = (direc == FORWARD) ? bstart : bend - 1;
	for (i = starti; i >= bstart  &&  i < bend; i += direc)
		if (outline[i].obold) {
			DSetH (outline[i].ohpos);
			DPrtChar (outline[i].ochar);
			PSendChk();
			}
	}


PUnderScr()				/* do underscores for the current line */
{
	int i, starti, uswidth, ulstart, ulend, ulpos, direc;
	
	direc = PPickDirec (ulmap[1].ulon, ulmap[nuls].uloff);
	starti = (direc == FORWARD) ? 1 : nuls;
	uswidth = DWidChar ('_');
	for (i = starti; i >= 1  &&  i <= nuls; i += direc) {
		ulstart = ulmap[i].ulon;
		ulend = ulmap[i].uloff - uswidth;
		ulpos = (direc == FORWARD) ? ulstart : ulend;
		if (ulstart > ulend) ulpos = (ulstart + ulend) / 2;
		repeat {
			DSetH (ulpos);
			DPrtChar ('_');
			if (direc == FORWARD) {
				if (ulpos >= ulend) break;
				ulpos += min (uswidth, ulend - ulpos);
				}
			else {
				if (ulpos <= ulstart) break;
				ulpos -= min (uswidth, ulpos - ulstart);
				}
			PSendChk();
			}
		}
	nuls = 0;
	}



/**************** Generalized Device driver code ****************/

DInit (device, portname, hackp)
	char *device, *portname;
	FLAG hackp;
{
	int i;
	
	if (((datfd = open ("config.dat", INPUT)) < 0
	    && (datfd = open ("a:config.dat", INPUT)) < 0)
	    ||  read (datfd, &tsect, 1) < 1)
		Fatal ("Can't open the system database file (CONFIG.DAT)\n");
	movmem (&tsect, &hdr, sizeof(hdr));
	if (device == DEFAULT) DfltPrt();
	else if (!FindRec (PRINT, device)) {
		printf ("Can't find device '%s'; using default.\n", device);
		DfltPrt();
		}
	movmem (&tsect, &dev, sizeof (dev));
	if (dev.isprop)   sizeof(attrstack[0]));
		++attrsp;
		}
	return (&attrstack[attrsp]);
	}


AttrPop()
{
	if (attrsp <= 0) Fatal ("Attr		movmem (&tsect, &widtab[64], 128);
		for (i = 0; i < 128; ++i)
			widtab[i] = PUnMica (widtab[i], HORIZONTAL);
		}
	if (dev.needtran) {
		ReadRec (hdr.ftran + dev.trannum);
		movmem (&tsect, &trantbl, 128);
		}
	OInit (portname);
	close (datfd);
	charh = PUnMica (dev.stdhoriz, HORIZONTAL);
	linev = PUnMica (dev.stdvert, VERTICAL);
	pageh = PUnMica (dev.dwidth, HORIZONTAL);
	pagev = PUnMica (dev.dheight, VERTICAL);
	spaceh = DWidChar (' ');
	if (hackp) dvpos = PUnMica (dev.paperoffset, VERTICAL);
	else dvpos = 0;
	dhpos = 0;
	DrvInit (dvpos);
	dbufchar = NOCHAR;
	}


DFini()
{
	DForce();
	DrvFini();
	OFini();
	}
	

DSetH (hpos)				/* move to hpos (absolute) */
	int hpos;
{
	dhpos = hpos;
	}


DSetV (vpos)				/* move to vpos (absolute) */
	int vpos;
{
	DMoveV (vpos - (dvpos % pagev));
	}


DMoveH (delh)				/* move by delh (relative) */
	int delh;
{
	dhpos += delh;
	}


DMoveV (delv)				/* move by delv (relative) */
	int delv;
{
	if (delv < 0) Fatal ("DMoveV: can't move up!\n");
	dvpos += delv;
	}


int
DHPos()					/* return current horizontal location */
{
	return (dhpos);
	}


int
DVPos()					/* return current vertical location */
{
	return (dvpos % pagev);
	}


DPrtChar (c)				/* print a character */
	char c;
{
	int offset;
	
	offset = DrvInfo (CENTCHAR) ? DWidChar (c) / 2 : 0;
	DMoveH (offset);
	DFlush();
	DMoveH (-offset);
	dbufchar = dev.needtran ? trantbl[c] : c;
	}


int
DWidChar (c)				/* width of c when printed */
	char c;
{
	return (dev.isprop ? widtab[c] : charh);
	}


DSetAttr (attr, val)		/* set innate attribute if possible */
	int attr;
	FLAG val;
{
	switch (attr) {
	
	case BOLD:
		if (dbold != val  &&  dev.boldon.len) {
			DFlush();
			OPutCmd (val ? &dev.boldon : &dev.boldoff);
			}
		dbold = val;
		break;
	case ITALIC:
		if (dital != val  &&  dev.italon.len) {
			DFlush();
			OPutCmd (val ? &dev.italon : &dev.italoff);
			}
		dital = val;
		break;
		}
	}


DForce()
{
	DFlush();
	OFlush();
	}


DFlush()					/* flush buffer */
{
	DrvPrtMove (dbufchar, dhpos, dvpos);
	dvpos %= pagev;
	dbufchar = NOCHAR;
	}


/**************** Output queue maintainer
				 and port drivers ****************/

OInit (portname)
	char *portname;
{
	char tport[20];
	
	if (portname != DEFAULT) {
		if (!FindRec (PORT, portname)) {
			sprintf (&tport, "%s Out", portname);
			if (!FindRec (PORT, &tport)) 
				Fatal ("Can't find port '%s' or '%s'.", portname, &tport);
			}
		}
	else if (hdr.prtoutp) ReadRec (hdr.prtoutp);
	else Fatal ("No default output port defined!\n");
	movmem (&tsect, &oport, sizeof (oport));
	if (dev.synctype != NOSYNC) {
		if (portname != DEFAULT) {
			sprintf (&tport, "%s In", portname);
			if (!FindRec (PORT, &tport))
				Fatal ("Can't find port '%s', required for synchronization.", &tport);
			}
		else if (hdr.prtinp) ReadRec (hdr.prtinp);
		else Fatal ("No default input port defined (one is required for synchronization).\n");
		movmem (&tsect, &iport, sizeof (iport));
		}
	IQInit (&oqueue, QUEUE_SIZE);
	syncok = TRUE;
	synccnt = 0;
	syncnbufs = 0;
	}


OFini()
{
	OFlush();
	}


OFlush()					/* flush output buffer */
{
	while (!IQEmpty (&oqueue)) OSendChk();
	}


OPutChar (c)				/* output a character */
	char c;
{
	while (IQFull (&oqueue)) OSendChk();
	OSendChk();					/* at least once */
	IQShove (c, &oqueue);
	}


OPutNum (n)				/* send a number in ASCII */
	int n;
{
	if (n >= 10) OPutNum (n / 10);
	OPutChar (n % 10 + '0');
	}


OPutCmd (cmd)				/* output a control string */
	struct str *cmd;
{
	char *chr, *endchr;
	int i;

	chr = &dev.strspc[cmd->idx];
	endchr = &dev.strspc[cmd->idx + cmd->len];
	OUnInter (cmd->len);
	while (chr < endchr) OPutChar (*chr++);
	}


OPutSeq (list)				/* send an uninterruptible sequence */
	int list;
{
	int *argptr, nargs;
	
	argptr = &list;
	for (nargs = 0; *argptr++ != ENDLIST; ++nargs);
	OUnInter (nargs);
	argptr = &list;
	while (nargs--) OPutChar (*argptr++);
	}

OUnInter (seqlen)			/* guarantee nchars uninterrupted */
	int seqlen;
{
	if (seqlen < 0  ||  seqlen >= 32)
		Fatal ("OUnInter: %d chars is too many!\n", seqlen);
	while (IQFull (&oqueue)) OSendChk();
	IQShove (SEQSTART + seqlen, &oqueue);
	}


OSendChk()				/* send a command if ready */
{
	while (!IQEmpty (&oqueue)  &&  OReady (IQPeek (&oqueue))) {
		OSend (IQGrab (&oqueue));
		if (!OStKnown()) break;
		}					/* break if we don't really know status */
	}


FLAG
OStKnown()				/* is output status known? */
{
	return (!(oport.sbiosp  &&  !oport.statport));
	}


FLAG
OReady (c)				/* ready for this character? */
	int c;
{
	int seqlen;
	
	if (c >= SEQSTART  &&  c < SEQEND) seqlen = c - SEQSTART;
	else seqlen = 1;
	switch (dev.synctype) {
	
	case NOSYNC:
		return (PORdy());
	case XON_XOFF:
		if (PIRdy()) syncok = (PIGet() != DC3);
		return (syncok  &&  PORdy());
	case ETX_ACK:
		if (synccnt + seqlen > SYNCBUFSIZ) {
			if (!PORdy()) return (FALSE);
			POPut (ETX);
			++syncnbufs;
			synccnt = 0;
			}
		if (PIRdy()  &&  PIGet() == ACK  &&  syncnbufs > 0) --syncnbufs;
		return (syncnbufs < 2  &&  PORdy());
	default:
		Fatal ("Invalid synchronization code: '%d'", dev.synctype);
		break;
		}
	}


OSend (c)				/* send a character */
	int c;
{
	if (c >= SEQSTART  &&  c < SEQEND) return;	/* ignore seq codes */
	POPut (c);
	++synccnt;
	}


FLAG
PIRdy()					/* char available from printer input? */
{
	if (iport.sbiosp  &&  !iport.statport) return (TRUE);
	return (((iport.sbiosp ? bios (iport.statport) : inp (iport.statport))
		    & iport.readymask) == iport.polarity);
	}


char
PIGet()					/* get char from printer */
{
	return ((iport.dbiosp ? bios (iport.dataport) : inp (iport.dataport))
		   & iport.datamask);
	}


FLAG
PORdy()					/* printer ready for character */
{
	if (oport.sbiosp  &&  !oport.statport) return (TRUE);
	return (((oport.sbiosp ? bios (oport.statport) : inp (oport.statport))
		    & oport.readymask) == oport.polarity);
	}


POPut (c)				/* send char to printer */
	char c;
{
	c &= oport.datamask;
	if (oport.dbiosp) bios (oport.dataport, c);
	else outp (oport.dataport, c);
	}



/******** Utility routines for reading database ********/


FLAG
FindRec (type,name)			/* Locate a record by class and name */
	int type;				/* returns success flag */
	char *name;
{
	int frec, lrec;

	switch (type) {

	case PORT:
		frec = hdr.fport;
		lrec = hdr.fterm;
		break;
	case PRINT:
		frec = hdr.fprint;
		lrec = hdr.fmicro;
		break;
	default:
		Fatal ("FindRec: Invalid record type.");
		return (FALSE);
		}
	for (frec=0; frec < lrec; ++frec) {
		ReadRec (frec);
		if (match (name, &tsect)) return (TRUE);
		}
	return (FALSE);
	}


ReadRec (rec)				/* read a CONFIG.DAT record */
	int rec;
{
	if (seek (datfd, rec, ABSOLUTE) < 0
	    ||  read (datfd, &tsect, 1) < 1)
		Fatal ("Read error in system database file (CONFIG.DAT)\n");
	}


DfltPrt()					/* read definition of default printer */
{
	if (hdr.ourprint == 0) Fatal ("No default printer defined!\n");
	ReadRec (hdr.ourprint);
	if (!tsect[0]) Fatal ("Invalid default printer definition (record deleted).\n");
	}



/* End of PRINTER.C  --  vanilla printer driver */

