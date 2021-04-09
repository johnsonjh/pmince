#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "subset.inc"
#include "term.inc"
#include "db.inc"
#define HELPFILE "movedb.hlp"

main(argc,argv)
 int argc;
 char **argv;
 {struct hashtable htab;
  struct terminal term;
  char dir0[MAXFNSZ],opt;
  TERM = &term;
  initio();
  printf("Movedb v1.1\nCopyright (c) 1982 by Perfect Software, Inc.");
  *SYS.helpdir = 'x';
  if (gettok("disk drive containing target data base",
	DBDIR,MAXDIRSZ,0,HELPFILE) == ESC) exit();
  initmove(&htab);
  if (gettok("drive containing source database",dir0,
	MAXDIRSZ,1,HELPFILE) == ESC) exit();
  while(1)
    {if (menu("What Do You Want to Copy from the Old Data Base?",
	"Members,Subsets,List Formats,All of the Above",&opt,1,
	HELPFILE) == ESC) exit();
     switch(opt)
       {case 0:
	  dbfetch(dir0,&htab);
	  break;
	case 1:
 	  ssfetch(dir0);
 	  break;
        case 2:
	  lffetch(dir0);
	  break;
        case 3:
	  dbfetch(dir0,&htab);
 	  ssfetch(dir0);
	  lffetch(dir0);
	  break;
        }
     }
  }




dbfetch(dir0,htab)
 char dir0[MAXFNSZ];
 struct hashtable *htab;
 {int i,iss,sect0,dbfd0,nextsct0,nsect,matchlst[MAXNLFLDS];
  struct logfield field0[MAXNLFLDS];
  struct sysvars sys0;
  struct subset sub;
  struct ssdirectory sslist;
  char tagbuf0[TAGBUFSZ],filbuf[MAXDESCSZ];
  char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS],mem[MEMBUFSZ],mem0[MEMBUFSZ];
  char *ssdatlst[NSSMEM][MAXNLFLDS];
  if (fopen0(mkpath(dir0,DEFFILE,filbuf),FREAD) < 0) return(-1);
  rdsys(&sys0);
  rdfld(field0,sys0.nlflds);
  rdtag(field0,tagbuf0,sys0.nlflds);
  fclose();
  if ((DBFD = opendb(DBDIR,&NEXTSECTOR)) < 0) exit();
  openhtab(htab);
  if ((dbfd0 = opendb(dir0,&nextsct0)) < 0) exit();    
  openss(&sslist,dir0,USESUBSETS);
  if (getss("Select Subset to Copy",&sub,&iss,&sslist,field0,sys0.nlflds,
	2,HELPFILE) == ESC) return;
  getmatch(FIELD,field0,SYS.nlflds,sys0.nlflds,matchlst);
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  lnkdat(mem0,datlst0,field0,sys0.nlflds);
  for(i=0;i<NSSMEM;i++) lnkdat(sub.ssmem[i],ssdatlst[i],field0,sys0.nlflds);
  for(sect0=1;sect0<nextsct0;)
    {printf("\nSector %d: ",sect0);
     nsect = rdmbr0(datlst0,sect0,sys0.nlflds,dbfd0);
     if (nsect != DELETED)
       {zero(datlst);
	if (procchar() == ESC) break;
        datcpy(datlst,datlst0,matchlst,sys0.nlflds);
        putid(datlst);
        if (memcmp(datlst0,sub.relop,ssdatlst,field0,sys0.nlflds))
      	  {printf(" writing");   
           wrtmbr(datlst,NEWENTRY,htab);
	   }
        else printf(" not writing");
	sect0 += nsect;
        }
     else sect0 += 1;
     } 
  closehtab(htab);
  closedb();
  fabort(dbfd0);
  }

		/* See which fields match the old db */
getmatch(field,field0,nlflds,nlflds0,matchlst)
 struct logfield field[MAXNLFLDS],field0[MAXNLFLDS];
 int nlflds,nlflds0;
 int matchlst[MAXNLFLDS];
 {int i,ifld;
  setmem(matchlst,2*MAXNLFLDS,-1);
  for(i=0;i<nlflds0;i++)
    {printf("\nOld field %d: %s",i+1,field0[i].tag);
     if ((ifld = fldmatch(field0[i].tag)) >= 0)
       {printf(": match with new field %d",ifld+1);        
        matchlst[i] = ifld;
	}
     }
  }

		/* Copy the fields that match */
datcpy(datlst1,datlst2,matchlst,nlflds0)
 char *datlst1[MAXNLFLDS],*datlst2[MAXNLFLDS];
 int matchlst[MAXNLFLDS],nlflds0;
 {int i,i1; 
  for(i=0;i<nlflds0;i++)
    if ((i1 = matchlst[i]) >= 0) 
      {strcpy0(datlst1[matchlst[i]],datlst2[i],FIELD[i1].length+1);
       datlst1[i1][FIELD[i1].length] = 0;
       }
  }










		/* Return the field from which most member ID chars come */
putid(datlst)
 char *datlst[MAXNLFLDS];
 {int i,j;
  for(i=0;i<SYS.nlflds;i++)
    {for(j=0;j<MEMIDSZ;j++)
     if (SYS.memberid[j].idfld == i) 
       {printf("%s:",datlst[i]);
        break;
        }
     }
  }










initmove(htab)
 struct hashtable *htab;
 {char filbuf[MAXDESCSZ];
  BACKUPFLAG = 0;
  if (rdmem(TERMFILE,*TERM,sizeof(*TERM)) < 0) exit();
  printf("\n%s Terminal",TERM->termdesc);
  if (fopen0(mkpath(DBDIR,DEFFILE,filbuf),FREAD) < 0) exit();    
  rdsys(&SYS);
  rdfld(FIELD,SYS.nlflds);
  rdtag(FIELD,TAGBUF,SYS.nlflds);
  fclose();
  strcpy(htab->filename,HTABFILE);
  htab->nbuckets = SYS.nbucket0;
  setversion();
  }
