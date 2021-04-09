#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "db.inc"

/*
		-------------------------------------
		| Hash Table Manipulation Functions |
		-------------------------------------
*/

			/* Enter member mem in hashtable htab */
mkentry(datlst,htab)			
 char *datlst[MAXNLFLDS];
 struct hashtable *htab;
  {struct htabentry entry0;
   char memid0[MEMIDSZ];
   int bucket,entryno;
			/* See into which bucket the member hashes. */
   bucket = hash(mkmemid(datlst,memid0),htab);
		      /* If the bucket slot is empty, put the member there. */ 
   rdentry(bucket,htab);
   if (entry(bucket,htab)->memid[0] == 0) 
	entryno = bucket;
			/* Else put her into the next free entry slot.  */
   else
      {entryno = htab->nextfree++;
       setlink(bucket,entryno,htab);
       }
   strcpy0(entry0.memid,memid0,MEMIDSZ);
   entry0.sector0 = NEXTSECTOR; 
   entry0.link = 0;
   wrtentry(&entry0,entryno,htab);
   if (BACKUPFLAG) if (++NENTRIES >= SYS.dirbakno) updatedb(htab);
   return(entryno);
   }





			/* Make a forward link from entry e0 to entry e1 */
setlink(e0,e1,htab)     
 int e0,e1;		
 struct hashtable *htab; 
  {int e;
			/* Step through the links of the hashtable */
   while(1)
    {rdentry(e0,htab);
     if ((e = entry(e0,htab)->link) == 0)  /* If link is zero, then 
					this is the last link in the chain,
					 so make new link here */
       {entry(e0,htab)->link = e1;
        wrtentry(entry(e0,htab),e0,htab);
        return;
        }
     e0=e;         	                /* Move to the next link */
     }
   }




		/* Find the n'th entry whose member ID matches memid0 */
fndentry(memid0,htab,n)
 char memid0[MEMIDSZ];
 struct hashtable *htab;
 int n;
 {int k;
  if ((k = fndentr0(memid0,htab,n)) >= 0) return(k);
  if (n == 0) return(err0("No such member"));
  setmode(CLEAR);
  return(err0("No more members"));
  }


  		 /* Thread through the linked list to find the nth entry
		      in htab to match memid0: return entry number; or -1
		      if unsuccessful  */
fndentr0(memid0,htab,n)
 char memid0[MEMIDSZ];
 struct hashtable *htab;
 int n;
 {int e0,e;
  for(e0=hash(memid0,htab);;)     
     {rdentry(e0,htab);
      if (strcmp0(entry(e0,htab)->memid,memid0,MEMIDSZ))
	  if (n-- == 0) return(e0);
      if ((e = entry(e0,htab)->link) == 0) return(-1);
      e0 = e;
      }
   }


		  	/* Read in entry entryno from hashtable *htab */
rdentry(entryno,htab)
 int entryno;
 struct hashtable *htab;
 {int sect;
  if ((sect = 1+entryno/(SECTORSZ/ENTRYSZ)) != htab->sectorno)
     {if (seek(htab->fd,htab->sectorno=sect,0) < 0) exit();
      read(htab->fd,htab->htabbuf,1);
      }
  }

			/* Return the storage location of entry entryno
			    in hashtable *htab */
struct htabentry *entry(entryno,htab)
 int entryno;
 struct hashtable *htab;
 {return(&htab->htabbuf[ENTRYSZ*(entryno%(SECTORSZ/ENTRYSZ))]);
  }


			/* Write out entry entry0 to position entryno
			     in hashtable *htab */
wrtentry(entry0,entryno,htab)
 struct htabentry *entry0;
 int entryno;
 struct hashtable *htab;
 {int sect;
  rdentry(entryno,htab);
  strcpy0(entry(entryno,htab),entry0,ENTRYSZ);
  if (seek(htab->fd,sect=1+entryno/(SECTORSZ/ENTRYSZ),0) < 0) exit();
  write(htab->fd,htab->htabbuf,1);
  }




hash(memid0,htab)			/* Hash the member id */           
  int memid0[2];
  struct hashtable *htab;
  {return(abs(memid0[0] + memid0[1])%htab->nbuckets);
   }

					/* Delete a member */
deletemem(entryno,htab)
 int entryno;
 struct hashtable *htab;
 {int i,nsect,s0;
  for(i=0;i<MEMIDSZ;i++) 
     entry(entryno,htab)->memid[i] = 
        (entry(entryno,htab)->link == 0) ? 0 : DELETED;
  nsect = memsect(entryno,htab);
  rdentry(entryno,htab);
  s0 = entry(entryno,htab)->sector0;
  entry(entryno,htab)->sector0 = -1;
  wrtentry(entry(entryno,htab),entryno,htab);
  if (s0 <= 0 || s0 >= NEXTSECTOR || nsect > MEMBUFSZ / SECTORSZ) return;
  setmem(IOBUF,SECTORSZ,0);
  IOBUF[0] = DELETED;
  if (seek(DBFD,s0,0) < 0) exit();
  for(i=0;i<nsect;i++)
    write(DBFD,IOBUF,1);
  }



			/* Open a hashtable */
openhtab(htab)
 struct hashtable *htab;
 {int *ibuf;
  char filbuf[MAXFNSZ];
  if ((htab->fd = open(mkpath(DBDIR,htab->filename,filbuf),2)) < 0)
    exit(err("Cannot open file %s",filbuf));
  read(htab->fd,IOBUF,1);
  ibuf = IOBUF;
  htab->nextfree = ibuf[0]; 
  htab->sectorno = -1;
  return;
  }

			/* Close a hashtable */
closehtab(htab)
 struct hashtable *htab;
 {int i,*ibuf;
  setmem(IOBUF,SECTORSZ,0);
  ibuf = IOBUF;
  ibuf[0] = htab->nextfree;
  if (seek(htab->fd,0,0) < 0) exit();
  write(htab->fd,IOBUF,1);
  close(htab->fd);
  } 

			/* Update the disk version of the data base */
updatedb(htab)
 struct hashtable *htab;
 {closedb();
  closehtab(htab);
  openhtab(htab);
  NENTRIES = 0;
  }



mkmemid(datlst,memid0)
  char *datlst[MAXNLFLDS],memid0[MEMIDSZ];
  {int i,pos;
   char *cp;
   for(i=0;i<MEMIDSZ;i++)
     {cp = datlst[SYS.memberid[i].idfld];
      pos = SYS.memberid[i].idpos;
      memid0[i] = (pos < strlen(cp)) ? tolower(cp[pos]) : ' ';
      }
   return(memid0);
   }


getmemid(memid0,htab,helpid,helpfile)
 int helpid;
 struct hashtable *htab;
 char memid0[MEMIDSZ],*helpfile;
 {int i;
  char buffer[MAXTAGSZ]; 
  if (getstr(SYS.memidmsg,buffer,MEMIDSZ+1,helpid,helpfile) == ESC) 
     return(ESC);
  for(i=0;i<MEMIDSZ;i++)
    memid0[i] = (i < strlen(buffer)) ? tolower(buffer[i]) : ' ';
  return(0);
  }

