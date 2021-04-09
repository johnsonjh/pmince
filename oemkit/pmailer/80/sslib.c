#include "constant.inc"
#include "sys.inc"
#include "subset.inc"
#include "db.inc"

				/* Get a subset from the user */
getss(mesg,sub,iss,sslist,field,nlflds,helpid,helpfile)
  char *mesg,helpid,*helpfile;
  struct subset *sub;
  int *iss,nlflds;
  struct ssdirectory *sslist;
  struct logfield field[MAXNLFLDS];
  {if (ssmenu(mesg,iss,sslist,helpid,helpfile) == ESC) return(ESC);
   if (*iss == 0) sszero(sub,field,nlflds);
   else 	  rdss(sub,(*iss)-1,sslist,field,nlflds);
   return(0);
   }



				/* Present the menu of subset options */
ssmenu(mesg,iss,sslist,helpid,helpfile)
  char *mesg,helpid,*helpfile;
  int *iss;
  struct ssdirectory *sslist;
  {if (sslist->nsubsets == 0)
     {err0("No subsets have been defined");
      return(ESC);
      }
   return(imenu(mesg,sslist->ssdesclist,iss,helpid,helpfile));
   }




				/* Initialize a new subset */
sszero(sub,field,nlflds)
  struct subset *sub;
  int nlflds;
  struct logfield field[MAXNLFLDS];
  {int i,k;
   char *datlst[NSSMEM][MAXNLFLDS];
   for(k=0;k<NSSMEM;k++)
     {lnkdat(sub->ssmem[k],datlst[k],field,nlflds);
      for(i=0;i<nlflds;i++)
        {datlst[k][i][0] = 0;
         sub->relop[k][i] = EQ;
         }
      }
  }










				/* Write the subset out to the disk file
				     SSFILE and enter it in the list at
				     position iss */
wrtss(sub,iss,sslist,field,nlflds)
  struct subset *sub;
  struct ssdirectory *sslist;
  int iss,nlflds;
  struct logfield field[MAXNLFLDS];
  {char buffer[MEMBUFSZ],dummy1[MEMBUFSZ],dummy2[MAXNLFLDS],dummy3[MAXNLFLDS];
   char	*datlst[MAXNLFLDS],*b,*d;
   int sect,nsect,i,k;
   setmem(buffer,2*MEMBUFSZ + 2*MAXNLFLDS,-1);
   nsect = sssectct(sub);
   if (iss == NEWENTRY || nsect > sslist->ssentry[iss].sectorct)
     {if (iss == NEWENTRY) iss = sslist->nsubsets++;
      sslist->ssentry[iss].sectorst = sslist->nextsssect;
      sslist->ssentry[iss].sectorct = nsect;
      sslist->nextsssect += nsect;
      }
   sect = sslist->ssentry[iss].sectorst;
   if (seek(sslist->ssfd,sect,0) < 0) exit();
   b = buffer;
   for(k=0;k<NSSMEM;k++)
     {lnkdat(sub->ssmem[k],datlst,field,nlflds);
      for(i=0;i<nlflds;i++)
        {d = datlst[i];
         while(*b++ = *d++);
         }   
      for(i=0;i<nlflds;i++)
         *b++ = sub->relop[k][i];
      } 
   write(sslist->ssfd,buffer,nsect);
   }


				/* Read the subset at list position iss 
				     from the disk */ 
rdss(sub,iss,sslist,field,nlflds)
  struct subset *sub;
  int iss,nlflds;
  struct ssdirectory *sslist;
  struct logfield field[MAXNLFLDS];
  {char buffer[MEMBUFSZ],dummy1[MEMBUFSZ],dummy2[MAXNLFLDS],dummy3[MAXNLFLDS];
   char	*datlst[MAXNLFLDS],*b,*d;
   int i,k;
   if (seek(sslist->ssfd,sslist->ssentry[iss].sectorst,0) < 0) return(-1);
   if (read(sslist->ssfd,buffer,sslist->ssentry[iss].sectorct) < 0) return(-1);
   b = buffer;
   for(k=0;k<NSSMEM;k++)
     {lnkdat(sub->ssmem[k],datlst,field,nlflds);
      for(i=0;i<nlflds;i++)
        {d = datlst[i];
         while(*d++ = *b++);
         }   
      for(i=0;i<nlflds;i++)
         sub->relop[k][i] = *b++;
      } 
   }



				/* Count the number of sectors subset
				     sub will occupy */
