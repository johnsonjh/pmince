#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "help.inc"
#include "db.inc"



/*
			---------------------
			| Run Time Routines |
			---------------------
*/

			/* Open the data base file and read in NEXTSECTOR */
opendb(directory,nextsector)
 char *directory;
 int *nextsector;
 {int *ibuf,fd;
  char filbuf[MAXDESCSZ];
  if ((fd = open(mkpath(directory,DBFILE,filbuf),2)) < 0) 
     exit(err("Cannot open data base file %s",filbuf)); 
  read(fd,IOBUF,1);
  ibuf = IOBUF;
  *nextsector = ibuf[0];
  return(fd);
  } 





				/* Close the data base */
closedb()
  {int *ibuf;
   if (cpmseek(DBFD,0,0) < 0) exit();
   cpmread(DBFD,IOBUF,1);
   ibuf = IOBUF;
   ibuf[0] = NEXTSECTOR;
   if (cpmseek(DBFD,0,0) < 0) exit();
   cpmwrite(DBFD,IOBUF,1);
   close(DBFD);
   DBFD = opendb(DBDIR,&NEXTSECTOR);
   } 




/*
		--------------------------- 
		| Screen Display Function |
		---------------------------
*/




			/* Display the data entry form */
dispfrm()	
  {char *cp,mode;  
   int i;       
   setmode(CLEAR);
   setmode(mode = HALFDSP);
   for(i=0;i<SCREENHT;i++)
     {for(cp = SCREEN[i];*cp;cp++) 
	{if (*cp < ' ')
	   {if (mode == HALFDSP) setmode(mode = NORMALDSP);
	    putch(' ');
	    }
         else
	   {if (mode == NORMALDSP) setmode(mode = HALFDSP);
	    putch(*cp);
	    }
	 }
      if (i < SCREENHT-1) {putch(CR); putch('\n');}
      }
   }


        


/*
		-----------------------------------
		| Functions of Individual Members |
		-----------------------------------
*/


				/* Link the list of pointer (datlst) to the 
			 	     buffer that will hold the data (mem) */
lnkdat(mem,datlst,lfield,nlflds0)
 char mem[MEMBUFSZ],*datlst[MAXNLFLDS];
 struct logfield lfield[MAXNLFLDS];
 int nlflds0;
 {int i;
  datlst[0] = mem;
  for(i=1;i<nlflds0;i++)
     datlst[i] = datlst[i-1] + lfield[i-1].length + 1;
  }




				/* Initialize a new member */
zero(datlst)	
 char *datlst[MAXNLFLDS];
 {int i;
  for(i=0;i<SYS.nlflds;i++)
    datlst[i][0] = 0;
  }


	        /* Set the entry date of member to today  (only if 
		    there is a date template called dtenter) */
setdate(datlst)	
 char *datlst[MAXNLFLDS];
 {int ifld;
  if ((ifld = tmpmatch("dtenter")) < 0) return;
  strcpy(datlst[ifld],TODAY.month);
  strcpy(datlst[ifld+1],TODAY.day);
  strcpy(datlst[ifld+2],TODAY.year);
  }






				/* Fill recorded values into the screen form */
fill0(datlst)	
 char *datlst[MAXNLFLDS];
 {int i;
  for(i=0;i<SYS.nlflds;i++)
    filldat(datlst,i);
  }



				/* Fill field ifld into the screen form */
filldat(datlst,ifld)
 char *datlst[MAXNLFLDS];
 int ifld;
 {int i,j;
  char *dat;
  dat = datlst[ifld];
  pos(i=FIELD[ifld].ipos,j=FIELD[ifld].jpos);
  while(*dat)
    {if (SCREEN[i][j] >= ' ') 
       {while(SCREEN[i][j] >= ' ') j++;
	pos(i,j);
	}
     putch(*dat++);
     j++;
     }
  }






   



				/* Read the member referred to by hashtable   
				    entry entryno in from the disk file. */
rdmbr(datlst,entryno,htab)
  char *datlst[MAXNLFLDS];
  int entryno;
  struct hashtable *htab;
  {int sect0;
   rdentry(entryno,htab);
   if (entryno >= htab->nextfree) return(-1);
   if ((sect0 = entry(entryno,htab)->sector0) < 0) return(-1);
   return(rdmbr0(datlst,sect0,SYS.nlflds,DBFD));
   }


rdmbr0(datlst,sect0,nlflds,dbfd)
  char *datlst[MAXNLFLDS];
  int sect0,nlflds,dbfd;
  {int i,nsect;
   char *d,*cp,membuf[MEMBUFSZ];
   if (cpmseek(dbfd,sect0,0) < 0) exit();
   cpmread(dbfd,membuf,1);
   nsect = membuf[0];
   if (nsect == DELETED || nsect == 0 || nsect > MEMBUFSZ / SECTORSZ) 
	return(DELETED);
   if (nsect > 1) cpmread(dbfd,&membuf[SECTORSZ],nsect-1);
   cp = membuf+1;
   for(i=0;i<nlflds;i++)
     {d = datlst[i];
      while(*d++ = *cp++);
      }
   return(nsect);
   }


				/* Write the member at htab entry entryno */  
