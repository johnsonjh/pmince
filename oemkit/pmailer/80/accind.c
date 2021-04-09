#include "constant.inc"
#include "ascii.inc"
#include "sys.inc"
#include "accind.inc"
#include "htab.inc"
#include "db.inc"
#define HELPFILE "accind.hlp"

main()   
 {struct hashtable htab;
  initacc(&htab);
  accind(&htab);
  fabort(htab.fd);
  }


				/* Access an individual member */
accind(htab)
 struct hashtable *htab;
 {char mem[MEMBUFSZ],mem0[MEMBUFSZ];
  char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS],memid0[MEMIDSZ],accopt,c;
  int entryno,n;
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  lnkdat(mem0,datlst0,FIELD,SYS.nlflds);
  zero(datlst0);
  while(1)
    {if (menu("Access Individual Member",
	  "Add a New Member,Review/Update Existing Member,Delete Member",
	   &accopt,0,HELPFILE) == ESC) 
              {closehtab(htab);
	       closedb();
	       return;
	       }
      switch(accopt) 
       {case 0:			/* Add a member */
	   while(1)
             {zero(datlst);
	      setdate(datlst);
	      setserial(datlst);
              if (getscr(datlst,datlst0,ADDMEM,1,HELPFILE) == SAVE) 
                 {wrtmbr(datlst,NEWENTRY,htab);
	          if (strcmp(FIELD[0].tag,"serialno") == 0) nextserial();
		  }
	      else break;
	      }
    	   break;
        case 1:			/* Review or revise a member */
           if (getmemid(memid0,htab,2,HELPFILE) == ESC) break;
           for(n=0;;n++)
             {if ((entryno = fndentry(memid0,htab,n)) < 0) break;
              rdmbr(datlst,entryno,htab);
              if ((c=getscr(datlst,datlst0,REVMEM,3,HELPFILE)) == SAVE) 
		resave(datlst,memid0,entryno,htab);
   	      if (c != CTLO) break;
   	      }
	   break;
        case 2:			/* Delete a member */
	   if (getmemid(memid0,htab,4,HELPFILE) == ESC) break;
           for(n=0;;n++)
             {if ((entryno = fndentry(memid0,htab,n)) < 0) break;
	      printf("\nYou will be shown a data display screen--");
	      err0("to delete the member displayed type Control D");
              rdmbr(datlst,entryno,htab);
	      if ((c = getdel(datlst,5,HELPFILE)) == CTLD)
		 deletemem(entryno,htab);
	      if (c != CTLO) break;
	      }
	   break;
        }
     }
  }




				/* Re-save old member to disk; delete and make
				    new entry if member ID has changed.
				    (Needed only to provide for misspelled
				     Member ID's)  */
resave(datlst,memid0,entryno,htab)
 char *datlst[MAXNLFLDS],memid0[MEMIDSZ];
 int entryno;
 struct hashtable *htab;
 {char memidbuf[MEMIDSZ];
  if (strcmp0(memid0,mkmemid(datlst,memidbuf),MEMIDSZ) == 0)
    {deletemem(entryno,htab);
     wrtmbr(datlst,NEWENTRY,htab);
     return;
     }
  wrtmbr(datlst,entryno,htab);
  }













  
			/* Get the user's response on deletion request */
getdel(datlst,helpid,helpfile)
 char *datlst[MAXNLFLDS],helpid,*helpfile;
 {char c;
  while(1)
    {dispfrm();
     fill0(datlst);
     switch(c = getchar())
       {case ESC:
        case CTLD:
        case CTLO:
	  return(c);
        case CTLQ:
	  help(helpid,helpfile); 
        }
      }
   }




					/* Initialization */
initacc(htab)
  struct hashtable *htab;
  {NENTRIES = 0;
   BACKUPFLAG = 1;
   strcpy(htab->filename,HTABFILE);
   htab->nbuckets = SYS.nbucket0;
   openhtab(htab);
   }

 

					/* Set the serial number */
setserial(datlst)
 char *datlst[MAXNLFLDS];
 {char serialno[SCREENWD];
  if (strcmp(FIELD[0].tag,"serialno") == 0) getserial(datlst[0]);
  }


getserial(serialno)
 char *serialno;
 {int fd;
  if ((fd = open(SERIALFILE,0)) < 0)
    {strcpy(serialno,"1"); 
     wrtmem(SERIALFILE,serialno,strlen(serialno));
     }
  else
    {close(fd);
     rdfil(SERIALFILE,serialno,MEMIDSZ+1);  
     }
  }

					/* Increment the serial number */ 
nextserial()
 {char serialno[SCREENWD];
  rdfil(SERIALFILE,serialno,MEMIDSZ+1);  
  itoa(serialno,atoi(serialno)+1);
  wrtmem(SERIALFILE,serialno,strlen(serialno));
  }
