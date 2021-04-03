/* COMM3.C	This is part three of the mince command set

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

	This file contains the Control-X  command execution
routines for the mince editor. There are also a small number of
subordinate routines. */

#include "mince.gbl"		/* Only when seperate files */

MLstBuffs()			/* print a list of the buffers */
{
	tmp= -1;
	tlrow = -1;			/* forget any saved row */
	for (cnt=BUFFSMAX-1; cnt>=0; --cnt) if (buffs[cnt].bbuff) {
		TSetPoint(++tmp,0);
		TCLEOL();
		BSetMod(BScrnMrk(tmp));
		TPrntStr(buffs[cnt].bname);
		TSetPoint(tmp,9);
		if (BModp(buffs[cnt].bbuff)) TPrntChar('*');
		else TPrntChar(' ');
		TPrntChar(' ');
#ifdef CPM
		itot(BLength(buffs[cnt].bbuff));
#else
		ltot(BLength(buffs[cnt].bbuff));
#endif
		TSetPoint(tmp,18);
		TPrntStr(buffs[cnt].fname);
		TKbChk();
		}
	TSetPoint(++tmp,0);
	TCLEOL();
	BSetMod(BScrnMrk(tmp));
	TForce();
	KbWait();
	if (tmp>=divide) ModeLine();
	}

MExit()				/* exit this level of the editor */
{
	arg=0;
	tmp=FALSE;
	for (cnt=BUFFSMAX-1; cnt>=0; --cnt) if (buffs[cnt].bbuff)
		tmp |= BModp(buffs[cnt].bbuff);
	if (tmp && !Ask("Abandon Modified Buffer(s)? ")) return;
	abort=TRUE;
	}

MFindFile()			/* Find a file */
{
	char tfname[FILMAX], tbname[BUFNAMMAX];

	arg=0;
	tfname[0]=0;
	do if (!GetArg("File to Find <CR>: ",CR,tfname,FILMAX)) return;
		while (tfname[0]==0);
	strip(tbname,tfname);
#ifdef CPM
	UpCase(tfname);
#endif
	LowCase(tbname);
	for (cnt=BUFFSMAX-1; cnt>=0 && (buffs[cnt].bbuff==NULL ||
		strcmp(tfname,buffs[cnt].fname)); --cnt);
	TKbChk();
	if (cnt<0) {
		cnt=CFindBuff(tbname);
		if (cnt>=0) {
			if (!GetArg("Buffer Exists!  Buffer to Use <CR>: ",
				CR,tbname,BUFNAMMAX)) return;
			LowCase(tbname);
			cnt=CFindBuff(tbname);
			ClrEcho();
			}
		if (cnt<0) if ((cnt=CMakeBuff(tbname,tfname))<0) return;
		buffs[cnt].bmodes[0]=0;
		strcpy(buffs[cnt].fname,tfname);
		BSwitchTo(buffs[cnt].bbuff);
		if (!BReadFile(tfname)) Echo("New File");
		}
	CSwitchTo(cnt);
	ScrnRange();
	}

MSetTabs()			/* Set the tab increment to arg */
{
	if (argp) tabincr=arg;
	else tabincr=BGetCol();
	if (tabincr==0) tabincr=1;
	arg=0;
	NewDsp();			/* force redisplay */
	}

MDelMode()			/* remove a mode */
{
	arg=0;
	if (!(tmp=GetModeId("Delete Mode <CR>: "))) return;
	for (cnt=MAXMODES-1; cnt>=0; --cnt)
		if (tmp==buffs[cbuff].bmodes[cnt]) {
			while (cnt>0 && buffs[cbuff].bmodes[cnt]) {
				buffs[cbuff].bmodes[cnt]=buffs[cbuff].bmodes[cnt-1];
				--cnt;
				}
			buffs[cbuff].bmodes[cnt]='\0';
			SetModes();
			}
	}

MFileRead()			/* read a file into the buffer */
{
	arg=0;
	if (BModp(buffs[cbuff].bbuff) && !Ask("Clobber Modified Buffer? ")) {
		ClrEcho();
		return;
		}
	if (!GetArg("File To Read <CR>: ",CR,buffs[cbuff].fname,FILMAX))
		return;
#ifdef CPM
	UpCase(buffs[cbuff].fname);
#endif
	if (!BReadFile(buffs[cbuff].fname)) Echo("New File");
	else ClrEcho();
	ModeLine();
	}

MFileSave()			/* write the buffer to the default file */
{
	arg=0;
	Echo("Writing...");
	if (!BWriteFile(buffs[cbuff].fname)) Error("Can't open file");
	else Echo ("File Written");
	}

MNxtOthrWind()			/* scroll the other window down */
{
	if (divide>=TMaxRow()-2) return;
	MSwpWind();
	while (arg-- > 0) MNextPage();
	MSwpWind();
	}

MFileWrite()			/* write the buffer to a file */
{
	arg=0;
	if (!GetArg("File To Write <CR>: ",CR,buffs[cbuff].fname,FILMAX)) return;
#ifdef CPM
	UpCase(buffs[cbuff].fname);
#endif
	MFileSave();
	ModeLine();
	}

MSwapMrk()			/* Swap the point with the mark */
{
	BSwapPnt(mark);
	arg=0;
	}

MPrvOthrWind()			/* scroll the other window up */
{
	if (divide>=TMaxRow()-2) return;
	MSwpWind();
	while (arg-- > 0) MPrevPage();
	MSwpWind();
	}

