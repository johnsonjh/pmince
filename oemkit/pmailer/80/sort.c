
#include "constant.inc"
#include "sys.inc"
#include "listform.inc"
#include "subset.inc"
#include "list.inc"
#include "db.inc"


			/* Extract the keys of the members in subset sub; 
			    sort them according to format.  Return INCORE 
			    and put sort image in list if sort is done in
			    core; else write merge files and return INFILE */
sort(sub,format,params,list)
 struct subset *sub;
 struct listformat *format;
 struct listparams *params;
 char *list[MAXLISTSZ];
 {int sect0,nrec;
  char keybuf[KEYBUFSZ],mode;
  sect0 = 1;  
  nrec = 0;
  printf("Reading Sector       (of %d)",NEXTSECTOR);
  for(params->nfiles = 0;;params->nfiles++)
    {mode = 
       getkeybuf(keybuf,keybuf + KEYBUFSZ,list,&sect0,sub,&format->keys,&nrec);
     if (mode == ESC) return(ESC);
     if (mode == INCORE) break;
     wrtrun(list,params->nfiles);
     }
  if (params->nfiles == 0)
     {compress(list);
      return(INCORE);
      }
  wrtrun(list,params->nfiles++);
  printf("\nSubset Contains %d Members\n",nrec);
  return(INFILE);
  }



			/* Compress the list of strings into a list
			     of disk sectors */
compress(list)
 char *list[MAXLISTSZ];
 {int i;
  for(i=0;list[i];i++)
     strcpy0(&list[i],list[i]+keylen(list[i])+1,2);
  }





				/* Extract the keys from each member that
				    matches subset sub; put them in keybuf */
getkeybuf(keybuf,bufmax,list,sect0,sub,srtkeys,nrec)
 char *keybuf,*bufmax,*list[MAXLISTSZ];
 int *sect0,*nrec;
 struct subset *sub;
 struct sortkeys *srtkeys;
 {int i,nsect,count;
  char mem[MEMBUFSZ],*datlst[MAXNLFLDS],*datlst0[NSSMEM][MAXNLFLDS];
  for(i=0;i<NSSMEM;i++) lnkdat(sub->ssmem[i],datlst0[i],FIELD,SYS.nlflds);
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  list[0] = 0;
  for(i=count=0;i < MAXLISTSZ-1;)
     {if (count-- == 0)
	{if (procchar() == ESC) return(ESC); 
         pos(0,16); printf("%d",*sect0);
	 count = SECTDISPCT;
         }
      if (*sect0 >= NEXTSECTOR) break;
      if ((nsect = rdmbr0(datlst,*sect0,SYS.nlflds,DBFD)) == DELETED)
	 nsect = 1;
      else 
        if (memcmp(datlst,sub->relop,datlst0,FIELD,SYS.nlflds))
           {keybuf = stripkeys(list[i] = keybuf,datlst,sect0,srtkeys);
            list[++i] = 0;
	    }
      *sect0 += nsect;
      if (keybuf + SCREENWD * srtkeys->nkeys > bufmax) break;
      }
  printf("\nRun contains %d members  ",i);
  printf("\nShell Sort v1.2");
  if (srtkeys->nkeys > 0) qsort(list,i);  
  *nrec += i;
  if (*sect0 < NEXTSECTOR) return(INFILE);
  return(INCORE);
  }




				/* Strip the keys from datlst, put them in
				     buf and then append sect0; return pointer
				     to next free buffer position */
stripkeys(buf,datlst,sect0,srtkeys)
 char *buf,*datlst[MAXNLFLDS];
 char sect0[2];
 struct sortkeys *srtkeys;
 {int i,ifld;
  for(i=0;i<srtkeys->nkeys;i++)
    {ifld = srtkeys->key[i].fldno;
     if (srtkeys->key[i].type == TEMPLATE)
       buf = striptmplt(buf,datlst,ifld,srtkeys->key[i].direction);
     else
       if (FIELD[ifld].type == NUMERIC && FIELD[ifld].length < MAXNDIG)
	 buf = stripnum(buf,datlst,ifld,srtkeys->key[i].direction);
        else
           buf = stripfld(buf,datlst,ifld);
     if (i < srtkeys->nkeys-1) *buf++ = EOK;
     }
  *buf++  = 0;
  *buf++ = sect0[0];
  *buf++ = sect0[1];
  return(buf); 
  }

			/* Strip a non-numeric logical field sort key;
			     return the next free buffer position   */
stripfld(buf,datlst,ifld)
 char *buf,*datlst[MAXNLFLDS];
 int ifld;
 {strcpylc(buf,datlst[ifld]);
  return(buf+strlen(datlst[ifld]));
  }



				/* Strip a numeric sort key; return
				   next free buffer position   */
stripnum(buf,datlst,ifld,direc)
 char *buf,*datlst[MAXNLFLDS];
 int ifld,direc;
 {int n;
  char *cp;
  n = getdig(datlst,ifld,direc);
  cp = &n;
  *buf++ = NUMERIC;
  *buf++ = *cp++;
  *buf++ = *cp;
  return(buf);
  }

				/* Strip a template key */ 
striptmplt(buf,datlst,ifld,direc)
 char *buf,*datlst[MAXNLFLDS];
 int ifld,direc;
 {char hourch;
  switch(FIELD[ifld].tmpltno)
    {case NAME:
       buf = strcpylc(buf,datlst[ifld+2]);
       *buf++ = ',';
       buf = strcpylc(buf,datlst[ifld]);
       *buf++ = ',';
       buf = strcpylc(buf,datlst[ifld+1]);
       break;
     case DATE:
       *buf++ = getdig(datlst,ifld+2,direc);
       *buf++ = getdig(datlst,ifld,direc);
       *buf++ = getdig(datlst,ifld+1,direc);
       break;
     case TIME:
       *buf = *datlst[ifld+2]  ? 1 : 2;	/* AM=> 1; PM=>2 */
       if (direc == REVERSE) *buf = 3 - *buf;
       buf++;
       hourch = atoi(datlst[ifld]);
       if (hourch < 12) hourch += 1;	/* If hour < 12 hour = hour+1; */
       else             hourch = 1;	/* If hour == 12 hour = 1;     */
       *buf++ = (direc == FORWARD) ? hourch : 13-hourch;
       *buf++ = getdig(datlst,ifld,direc);
       break;
     } 
  return(buf);
  }




			/* Strip off one digit */
getdig(datlst,ifld,direc)
 char *datlst[MAXNLFLDS];
 int ifld,direc;
 {if (direc == FORWARD) return(atoi(datlst[ifld]));
  return(FIELD[ifld].maxval - atoi(datlst[ifld]));
  }






				/* Write out a run to the disk */
wrtrun(list,run)
 char **list;
 int run;
 {int i,sect0;
  char filename[MAXFNSZ];
  if (fopen0(mkmrgfil(run,filename),FWRITE) < 0) exit();
  for(i=0;list[i];i++) 
    fputbuf(list[i],keylen(list[i]) + 3);
  rawfput(EOF);
  fclose();
  }




