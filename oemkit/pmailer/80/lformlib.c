#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "subset.inc"
#include "listform.inc"
#include "db.inc"


				/* Get a format string from the user */
getfrmstr(mesg,frmstr,strmax,chngflg,helpid,helpfile)
 char *mesg,*frmstr,*strmax,*chngflg,helpid,*helpfile;
 {char c,strbuf[MAXDESCSZ];
  int i;
  frmstr[0] = 0;
  frmstr[1] = EOB;
  prfrmstr(mesg,frmstr);        
  for(i=0;;)
    switch(c =getchar())
      {case ESC:
	 return;
       case CTLQ:
	 help(helpid,helpfile);
       case CTLX:
	 prfrmstr(mesg,frmstr);        
	 break;
       case BACK:
	 if (i > 0) frmstr[--i] = 0;
	 prfrmstr(mesg,frmstr);        
	 break;
       case CR:
	 c = '\n';
       default:
	 if (c != '\n' && c < ' ')
	   {putch(BELL);
	    break;
	    }
	 if (i+2 >= strmax-frmstr) 	
	   {err0("No more room in format string buffer [strbuf]");
	    break;
	    }
	 putch(c);
         frmstr[i++] = c;
	 frmstr[i] = 0;
	 frmstr[i+1] = EOB;
	 *chngflg = 1;
	 break;
       }
   }


				/* Compress format string buffer */
compress(lf)
 struct listformat *lf;
 {char strbuf0[STRBUFSZ];
  int i,j;
  strbuf0[0] = EOB;
  for(i=0;i<lf->listhead.nheadfields;i++)
    addbuf(&lf->listhead.headfield[i],lf->strbuf,strbuf0);
  for(i=0;i<lf->page.pagehead.nheadfields;i++)
    addbuf(&lf->page.pagehead.headfield[i],lf->strbuf,strbuf0);
  for(i=0;i<lf->memform.nmemfields;i++)
    addbuf(&lf->memform.memfield[i],lf->strbuf,strbuf0);
  for(i=0;i<lf->keys.nkeys;i++)
    for(j=0;j<lf->keys.key[i].nsubhdflds;j++)
      addbuf(&lf->keys.key[i].subhead[j],lf->strbuf,strbuf0);
  strcpy0(lf->strbuf,strbuf0,STRBUFSZ);
  }
      

		  	/* If format field lfld is a string field, then
			     add string to strbuf0 and change pointer
			     in lfld */
addbuf(lfld,strbuf,strbuf0)
 struct lffield *lfld;
 char *strbuf,*strbuf0;
 {int i;
  char *str;
  if (lfld->type != STRING) return;
  str = &strbuf[lfld->fldid];
  lfld->fldid = buflen(strbuf0);
  for(i=buflen(strbuf0);strbuf0[i++] = *str++;);
  strbuf0[i] = EOB;
  }




				/* Print Format String with message */
prfrmstr(mesg,frmstr)
 char *mesg,*frmstr;
 {setmode(CLEAR);
  printf("%s\nEnter string [Escape when finished]:\n",mesg);
  dispfrmstr(frmstr);
  }




				/* Display a Format String */
dispfrmstr(frmstr)
 char *frmstr;
 {for(;*frmstr;frmstr++)
   {if (*frmstr == '\n') puts("<cr>");
    else	         putchar(*frmstr);
    } 
  }













	  


				/* Display the list format definition */
displf(lf,desc)
 struct listformat *lf;    
 char *desc;
 {hdcpyopt();
  printf("\n\n                   %s:",desc);
  dispfldvec("I. LIST HEADING",lf->listhead.headfield,
	lf->listhead.nheadfields,lf->strbuf);
  dispfldvec("II. PAGE FORMAT",lf->page.pagehead.headfield,
	lf->page.pagehead.nheadfields,lf->strbuf);
  if (lf->page.formht > 0) 
     printf("\n    Page Size = %d lines; Bottom Margin = %d lines",
 	lf->page.formht,lf->page.bottommarg);
  dispfldvec("III. MEMBER FORMAT",lf->memform.memfield,
	lf->memform.nmemfields,lf->strbuf);
  printf("\nIV. SORT KEYS");
  dispkeys(&lf->keys,lf->strbuf);
  err0("");
  }

				/* Display an array of output fields */
