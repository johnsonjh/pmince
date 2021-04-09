#include "constant.inc"
#include "sys.inc"
#include "db.inc"

#define HELPFILE "main.hlp"

				/* Present the main system menu and
				    execl selected subsystem */
sysmenu()
 {char opt,buffer[SECTORSZ],buf2[MAXDESCSZ];
  int (*ptrfn)();
  ptrfn = OVERLAY;
  while(1) 
    {strcpy(buffer,"Access Individual Members,Generate List/Report,");
     strcat(buffer,"Generate Mail,Define Subset,Define List Format,");
     strcat(buffer,"Define Printer Form");
     if (menu(SYS.dbname,buffer,&opt,0,HELPFILE) == ESC) 
	{fabort(DBFD);
         chdir(SYS.progdir);
         exit();
	 }
     printf("Loading module %s",
	extropt(opt,"accind,list,mail,defss,deflform,defpform",buf2));  
     if (swapin(mkpath(SYS.progdir,buf2,buffer),OVERLAY) < 0) exit();
     (*ptrfn)();
     }
  }



