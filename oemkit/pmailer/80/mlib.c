#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "htab.inc"
#include "mail.inc"
#include "db.inc"
#include "mform.inc"
#define HELPFILE "mail.hlp"




			/* Get the list target options from the user */
gettarg(mform,target)
 struct mailformat *mform;
 char *target;
 {char opt,filename[MAXFNSZ],togbuf[SYSBUFSZ];
  *target = CRTMASK;
  while(1)
    {if (togmenu("Mail Target Selection","CRT,Printer,File",
	&opt,*target,togbuf,1,HELPFILE) == ESC) return;
     switch(opt)
       {case 0:
 	  toggle(target,CRTMASK);
	  break;
        case 1:
 	  toggle(target,HDCPYMASK);
	  break;
	case 2:
	  if (*target & FILEMASK)
	     {toggle(target,FILEMASK);
	      break;
	      }
	  if (gettok("output file name",filename,MAXFNSZ,2,HELPFILE) == ESC)
	    break;
          if (menu("Do You Want to Start A New File or Add to Existing File?",
		"Create New File,Append to Existing File",&opt,
		3,HELPFILE) == ESC) break;
	  if (fopen0(filename,opt == 0 ? FWRITE : FAPPEND) < 0) break;
	  toggle(target,FILEMASK);
	  break;
        }
     }
  }



mail(mform,htab,sslist,target,hitlist)
 struct mailformat *mform;
 char target;
 struct hashtable *htab;
 struct ssdirectory *sslist;
 int hitlist[HITBUFSZ];
 {char letbuf[LETBUFSZ],*datlst[MAXNLFLDS],filename[MAXFNSZ];
  char extropts[SYSBUFSZ],extropt,mem[MEMBUFSZ];
  int nhits;
  if (gettok("input file name",filename,MAXFNSZ,4,HELPFILE) == ESC) return;
  if (rdfil(filename,letbuf,LETBUFSZ) < 0) return;
  gettarg(mform,&target);
  strcpy(extropts,EXTROPTS);
  if (hitlist[0]) strcat(extropts,",Recipients of Last Run");
  if (menu("How Do You Wish to Specify Recipients?",extropts,&extropt,
	5,HELPFILE) == ESC) return;
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  if ((target & HDCPYMASK) && mform->contopt == CONTINUOUS) 
     err0("Load Forms in Printer");
  NACROSSCT = 0;
  switch(extropt)
    {case INDIVIDUAL:
       sendind(letbuf,mform,target,htab,hitlist);
       break; 
     case SUBSET: 
       sendss(letbuf,mform,sslist,target,hitlist);
       break; 
     case SORTIMAGE:
       sendimg(letbuf,mform,target,hitlist);
       break;
     case PREVIOUSRUN:
       sendhits(letbuf,mform,target,hitlist);
       break;
     }
  settarg(target);
  flushlab(mform);
  if (target & FILEMASK) fclose();  
  initio();
  } 






	    	     
			/* Send letters to individual members by Member ID */
sendind(letbuf,mform,target,htab,hitlist) 
 char *letbuf,target,hitlist[HITBUFSZ];
 struct mailformat *mform;
 struct hashtable *htab;
 {int n,entryno,nhits,sect0;
  char memid0[MEMIDSZ],c,mem[MEMBUFSZ],*datlst[MAXNLFLDS];
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  for(nhits=0;;)
    {if (getmemid(memid0,htab,6,HELPFILE) == ESC) return;
     for(n=0;;n++)
       {if ((entryno = fndentry(memid0,htab,n)) < 0) break;
        rdentry(entryno,htab);
        if ((sect0 = entry(entryno,htab)->sector0) < 0) break;
        rdmbr0(datlst,sect0,SYS.nlflds,DBFD);
        dispfrm();
        fill0(datlst);
        if ((c = getindres(7,HELPFILE,datlst)) == ESC) break; 
	if (c == 'y')
	  {if (prlet(letbuf,datlst,mform,target) == ESC) return;
	   addhit(hitlist,nhits++,sect0);
  	   break;
	   }
        }
     }
  }
   


   
			/* See if the user wants to send to this member */
getindres(helpid,helpfile,datlst)
 char helpid,*helpfile,*datlst[MAXNLFLDS];
 {char c;
  while(1)
    switch(c = tolower(getchar()))
      {case ESC:
       case 'n':
       case 'y':
	 setmode(CLEAR);
	 return(c);  
       case CTLQ:
	  help(helpid,helpfile);
	  dispfrm();
	  fill0(datlst);
	  break;
       }
  }






     


	
				/* Add a hit to the hitlist */
addhit(hitlist,nhits,sect0)
 int hitlist[HITBUFSZ],nhits,sect0;
 {if (nhits < HITBUFSZ-1) 
    {hitlist[nhits] = sect0;
     hitlist[nhits+1] = 0;
     }
  else			  hitlist[0] = 0;
  }









			/* Send letters to the members in a sort image file */
sendimg(letbuf,mform,target,hitlist) 
 char letbuf[LETBUFSZ],target;
 struct mailformat *mform;
 int hitlist[HITBUFSZ];
 {int n,imagefd; 
  char filename[MAXFNSZ];
  if (gettok("name of file containing sort image",filename,  
     MAXFNSZ,8,HELPFILE) == ESC) return;
  if ((imagefd = open(filename,0)) < 0) 
    return(err("Cannot open file %s",filename));
  for(n=0;;n++)
    {rddac(imagefd,n,hitlist,HITBUFSZ*2);
     if (sendhits(letbuf,mform,target,hitlist) < HITBUFSZ) return;
     }
  if (target == CRTMASK) err0("");
  }



			/* Send to a hitlist */
sendhits(letbuf,mform,target,hitlist)
 char letbuf[LETBUFSZ],target;
 struct mailformat *mform;
 int hitlist[HITBUFSZ];
 {int i;
  char *datlst[MAXNLFLDS],mem[MEMBUFSZ];
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  for(i=0;i<HITBUFSZ;i++)
    {if (hitlist[i] == 0) return(i);
      rdmbr0(datlst,hitlist[i],SYS.nlflds,DBFD);
     if (prlet(letbuf,datlst,mform,target) == ESC) return(i);
     }
  return(HITBUFSZ);
  }
 








/*
		--------------------------
		| Keyword Initialization |
		--------------------------
*/


initkey()
  {strcpy(KEYWORDS[0].keytag,"format");
   KEYWORDS[0].keychar = FORMAT;
   strcpy(KEYWORDS[1].keytag,"unformat");
   KEYWORDS[1].keychar = UNFORMAT;
   strcpy(KEYWORDS[2].keytag,"paragraph");
   KEYWORDS[2].keychar = PARAGRAPH;
   strcpy(KEYWORDS[3].keytag,"newline");
   KEYWORDS[3].keychar = NEWLINE;
   strcpy(KEYWORDS[4].keytag,"under");
   KEYWORDS[4].keychar = UNDER;  
   strcpy(KEYWORDS[5].keytag,"nounder");
   KEYWORDS[5].keychar = NOUNDER;
   }





