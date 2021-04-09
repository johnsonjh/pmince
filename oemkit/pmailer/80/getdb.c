#include "ascii.inc"
#include "gendb.inc"


getdb(dbchngflg)
  char *dbchngflg;
  {char opt,sopt,chngflg,options[SYSBUFSZ];
   strcpy(options,"Define Data Display Screen,Display Screen Definition,");
   strcat(options,"Set System Parameters,Save Data Base");
   for(*dbchngflg = chngflg = 0;;)
     {if (menu(SYS.dbname,options,&opt,3,HELPFILE) == ESC) 
         {if (savmenu("Data Base Definition",&sopt,*dbchngflg || chngflg,
  	    4,HELPFILE) != ESC) return(sopt);
          }
      else switch(opt)
        {case 0:
  	   genscr(&chngflg,dbchngflg,5,HELPFILE);
  	   break;
 	 case 1:
  	   prdbtxt(CRTON);
  	   break;
	 case 2:
  	   setsys(&chngflg,dbchngflg);
  	   break;
   	 case 3:
	   return(SAVE);
  	 }
      }
   }


					/* Allow user to define entry screen */
genscr(chngflg,dbchngflg,helpid,helpfile) 
  char *chngflg,*dbchngflg;
  char helpid,*helpfile;
  {int i0,j0,i,j;
   char c;
   i0 = j0 = 0;
   dispscr();
   while(1)
      {pos(i0,j0);
       switch(c=getchar())
	  {case CR:
	     j0 = 0;
	     if (i0 < (SCREENHT-1)) i0++; 
	     else 		    i0 = 0;
	     break;
	   case CTLB:
	   case BACK:
	     if (j0 > 0) j0--;
	     else        j0 = SCREENWD-2;
	     break;
	   case CTLF:
	   case FORWARD:
	     if (j0 < SCREENWD-2) j0++;
	     break;
	   case CTLN:
	   case LF:   
	     if (i0 < SCREENHT-1) i0++;
	     else 		  i0 = 0;
	     break;
	   case CTLP:
	   case UP:
	     if (i0 > 0) i0--;
	     else 	 i0 = SCREENHT-1;
	     break;
	   case ESC:
	     return;
	   case CTLQ:
	     help(helpid,helpfile);
	   case CTLX:
 	     dispscr();
	     break;
	   case TAB:
	     *dbchngflg = 1;
  	     putfld(i0,j0);
	     dispscr();
  	     break;
	   case CTLT:
	     *dbchngflg = 1;
	     puttmplt(i0,j0);
	     dispscr();
	     break;
	   case CTLS:
	     *dbchngflg = 1;
             delelem(i0,j0);
	     dispscr();
	     break;
	  case CTLD:
	     *chngflg = 1;
	     dellin(i0);
	     dispscr();
	     break;
	  case CTLA:
	     *chngflg = 1;
	     inslin(i0);
	     dispscr();
	     break;
	  default:
      	     if (c < ' ' || SCREEN[i0][j0] < ' ')
		{putch(BELL);
		 break;
		 }
	     *chngflg = 1;
             setmode(HALFDSP);
	     SCREEN[i0][j0++] = putch(c);
	     if (j0 == SCREENWD) 
		{j0 = 0; 
		 i0++;
		 }
	     break;
	  }   
      }
   }