MSetIndent()			/* set the indent column */
{
	if (argp) indentcol=arg;
	else indentcol=BGetCol();
	Echo("Indent Column is ");
	itot(indentcol);
	arg=0;
	}

#ifdef UNIX
MUnix()				/* execute a shell command */
{
	char cmnd_line[80];
	int status;
	
	arg=0;
	*cmnd_line='\0';
	if (!GetArg("Shell Command <CR>: ",CR,cmnd_line,80)) return;
	TSetPoint(TMaxRow()-1,0);
	TCLEOL();
	TForce();
	TFini();
	system(cmnd_line);
	while (wait(&status) != -1);
	printf("[Hit <CR> to return to Mince]");
	while (getchar()!='\n');
	TInit();
	NewDsp();
	}
#endif

MPrintPos()			/* print the current position */
{
#ifndef CPM
	long tmp;
#endif

	BSwapPnt(mark);
	tmp=BLocation();
	BSwapPnt(mark);
	Echo("Point ");
#ifdef CPM
	itot(BLocation());
#else
	ltot(BLocation());
#endif
	TPrntStr("   length ");
#ifdef CPM
	itot(BLength(buffs[cbuff].bbuff));
#else
	ltot(BLength(buffs[cbuff].bbuff));
#endif
	TPrntStr("   column ");
	itot(BGetCol());
	TPrntStr("   mark ");
#ifdef CPM
	itot(tmp);
#else
	ltot(tmp);
#endif
	TForce();
	KbWait();
	ClrEcho();
	}

MOneWind()			/* switch to one window mode */
{
	if (divide>=TMaxRow()-2) return;
	topp=TRUE;
	BSetMod(BScrnMrk(divide));
	divide=TMaxRow()-2;
	BKillMrk(altstart);
	BKillMrk(altpnt);
	sendp=FALSE;
	}

MTwoWind()			/* Switch to two window mode */
{
	if (divide<TMaxRow()-2) return;
	divide=(TMaxRow()-2)/2;
	ScrnRange();
	altpnt=BCreMrk();
	BSwapPnt(sstart);
	altstart=BCreMrk();
	BSwapPnt(sstart);
	altbuff=cbuff;
	ModeLine();
	}

MSwitchTo()			/* switch buffers */
{
	int i;

	arg=0;
	if (!GetArg("Switch to Buffer <CR>: ",CR,namearg,BUFNAMMAX)) return;
	LowCase(namearg);
	i=CFindBuff(namearg);
	if (i<0 && (!Ask("Create new buffer?") ||
		(i=CMakeBuff(namearg,"DELETE.ME"))<0)) return;
	CSwitchTo(i);
	ScrnRange();
	}

MSetFill()			/* set the fill width */
{
	if (argp) fillwidth=arg;
	else fillwidth=BGetCol();
	Echo("Fill Column is ");
	itot(fillwidth);
	arg=0;
	}

MKillBuff()			/* delete a buffer */
{
	int count;

	arg=0;
	if (!GetArg("Delete Buffer <CR>: ",CR,namearg,BUFNAMMAX)) return;
	LowCase(namearg);
	if ((count=CFindBuff(namearg))<0) {
		Error("Does not exist");
		return;
		}
	if (count==cbuff) MSwitchTo();
	if (count==cbuff) {
		Error("Can't delete current");
		return;
		}
	if (BModp(buffs[count].bbuff) && !Ask("Delete Modified Buffer?")) return;
	if (divide<TMaxRow()-2 && count==altbuff) MOneWind();
	BDelBuff(buffs[count].bbuff);
	buffs[count].bbuff=NULL;
	strcpy(namearg,buffs[cbuff].bname);
	}

MAddMode()				/* add a mode to the list */
{
	arg=0;
	if (!(tmp=GetModeId("Mode Name <CR>: "))) return;
	for (cnt=MAXMODES-1; cnt>=0; --cnt) {
		if (!buffs[cbuff].bmodes[cnt]) buffs[cbuff].bmodes[cnt]=tmp;
		if (tmp==buffs[cbuff].bmodes[cnt]) {
			SetModes();
			return;
			}
		}
	Error("No More Mode Room");
	}
	
MSwpWind()			/* Switch which window we are in */
{
	int count;

	if (divide>=TMaxRow()-2) return;
	tmp=altstart;			/* switch alt and cur screen start */
	altstart=sstart;
	sstart=tmp;
	tmp=altpnt;			/* switch alt and cur point */
	altpnt=BCreMrk();
	count=cbuff;			/* switch buffers */
	if (altbuff!=cbuff) CSwitchTo(altbuff);
	altbuff=count;
	sendp=FALSE;			/* fix up sstart, psstart, and send */
	BPntToMrk(sstart);
	BMove(-1);
	BMrkToPnt(psstart);
	BPntToMrk(tmp);		/* set new point */
	BKillMrk(tmp);
	topp = !topp;			/* switch top for bottom */
	}

MGrowWind()			/* grow the current window */
{
	if (divide>=TMaxRow()-2) return;
	BSetMod(BScrnMrk(divide));
	divide += topp? arg : -arg;
	if (divide>TMaxRow()-6) divide=TMaxRow()-6;
	if (divide<3) divide=3;
	ModeLine();
	arg=0;
	}

	/* page mode commands */
MTrimWhite()
{
	tmark=BCreMrk();
	BToStart();
	while (!BIsEnd()) {
		ToEndLine();
		MDelWhite();
		BMove(1);
		}
	BPntToMrk(tmark);
	BKillMrk(tmark);
	}

/* END OF COMM3.C - Mince command routines */

