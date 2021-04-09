#include "gendb.inc"

main(argc,argv)
 int argc;
 char **argv;
 {char opt,dbchngflg;
  struct terminal term;
  TERM = &term;
  initgendb();
  if (argc == 2) 
    strcpy(DBDIR,argv[1]);
  else 		 
    if (gettok("data base disk drive",DBDIR,MAXDIRSZ,0,HELPFILE) == ESC) 
	exit();
  while(1)
    {if (menu("Data Base Generation",
	"Create New Data Base,Modify Existing Data Base,\
Re-Initialize Data Base Subsystem",
	&opt,1,HELPFILE) == ESC) exit();
     switch(opt)
       {case 0:
  	  zerodb();   
	  if (getstr("data base name",SYS.dbname,MAXDESCSZ,
		19,HELPFILE) == ESC) break;
	  if (getdb(&dbchngflg) == DISCARD) break;
	  printf("WARNING: The database on disk %c ",*DBDIR);
	  printf("is about to be initialized.");
          printf("\nAny existing data there will be lost.");
	  printf("\nTo Abort now, hit ESCape;");
	  printf(" to continue hit any other character%c",BELL);
	  if (getchar() == ESC) break;
	  setmemid();
	  wrtimg();
	  dbinit();
          err0("Data Base Written");
	  break;
	case 1:
	  if (rdimg() < 0) break;
	  if (getdb(&dbchngflg) == DISCARD) break;
	  if (dbchngflg == 0) 
	    {wrtimg();
	     break;
	     }
	  printf("\nWARNING: \
The database on disk %c is about to be initialized.",*DBDIR);
	  printf("\nAny existing data there will be lost.");
	  printf("\nIf you want to use any of the data there again, ");
	  printf("you should have a backup copy.");
	  printf("\nTo Abort now, hit ESCape;");
	  printf(" to continue hit any other character%c",BELL);
	  if (getchar() == ESC) break;
	  setmemid();
	  wrtimg();
	  dbinit();
	  printf("\nNew Data Base Generated.");
	  printf("\nTo copy the old data base to the new one,");
	  printf(" use program \"movedb\".");
	  err0("");
	  break;
	case 2:
	  reinit();
	  break;
 	}
     }
  }




reinit()
 {char opt;
  printf("WARNING: Re-initialization will destroy existing information.");
  err0("Be sure you have a backup of any data you wish to use again");
  while(1)
    {if (menu("Select Subsystem to Re-Initialize",
	"Members,Subsets,List Formats,Mail Formats",&opt,2,HELPFILE) == ESC) 
      return;
     switch(opt)
        {case 0:
          if (rdimg() < 0) break;
	  dbcreat();	
	  htabcreat();
	  break;
	case 1: 
	  sscreat();
	  break;
	case 2:
	  lfcreat();
	  break;
	case 3:
	  mfcreat();
	  break;
	}
     }
  }




	  printf(