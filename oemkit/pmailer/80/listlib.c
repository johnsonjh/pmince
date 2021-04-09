#include "constant.inc"
#include "sys.inc"
#include "listform.inc"
#include "list.inc"
#include "db.inc"

				/* Make a file name for external merge-sort */
mkmrgfil(run,filbuf)
 int run;
 char filbuf[MAXFNSZ];
 {*filbuf = 0;
  ccat(filbuf,*SYS.sortdir);
  ccat(filbuf,':');
  strcat(filbuf,"merge.");
  itoa(filbuf+strlen(filbuf),run);
  return(filbuf);
  }



			/* Return the length of sort keys */
keylen(key)	
 char *key;
 {char *k0;
  for(k0=key;*key;key++)
    if (*key == NUMERIC) key += 2;
  return(key-k0);
  }



				/* Write out the sort image file */
putimage(list,imagefile)
 char *list[MAXLISTSZ],*imagefile;
 {int i;
  if (fopen0(imagefile,FWRITE) < 0) return;
  for(i=0;fputw(list[i]);i++);
  fclose();
  }



			/* Copy the data fields from datlst2 to datlst1 */
datcpy(datlst1,datlst2)
 char *datlst1[MAXNLFLDS],*datlst2[MAXNLFLDS];
 {int i;
  for(i=0;i<SYS.nlflds;i++)
    strcpy(datlst1[i],datlst2[i]);
  }






			/* Generate a list format block; 
			    return number of lines used  */
genlfblk(lffld,nlffld,datlst,ssdesc,strbuf,line,formht)
 struct lffield *lffld;
 int nlffld,*line,formht;
 char *datlst[MAXNLFLDS],*ssdesc,*strbuf;
 {int i,fldid0;
  char buffer[SYSBUFSZ];
  for(i=0;i<nlffld;i++)
    {fldid0 = lffld[i].fldid;
     *buffer = 0;
     switch(lffld[i].type)
       {case LOGFIELD:
	  strcpy(buffer,datlst[fldid0]);
	  break;
	case TEMPLATE:
	  instmplt(buffer,datlst,fldid0);
	  break;
	case STRING:
	  strcpy(buffer,strbuf + fldid0);
	  break;
	case TODAYDATE:
	  insdate(buffer,&TODAY);
 	  break;
	case SSNAME:
	  strcpy(buffer,ssdesc);
	  break;
	case PAGENUM:
	  strcat(buffer,"Page ");
	  itoa(buffer + strlen(buffer),1 + *line / formht);
	  break;
	default:
	  err("Unrecognized Field Type %d",lffld[i].type);
	  exit();
	}
     setfldlen(buffer,lffld[i].posmode,lffld[i].fldlen);
     setdspmode(lffld[i].dispmode);
     puts(buffer);
     setdspmode(lffld[i].dispmode);
     *line += linect(buffer);
     }
  }
     


				/* Set the length of the output field */
setfldlen(buffer,mode,fldlen)        
 char *buffer,mode;
 int fldlen;       
 {int i,len0,n;
  if (mode == NOPOSIT) return;
  len0 = strlen(buffer);
  buffer[fldlen] = 0;
  switch(mode)
    {case LEFTJUST:
	for(i=len0;i<fldlen;i++) buffer[i] = ' ';
	return;
     case CENTER:
	n = (fldlen-len0)/2;
	advstr(buffer,n);
	for(i=0;i<n;i++) buffer[i] = ' ';
	for(i=len0+n;i<fldlen;i++) buffer[i] = ' ';
	break;  
     case RIGHTJUST:
	if (len0 > fldlen) break;
	advstr(buffer,fldlen-len0);
	for(i=0;i<fldlen-len0;i++) buffer[i] = ' ';
	break; 
     }
   }  

				/* Set the output field display mode */
setdspmode(mode)
 char mode;
 {setmode(NORMALDSP);
  if (mode & UNDERMASK) setmode(UNDERDSP); 
  }




				/* Set up the list targets */
settarg(params)
 struct listparams *params;
 {CRTSW = params->target & CRTMASK;
  HDCPYSW = params->target & HDCPYMASK;
  FILESW = params->target & FILEMASK;
  if (FILESW) fopen0(params->listfile,FWRITE);
  }







			/* Generate Subheadings. (Current member is datlst;
			    previous member was datlst0) */
gensubhead(datlst,datlst0,srtkeys,ssdesc,strbuf,line,formht)
 char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS],*strbuf,*ssdesc;
 struct sortkeys *srtkeys;
 int *line,formht;
 {int i,ifld;
  for(i=0;i<srtkeys->nkeys;i++)
     {ifld = srtkeys->key[i].fldno;
      if (fldcmp(datlst,datlst0,&ifld,srtkeys->key[i].type,FIELD) != EQUALS)
         genlfblk(srtkeys->key[i].subhead,srtkeys->key[i].nsubhdflds,
		datlst,ssdesc,strbuf,line,formht);
      }
  return(0);
  } 


				/* Generate a page heading */
genpghead(page,datlst,ssdesc,strbuf,line)
 struct pageformat *page;  
 char *datlst[MAXNLFLDS],*ssdesc,*strbuf;
 int *line;
 {int nformht,nlastln;  
  if ((*line % page->formht) < page->formht - page->bottommarg) return(0);
  for(;*line % page->formht;(*line)++) putchar('\n'); 
  genlfblk(page->pagehead.headfield,page->pagehead.nheadfields,
	datlst,ssdesc,strbuf,line,page->formht);
  return(0);
  }