wrtmbr(datlst,entryno,htab)
  char *datlst[MAXNLFLDS];
  int entryno;
  struct hashtable *htab;
  {int i,nsect;
   char *d,*cp,mem[MEMBUFSZ];
   setmem(mem,MEMBUFSZ,-1);
   cp = mem+1;
   for(i=0;i<SYS.nlflds;i++)
     {clrblnks(datlst[i]);        /* Clear out leading and trailing blanks */
      d = datlst[i];
      while(*cp++ = *d++);    
      }
   nsect = sectct(cp-mem);
   if (entryno != NEWENTRY)
     {rdentry(entryno,htab);
      if (nsect != memsect(entryno,htab))
        {deletemem(entryno,htab);
         entryno = NEWENTRY;
         }
      }
   if (entryno == NEWENTRY)
     {if ((entryno = mkentry(datlst,htab)) < 0) return(-1);
      NEXTSECTOR += nsect;   
      }
   mem[0] = nsect;   
   rdentry(entryno,htab);
   if (cpmseek(DBFD,entry(entryno,htab)->sector0,0) < 0) exit();
   cpmwrite(DBFD,mem,nsect);
   return(0);
   }







			/* See how many sectors the member at
			      entry entryno occupies */
memsect(entryno,htab)
 int entryno;
 struct hashtable *htab;
 {int nsect,s0;
  rdentry(entryno,htab);
  if (cpmseek(DBFD,s0=entry(entryno,htab)->sector0,0) < 0) exit();
  cpmread(DBFD,IOBUF,1);
  return(nsect=IOBUF[0]);
  }








					/* Read in the data tags.  */
rdtag(field,tagbuf,nlflds)
  struct logfield field[MAXNLFLDS];
  char tagbuf[TAGBUFSZ];
  {int i;
   for(i=0;i<nlflds;i++)
     {field[i].tag = tagbuf;
      while((*tagbuf++ = rawfget()) != 0);
      }
   }





				  /* Accept a Field Tag from the user.  Ifld
				     is returned as logical field number */
accfldtag(mesg,ifld,helpid,helpfile)
  char *mesg,helpid,*helpfile;
  int *ifld;
  {int i,tag0[MAXTAGSZ];
   while(1)
     {if (gettok(mesg,tag0,MAXTAGSZ,helpid,helpfile) == ESC) return(ESC);
      if ((*ifld = fldmatch(tag0)) >= 0) return(0);
      err("No such logical field %s",tag0);
      }
   }


				/* See if tag0 matches a logical field */
fldmatch(tag0)
  char *tag0;
  {int i;
   for(i=0;i<SYS.nlflds;i++)
     if (strcmp(FIELD[i].tag,tag0) == 0) return(i);
   return(-1);
   }				






				/* Accept a template tag, return logical
				    field number of first template field 
   				    as ifld */
acctmptag(mesg,ifld,helpid,helpfile)
  char *mesg,helpid,*helpfile;
  int *ifld;
  {int i,tag0[MAXTAGSZ];
   while(1)
     {if (gettok(mesg,tag0,MAXTAGSZ-1,helpid,helpfile) == ESC) return(ESC);
      if ((*ifld = tmpmatch(tag0)) >= 0) return(0);
      err("No such template %s",tag0);
      }
   }

			/* See which template tag0 matches */
tmpmatch(tag0)
  char *tag0;
  {int i;
   char buffer[MAXTAGSZ];
   strcpy(buffer,tag0);
   ccat(buffer,'.');
   for(i=0;i<SYS.nlflds;i++)
      if (strmatch(FIELD[i].tag,buffer) == 0) return(i);
   return(-1);
   }










/*
			----------------------------------
			|  Help Message Display Function |
			----------------------------------
*/
			/* Print the helpid'th screen in file helpfile */
help(helpid,helpfile)
 int helpid;      
 char *helpfile;
  {int fd,i,iopos;
   char filbuf[MAXFNSZ],c;
   struct helpscr helptab[HELPTABSZ];
   setmode(CLEAR);
   if ((fd = open(mkpath(SYS.helpdir,helpfile,filbuf),0)) < 0)
     return(err("Cannot find helpfile %s",filbuf));
   rddac(fd,0,&helptab[0],HELPTABSZ * sizeof(helptab[0]));
   seek(fd,helptab[helpid].hsector,0);
   read(fd,IOBUF,1);
   iopos = helptab[helpid].offset;
   for(i = 0;i < helptab[helpid].nchars;i++)
     {if (iopos == SECTORSZ)
        {if (read(fd,IOBUF,1) <= 0) break;
	 iopos = 0;
	 }
      if ((c = IOBUF[iopos++]) == TAB)
	{putchar(' ');
	 while(CARRIAGEPOS % 8) putchar(' ');
	 }
      else 
	putchar(c);
      }
   fabort(fd);
   pos(SCREENHT-1,0); printf("Hit any character to return to program");
   getchar();
   setmode(CLEAR);
   }









#include "cpmlib.c"
