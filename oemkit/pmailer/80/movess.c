#include "constant.inc"
#include "sys.inc"
#include "subset.inc"
#include "db.inc"

ssfetch(dir)
 char dir[MAXFNSZ];
 {struct ssdirectory sslist,sslist0;
  int i,k,matchlst[MAXNLFLDS];
  struct logfield field0[MAXNLFLDS];
  struct sysvars sys0;
  struct subset sub,sub0;
  char tagbuf0[TAGBUFSZ],filbuf[MAXDESCSZ],desc[MAXDESCSZ];
  char *datlst[NSSMEM][MAXNLFLDS],*datlst0[NSSMEM][MAXNLFLDS];
  if (openss(&sslist,DBDIR,DEFINESUBSETS) < 0) exit();
  if (openss(&sslist0,dir,DEFINESUBSETS) < 0) exit();
  if (fopen0(mkpath(dir,DEFFILE,filbuf),FREAD) < 0) return(-1);
  rdsys(&sys0);
  rdfld(field0,sys0.nlflds);
  rdtag(field0,tagbuf0,sys0.nlflds);
  fclose();
  getmatch(FIELD,field0,SYS.nlflds,sys0.nlflds,matchlst);
  for(k=0;k<NSSMEM;k++)
    {lnkdat(sub.ssmem[k],datlst[k],FIELD,SYS.nlflds);
     lnkdat(sub0.ssmem[k],datlst0[k],field0,sys0.nlflds);
     }
  for(i=0;i<sslist0.nsubsets;i++)
    {printf("\nSubset %s: ",extropt(i,sslist0.ssdesclist,desc));
     addopt(desc,sslist.ssdesclist);
     rdss(&sub0,i,&sslist0,field0,sys0.nlflds);
     for(k=0;k<NSSMEM;k++)
        {zero(datlst[k]);
         datcpy(datlst[k],datlst0[k],matchlst,sys0.nlflds);
	 relcpy(sub.relop[k],sub0.relop[k],matchlst,sys0.nlflds);
         }
     wrtss(&sub,NEWENTRY,&sslist,FIELD,SYS.nlflds);
     }
  closess(&sslist);
  fabort(sslist0.ssfd);
  return(0);
  }



relcpy(relop,relop0,matchlst,nlflds0)
 char relop[MAXNLFLDS],relop0[MAXNLFLDS];
 int matchlst[MAXNLFLDS],nlflds0;
 {int i;
  setmem(relop,MAXNLFLDS,0);
  for(i=0;i<nlflds0;i++)     
    if (matchlst[i] >= 0) relop[matchlst[i]] = relop0[i];
  }






