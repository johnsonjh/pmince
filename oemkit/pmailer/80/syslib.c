#include "constant.inc"
#include "sys.inc"
#include "db.inc"

					/* Read in the system variables */
rdsys(sys)
 struct sysvars *sys;
 {fgetbuf(&sys[0].nlflds,sizeof(*sys));
  }

					/* Write out the system variables */
wrtsys(sys)
 struct sysvars *sys;
 {fputbuf(&sys[0].nlflds,sizeof(*sys));
  }

      
					/* Read in the screen entry form. */
rdscr(screen)	
  char screen[SCREENHT][SCREENWD];
  {int i,j;
   for(i=0;i<SCREENHT;i++)
     for(j=0;j<SCREENWD;j++)
       if ((screen[i][j] = rawfget()) == 0) break;
    }






	  		/* Write out the screen image */
wrtscr()
  {int i;
   char *p0,*p1,*p;
   printf("\nWriting screen image");
   for(i=0;i<SCREENHT;i++)
     {p0 = &SCREEN[i][0]; 
      p1 = &SCREEN[i][SCREENWD-1];
      while(*p1 == ' ' && p1 >= p0-1) p1--;
      for(p=p0;*p && p<=p1;p++) 
        rawfput(*p);
      rawfput(0);  
      }
   }




  					/* Read in the logical lfields */
rdfld(lfield,nlflds)	
 struct logfield lfield[MAXNLFLDS];
 int nlflds;
 {fgetbuf(&lfield[0].type,nlflds * sizeof(lfield[0]));
  }
  


			/* Write out the logical field structure */
wrtfld()	
 {printf("\nWriting logical fields");
  fputbuf(&FIELD[0].type,SYS.nlflds * sizeof(FIELD[0]));
  }









			/* Write out the field tags */
wrttag()
 {int i;
  for(i=0;i<SYS.nlflds;i++)  
     fputstr(FIELD[i].tag);
  }











				/* Make a template tag out of a lfield tag */
mktmtag(tag,buffer)
 char *tag,*buffer;
 {int i0;
  if ((i0 = strmatch(tag,".")) < 0) return(tag);
  strcpy0(buffer,tag,i0);
  buffer[i0] = 0;
  return(buffer);
  }





setmemid()
 {if (FIELD[0].tmpltno == NAME)
     setnameid(0);
  else
     setfldid(0);
  }


setnameid(ifld)
 int ifld;
  {int i;
   SYS.memberid[0].idfld = ifld;
   SYS.memberid[0].idpos = 0;
   for(i=1;i<MEMIDSZ;i++)
     {SYS.memberid[i].idfld = ifld + 2;
      SYS.memberid[i].idpos = i-1;
      }
   strcpy(SYS.memidmsg,"First Initial and First 3 Letters of Last Name");
   }


setfldid(ifld)
 int ifld;
 {int i;
  for(i=0;i<MEMIDSZ;i++)
    {SYS.memberid[i].idfld = ifld;
     SYS.memberid[i].idpos = i;
     }
   strcpy(SYS.memidmsg,"first 4 characters of field ");
   strcat(SYS.memidmsg,FIELD[ifld].tag);
   }