dispfldvec(str,lffld,nflds,strbuf)
 char *str,*strbuf;
 struct lffield *lffld;
 int nflds;
 {int i;
  printf("\n%s",str);
  for(i=0;i<nflds;i++)
     displffld(&lffld[i],i,strbuf);
  }



				/* Display the sort keys */
dispkeys(srtkeys,strbuf)
 struct sortkeys *srtkeys;
 char *strbuf;
 {int i;
  for(i=0;i<srtkeys->nkeys;i++)
    dispkey(&srtkeys->key[i],i,strbuf);
  }



				/* Display a single sort key */
dispkey(ky,i,strbuf)
 struct key0 *ky;
 int i;
 {char desc[MAXDESCSZ];
  int j;
  setmargin(LFDSPMARGIN);
  printf("\nKey %d: [%s type]; Sort by ",
	i+1,extropt(ky->type-1,KEYTYPES,desc));
  if (ky->type == LOGFIELD) puts(FIELD[ky->fldno].tag);
  else			    puts(mktmtag(FIELD[ky->fldno].tag,desc));
  if (FIELD[ky->fldno].type == NUMERIC) 
     printf("; %s sort",extropt(ky->direction,SORTDIRS,desc));
  if (ky->nsubhdflds > 0) printf("\nSubheading Fields:");
  for(j=0;j<ky->nsubhdflds;j++)
    displffld(&ky->subhead[j],j,strbuf);
  setmargin(-LFDSPMARGIN);
  }

				/* Display an output field */
displffld(fld,fldno,strbuf)
 struct lffield *fld;
 int fldno;
 char *strbuf;
 {char desc[MAXDESCSZ];
  if (procchar() == ESC) return;
  setmargin(LFDSPMARGIN);
  printf("\nField Number %d:  ",fldno+1);
  if (fld->dispmode & UNDERMASK) setmode(UNDERDSP);
  printf("<%s> ",extropt(fld->type,FIELDTYPES,desc));
  if (fld->type == STRING) 
     {putchar('"');
      dispfrmstr(strbuf + fld->fldid);    
      putchar('"');
      }
  if (fld->type == TEMPLATE) puts(mktmtag(FIELD[fld->fldid].tag,desc));
  if (fld->type == LOGFIELD) puts(FIELD[fld->fldid].tag);
  setmode(NORMALDSP);
  if (fld->posmode != NOPOSIT)
    {printf("\nField Length = %d",fld->fldlen);   
     printf("; Position = %s",extropt(fld->posmode,POSMODES,desc));
     }
   putchar('\n');
   setmargin(-LFDSPMARGIN);
   }
 
     



       

				/* Initialize a List Fomrat */
initlf(lf)
 struct listformat *lf;
 {lf->listhead.nheadfields = 0;
  lf->page.formht = 0;
  lf->page.bottommarg = 0;
  lf->page.pagehead.nheadfields = 0;
  lf->memform.nmemfields = 0;
  lf->keys.nkeys = 0;
  lf->strbuf[0] = EOB;
  }





				/* Initialize an Output Field */
initof(lffld)
 struct lffield *lffld;
 {lffld->posmode = NOPOSIT;
  lffld->dispmode = 0;
  }



				/* Copy output field lffld2 to lffld1 */
lffldcpy(lffld1,lffld2)
 struct lffield *lffld1,*lffld2;
 {strcpy0(lffld1,lffld2,sizeof(*lffld1));
  }



				/* Copy sort key key2 to key1 */
keycpy(key1,key2)
 struct key0 *key1,*key2;
 {strcpy0(key1,key2,sizeof(*key1));
  }





