#include "gendb.inc"
#include "help.inc"

initgendb()
  {int i;
   initio();
   *SYS.helpdir = 'x';
   printf("Gendb v3.1\nCopyright (c) 1982 by Perfect Software, Inc.");
   if (rdmem(TERMFILE,*TERM,sizeof(*TERM)) < 0) exit();
   printf("\n%s Terminal",TERM->termdesc);
   if (rdtmplt() < 0) exit();
   for(i=0;i<MAXNLFLDS;i++)
     FIELD[i].tag = TAG[i];
   zerodb();
   setversion();
   }


zerodb()
  {int i,j;
   SYS.nlflds = 0;
   setpar();
   for(i=0;i<SCREENHT;i++)
     for(j=0;j<SCREENWD;j++)
       SCREEN[i][j] = ' ';
   }


				/* Set default parameters */
setpar()
 {strcpy(SYS.progdir,PROGDIR0);
  strcpy(SYS.sortdir,SORTDIR0);
  strcpy(SYS.helpdir,HELPDIR0);
  SYS.nbucket0 = nextprime(DBSIZE0); 
  SYS.dirbakno = DIRBAKUP0;
  }


				/* Display the screen */
dispscr()	
  {int i,j;
   char *p0,*p1,*p,mode;
   setmode(CLEAR);
   setmode(mode = HALFDSP);
   for(i=0;i<SCREENHT;i++)
     {p0 = &SCREEN[i][0]; 
      p1 = &SCREEN[i][SCREENWD-1];
      while(*p1 == ' ' && p1 >= p0-1) p1--;
      for(p=p0;p<=p1;p++) 
        {if (*p < ' ') 
           {if (mode  == HALFDSP) setmode(mode = NORMALDSP);
            putch('_');
            }
         else
           {if (mode == NORMALDSP) setmode(mode = HALFDSP);
            putch(*p);
            }
         }
      if (i < SCREENHT-1) {putch(CR); putch('\n');}
      }
   }





		  /* Delete a member structure element */
delelem(i0,j0)    
  int i0,j0;
  {int ifld,nflds;
   if (SYS.nlflds == 0) 
     {err0("No fields have been defined");
      return;
      }
   if (SCREEN[i0][j0] >= ' ')
     return(err0("No Field Here"));
   ifld = fndfld(i0,j0);
   if (FIELD[ifld].tmpltno != NOTEMPLATE)
     ifld -= FIELD[ifld].tmpltpos;
   nflds = fldct(ifld);
   delscr(ifld,nflds);
   delfld(ifld,nflds);
   SYS.nlflds -= nflds;
   }



		/* Return the number of the last logical field which 
		   begins on or before i0,j0 */
fndfld(i0,j0)
 int i0,j0;
 {int ifld;
  for(ifld=0;ifld<SYS.nlflds;ifld++)
    if (FIELD[ifld].ipos > i0 ||   
        (FIELD[ifld].ipos == i0 && FIELD[ifld].jpos > j0))
      return(ifld-1);
  return(SYS.nlflds-1);
  }





			/* Count the number of fields in the element
			     starting at field ifld */
fldct(ifld)
 int ifld;
 {int itmplt;
  if ((itmplt = FIELD[ifld].tmpltno) == NOTEMPLATE) return(1);
  return(TMPLTLST.templat[itmplt].nfields);
  }
 


		/* Delete nfields from the screen beginning at ifld */
delscr(ifld,nflds)
 int ifld,nflds;
 {int i0,j0,k,ndat;
  for(k=ifld;k<ifld+nflds;k++)
    {i0 = FIELD[k].ipos; j0 = FIELD[k].jpos;
     for(ndat=0;ndat<FIELD[k].length;)
        {if (SCREEN[i0][j0] < ' ') ndat++;
	 SCREEN[i0][j0++] = ' ';
	 }
     if (FIELD[k].type == STATUS) SCREEN[i0][j0] = ' ';
     }
  }


		/* Delete nflds from the FIELD structure beginning at ifld */
delfld(ifld,nflds)
 int ifld,nflds;
 {int i;
  for(i=ifld;i<SYS.nlflds-nflds;i++)
    {fldcpy(&FIELD[i],&FIELD[i+nflds]);
     strcpy(TAG[i],TAG[i+nflds]);
     }
  }


		/* Insert nflds in the FIELD structure after ifld */
insfld(ifld,nflds)
 int ifld,nflds;
 {int i;
  for(i=SYS.nlflds-1;i>ifld;i--)
     {fldcpy(&FIELD[i+nflds],&FIELD[i]);
      strcpy(TAG[i+nflds],TAG[i]);
      } 
  }
    









		/* Copy logical field fld2 to fld1; do not copy tag pointer */
fldcpy(fld1,fld2)
 struct logfield *fld1,*fld2;
 {strcpy0(fld1,fld2,&fld1[1].type-&fld1[0].type-2);
  }


		/* Delete line i0 */
dellin(i0)
 int i0;
 {int i,j;
  for(j=0;j<SCREENWD;j++)
    if (SCREEN[i0][j] < ' ')
      return(err0("Must delete all fields on line before deleting line"));
  for(i=i0;i<SCREENHT-1;i++)
    strcpy0(SCREEN[i],SCREEN[i+1],SCREENWD);
  for(j=0;j<SCREENWD;j++)
    SCREEN[SCREENHT-1][j] = ' ';
  for(i=0;i<SYS.nlflds;i++)
    if (FIELD[i].ipos > i0) FIELD[i].ipos--;
  }


			/* Insert a line before i0 */
