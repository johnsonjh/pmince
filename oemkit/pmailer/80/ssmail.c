#include "constant.inc"
#include "sys.inc"
#include "subset.inc"
#include "db.inc"
#include "mail.inc"
#include "mform.inc"
#define HELPFILE "mail.hlp"


				/* Send a letter to a subset */
sendss(letbuf,mform,sslist,target,hitlist) 
 char letbuf[LETBUFSZ],target;
 struct mailformat *mform;
 struct ssdirectory *sslist;
 int hitlist[HITBUFSZ];
 {struct subset sub;
  char *datlst[MAXNLFLDS],mem[MEMBUFSZ],*datlst0[NSSMEM][MAXNLFLDS];
  int i,iss,nsect,sect0,nhits;
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  for(i=0;i<NSSMEM;i++) lnkdat(sub.ssmem[i],datlst0[i],FIELD,SYS.nlflds);
  if (getss("Select Subset of Recipients",&sub,&iss,sslist,FIELD,SYS.nlflds,
	9,HELPFILE) == ESC)
    return;
  nhits = 0;
  for(sect0=1;sect0<NEXTSECTOR;)
    {if ((nsect = rdmbr0(datlst,sect0,SYS.nlflds,DBFD)) == DELETED)
       sect0 += 1;
     else
       {if (memcmp(datlst,sub.relop,datlst0,FIELD,SYS.nlflds))
	  {if (prlet(letbuf,datlst,mform,target) == ESC) return;
	   addhit(hitlist,nhits++,sect0);
	   }
        sect0 += nsect;
        }
     }
  }
