#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "subset.inc"
#include "term.inc"
#include "db.inc"

#define HELPFILE "setup.hlp"
  
main(argc,argv)
 int argc;
 char **argv;
 {char filbuf[MAXFNSZ];
  printf("Perfect Mailer v1.1\nCopyright (c) 1982 by Perfect Software, Inc.");
  if (rdmem(TERMFILE,*TERM,sizeof(*TERM)) < 0) exit();
  printf("\n%s Terminal",TERM->termdesc);
  *SYS.helpdir = 'x';
  if (argc == 2) 
    strcpy(DBDIR,argv[1]);
  else
    if (gettok("disk drive containing data base",DBDIR,MAXDIRSZ,
	2,HELPFILE) == ESC) exit();
  chdir(DBDIR);
  if (fopen0(DEFFILE,FREAD) < 0) exit();
  rdsys(&SYS);
  printf("\n%s",SYS.dbname);
  rdfld(FIELD,SYS.nlflds);
  rdtag(FIELD,TAGBUF,SYS.nlflds);
  rdscr(SCREEN);
  fclose();
  if ((DBFD = opendb(DBDIR,&NEXTSECTOR)) < 0) exit();
  setversion();
  initdate();
  chkdate();
  }
  
  
  
  
  					/* Initialize the date.
  					   Not a very pretty function */
  
initdate()
 {int i,k;
  strcpy(MNBUF,"January");
  strcpy(MNBUF+8,"February");
  strcpy(MNBUF+17,"March");
  strcpy(MNBUF+23,"April");
  strcpy(MNBUF+29,"May");
  strcpy(MNBUF+33,"June");
  strcpy(MNBUF+38,"July");
  strcpy(MNBUF+43,"August");
  strcpy(MNBUF+50,"September");
  strcpy(MNBUF+60,"October");
  strcpy(MNBUF+68,"November");
  strcpy(MNBUF+77,"December");
  for(i=k=0;i<12;i++)
    {MNSTR[i] = &MNBUF[k]; 
     while(MNBUF[k++]);
     }
  initb(DAYSPERMN,"31,28,31,30,31,30,31,31,30,31,30,31");
  } 
  
  
  
chkdate()
 {char mesg[MAXDESCSZ],buf[MAXDESCSZ],opt;
  while(1)
    {rddate();
     strcpy(mesg,"Current Date = ");
     strcat(mesg,MNSTR[atoi(TODAY.month)-1]);
     ccat(mesg,' ');
     strcat(mesg,TODAY.day);
     strcat(mesg,", 19");
     strcat(mesg,TODAY.year);
     if (menu(mesg,"Date Correct,Change Date",&opt,0,HELPFILE) == ESC) 
       exit();
     if (opt == 0) return;
     getdate();
     }
  }


getdate()
  {int month0,day0,year0; 
   printf("Enter Today's Date: %c",BELL);
   if (getnum("month",&month0,1,12,1,HELPFILE) == ESC) return;
   if (getnum("day",&day0,1,31,1,HELPFILE) == ESC) return;
   if (getnum("year",&year0,80,88,1,HELPFILE) == ESC) return;  
   itoa(TODAY.month,month0);
   itoa(TODAY.day,day0);
   itoa(TODAY.year,year0);
   wrtdate();
   }



rddate()
 {int k;
  seek(DBFD,0,0);
  read(DBFD,IOBUF,1);
  itoa(TODAY.month,k = IOBUF[2]);
  itoa(TODAY.day,k = IOBUF[3]);
  itoa(TODAY.year,k = IOBUF[4]);
  }
  



wrtdate()
 {seek(DBFD,0,0);
  read(DBFD,IOBUF,1);
  IOBUF[2] = atoi(TODAY.month);
  IOBUF[3] = atoi(TODAY.day);
  IOBUF[4] = atoi(TODAY.year);  
  seek(DBFD,0,0);
  write(DBFD,IOBUF,1);
  }




cat(