inslin(i0)
 int i0;
 {int i,j;
  if (i0 > 0)        
    if (intmplt(i0-1,SCREENWD-1))
      return(err0("Cannot insert line inside a template"));
  for(j=0;j<SCREENWD;j++)
    if (SCREEN[SCREENHT-1][j] < ' ')
       return(
	err0("Last line cannot contain any fields when a line is inserted"));
   for(i=SCREENHT-1;i>i0;i--)
     strcpy0(SCREEN[i],SCREEN[i-1],SCREENWD);
   for(j=0;j<SCREENWD;j++)
      SCREEN[i0][j] = ' ';
   for(i=0;i<SYS.nlflds;i++)
     if (FIELD[i].ipos >= i0) FIELD[i].ipos++;
   }


			/* Check to see if i0,j0 is inside of a template */
intmplt(i0,j0)
 int i0,j0;
 {int ifld,itmplt,ndat;
  if ((ifld = fndfld(i0,j0)) < 0) return(0);
  if ((itmplt = FIELD[ifld].tmpltno) == NOTEMPLATE) return(0);
  if (FIELD[ifld].tmpltpos < TMPLTLST.templat[itmplt].nfields-1) return(1);
  return(infld(i0,j0,ifld));
  }

			/* Check to see if i0,j0 is inside field ifld */
infld(i0,j,ifld)
 int i0,j,ifld;
 {int j0,ndat; 
  if (i0 != FIELD[ifld].ipos) return(0);
  j0 = FIELD[ifld].jpos;
  if (j < j0) return(0);
  for(ndat=0;ndat<FIELD[ifld].length;)
    if (SCREEN[i0][j0++] < ' ') ndat++;
  if (j >= j0) return(0);
  return(1);
  }







       			/* Find the total length of the first n tags */
taglen(n)
 int n;
 {int i,len;
  for(i=len=0;i<n;i++)
    len += strlen(FIELD[i].tag) + 1;
  return(len);
  }








				/* Check to make sure tag has not been used */
chktag(tag)
 char *tag;
 {int i;
  char tagbuf[MAXTAGSZ];
  if (chktmtag(mktmtag(tag,tagbuf))) return(-1);
  for(i=0;i<SYS.nlflds;i++)
    if (strcmp(tag,mktmtag(TAG[i],tagbuf)) == 0)
      return(err("Tag %s has already been used in member definition.",tag));
  return(0);
  }




			/* Allow the user to adjust defaults */
setsys(chngflg,dbchngflg)
 char *chngflg,*dbchngflg;
 {char opt,options[SYSBUFSZ];
  strcpy(options,"Display System Parameters,Set Program Directory,");
  strcat(options,"Set Sort Directory,Set Help Directory,");
  strcat(options,"Set Expected Data Base Size,Set Data Base Backup Interval,");
  strcat(options,"Set Data Base Name");
  while(1)
    {if (menu("System Parameter Definition",options,
      	&opt,12,HELPFILE) == ESC) return;
     switch(opt)
       {case 0:
	   dispsys();
	   err0("");
	   break;
	case 1:
	   getstr("program directory",SYS.progdir,MAXFNSZ,13,HELPFILE);
	   *chngflg = 1;
	   break;
	case 2:
	   getstr("sort directory",SYS.sortdir,MAXFNSZ,14,HELPFILE);
	   *chngflg = 1;
	   break;
	case 3:
	   getstr("help directory",SYS.helpdir,MAXFNSZ,15,HELPFILE);
	   *chngflg = 1;
	   break;
     	case 4:
	   if (getnum("maximum number of members expected",&SYS.nbucket0,
		100,MAXDBSZ,16,HELPFILE) == ESC) break;
	   *dbchngflg = 1;
	   SYS.nbucket0 = min(SYS.nbucket0,3000);
           SYS.nbucket0 = nextprime(SYS.nbucket0);
	   break;
        case 5:  
           getnum("data base update interval",&SYS.dirbakno,
	 	1,MAXDBSZ,17,HELPFILE);
	   *chngflg = 1;
	   break;
	case 6:
	  getstr("data base name",SYS.dbname,MAXDESCSZ,18,HELPFILE);
	  *chngflg = 1;
	  break;
        }
     }
   }  
  




 				/* Find the next prime after n */
nextprime(n)
 int n;
 {while(n++)
    if (isprime(n)) return(n);
  }

isprime(n)
  int n;
  {int i,n2;
   n2 = n/2;
   for(i=2;i<n2;i++)
     if (n%i == 0) return(0);
   return(1);
   }





				/* Display the system parameters */
dispsys()
  {printf("\n\n\n\nIII. SYSTEM PARAMETERS:");
   printf("\nProgram Directory = %s",SYS.progdir);
   printf("\nSort Directory = %s",SYS.sortdir);
   printf("\nHelp Directory = %s",SYS.helpdir);
   printf("\nExpected Data Base Size [adjusted] = %d",SYS.nbucket0);
   printf("\nData Base Update Interval = %d",SYS.dirbakno);
   printf("\nData Base Name = %s",SYS.dbname);
   }











xtprime(SYS.nbucket0);
	   break;
        case 5:  
           getnum("data base update inte