sssectct(sub)
 struct subset *sub;
 {int i,k,n,nsect;
  char *datlst[MAXNLFLDS];
  for(k=n=0;k<NSSMEM;k++)
    {lnkdat(sub->ssmem[k],datlst,FIELD,SYS.nlflds);
     for(i=0;i<SYS.nlflds;i++)
       n += strlen(datlst[i])+2;
     }
  nsect = n/SECTORSZ;
  if (n%SECTORSZ) nsect++;
  return(nsect);
  }



				/* Open subset file and read in 
				    subset descriptions and list */
openss(sslist,dir,option)
 struct ssdirectory *sslist;
 char *dir,option;
 {int i;
  char descbuf[SSDSCLSTSZ],filbuf[MAXFNSZ];
  rdmem(mkpath(dir,SSLISTFILE,filbuf),sslist,sizeof(*sslist));
  rdfil(mkpath(dir,SSDESCFILE,filbuf),descbuf,SSDSCLSTSZ);
  if (option == DEFINESUBSETS)
    strcpy(sslist->ssdesclist,descbuf);
  if (option == USESUBSETS)
    {strcpy(sslist->ssdesclist,"All Members");
     if (descbuf[0])
       {ccat(sslist->ssdesclist,',');
        strcat(sslist->ssdesclist,descbuf);
        }
     }
  sslist->nsubsets = optct(sslist->ssdesclist);
  if ((sslist->ssfd = open(mkpath(dir,SSFILE,filbuf),
	option == DEFINESUBSETS ? 2 : 0)) < 0) 
    return(err("Cannot open file %s",filbuf));
  return(0);
  }


			/* Close and write out files */
closess(sslist,chngflg)
 struct ssdirectory *sslist;
 char chngflg;
 {char filbuf[MAXFNSZ];
  if (chngflg == 0) return(fabort(sslist->ssfd));
  wrtmem(mkpath(DBDIR,SSDESCFILE,filbuf),
	&sslist->ssdesclist[0],strlen(sslist->ssdesclist));
  wrtmem(mkpath(DBDIR,SSLISTFILE,filbuf),
	sslist,sizeof(*sslist));
  close(sslist->ssfd);
  }
  

				/* Display the subset characteristics */
dispss(sub,desc)
 struct subset *sub;
 char *desc;
 {int i,k;
  char *datlst[NSSMEM][MAXNLFLDS],buffer[12];
  hdcpyopt();
  printf("\nSubset of %s:",desc);
  for(k=0;k<NSSMEM;k++)
     lnkdat(sub->ssmem[k],datlst[k],FIELD,SYS.nlflds);
  for(k=0;k<NSSMEM;k++)
    for(i=0;i<SYS.nlflds;i++)
       if (datlst[k][i][0] != 0)
	  dispfld(&i,datlst[k],sub->relop[k][i]);
   printf("\n\n");
   err0("");
   }




				/* Display a single subset field */
dispfld(ifld,datlst,op)
 int *ifld;
 char *datlst[MAXNLFLDS],op;
 {int i;
  char relopbuf[MAXDESCSZ],buffer[MAXTAGSZ];
  extropt(op,RELOPS,relopbuf);
  mktmtag(FIELD[*ifld].tag,buffer);
  if (FIELD[*ifld].tmpltpos == 0) switch(FIELD[*ifld].tmpltno)
    {case DATE:
       if (*datlst[*ifld] == 'T')
	  {printf("\n%s <%s> <current date> + %d days",
		buffer,relopbuf,datlst[*ifld+2][1] - 48);
	   (*ifld) += 2;
	   return;
	   }
       if (*datlst[*ifld] == 0 || *datlst[(*ifld)+1] == 0 || 
		*datlst[(*ifld)+2] == 0) break;
       printf("\n%s <%s> %s/%s/%s",
	  buffer,relopbuf,datlst[*ifld],datlst[(*ifld)+1],datlst[(*ifld)+2]);
       (*ifld) += 2;
       return; 
     case TIME:
       printf("\n%s <%s> %s:%s %s",buffer,relopbuf,datlst[(*ifld)++],
		datlst[(*ifld)++],datlst[(*ifld)++] ? "AM" : "PM");
       return;
     }
   if (FIELD[*ifld].type == STATUS)
     {printf("\n%s <%s> %s",
	FIELD[*ifld].tag,relopbuf,datlst[*ifld][0] == 'x' ? "true" : "false");
      return;
      }
   printf("\n%s <%s> %s",FIELD[*ifld].tag,relopbuf,datlst[*ifld]);
   }






