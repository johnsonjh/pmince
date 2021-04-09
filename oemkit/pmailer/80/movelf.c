#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "listform.inc"
#include "db.inc"

lffetch(dir)
 char dir[MAXFNSZ];
 {int i,matchlst[MAXNLFLDS];
  struct logfield field0[MAXNLFLDS];
  struct sysvars sys0;
  struct formatdir formdir,formdir0;
  struct listformat lf,lf0;
  char tagbuf0[TAGBUFSZ],filbuf[MAXDESCSZ],descbuf[LFRMDSCBUFSZ];
  char descbuf0[LFRMDSCBUFSZ],desc[MAXDESCSZ];
  char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS],mem[MEMBUFSZ],mem0[MEMBUFSZ];
  openform(&formdir,DBDIR,LFRMDSCFILE,LISTFRMFILE,descbuf,sizeof(lf));
  openform(&formdir0,dir,LFRMDSCFILE,LISTFRMFILE,descbuf0,sizeof(lf));
  if (fopen0(mkpath(dir,DEFFILE,filbuf),FREAD) < 0) return(-1);
  rdsys(&sys0);
  rdfld(field0,sys0.nlflds);
  rdtag(field0,tagbuf0,sys0.nlflds);
  fclose();
  getmatch(FIELD,field0,SYS.nlflds,sys0.nlflds,matchlst);
  for(i=0;i<optct(formdir0.desclist);i++)
    {printf("\nCopying %s ",extropt(i,formdir0.desclist,desc));
     rdformat(&lf0,i,&formdir0);
     if (lfxfer(&lf,&lf0,matchlst) >= 0)
       {wrtformat(&lf,optct(formdir.desclist),&formdir);
        addopt(desc,formdir.desclist);
	}
     } 
  closeform(&formdir);
  fabort(formdir0.formfd);
  }


lfxfer(lf1,lf2,matchlst)
 struct listformat *lf1,*lf2;
 int matchlst[MAXNLFLDS];
 {int i;
  strcpy0(lf1,lf2,sizeof(*lf1));
  if (lfblkxfer(lf1->listhead.headfield,lf2->listhead.headfield,
	lf2->listhead.nheadfields,matchlst) < 0) return(-1);
  if (lfblkxfer(lf1->page.pagehead.headfield,lf2->page.pagehead.headfield,
	lf2->page.pagehead.nheadfields,matchlst) < 0) return(-1);
  if (lfblkxfer(lf1->memform.memfield,lf2->memform.memfield,
	lf2->memform.nmemfields,matchlst) < 0) return(-1);
  for(i=0;i<lf2->keys.nkeys;i++)
    {if (lfblkxfer(lf1->keys.key[i].subhead,lf2->keys.key[i].subhead,
	lf2->keys.key[i].nsubhdflds,matchlst) < 0) return(-1);
     if ((lf1->keys.key[i].fldno = matchlst[lf2->keys.key[i].fldno]) < 0) 
	return(err("Old field %d missing in new data base",
	   lf2->keys.key[i].fldno + 1)); 
     }
  }


lfblkxfer(lfblk1,lfblk2,nfields,matchlst)
 struct lffield *lfblk1,*lfblk2;
 int nfields,matchlst[MAXNLFLDS];
 {int i;
  for(i=0;i<nfields;i++)
    if (lfblk2[i].type == LOGFIELD || lfblk2[i].type == TEMPLATE)
     if ((lfblk1[i].fldid = matchlst[lfblk2[i].fldid]) < 0) 
	return(err("Old field %d missing in new data base",
           lfblk2[i].fldid + 1)); 
  return(0);
  }


