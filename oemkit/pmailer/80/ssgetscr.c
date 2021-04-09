#include "constant.inc"
#include "ascii.inc"
#include "sys.inc"
#include "subset.inc"
#include "db.inc"

#define HELPFILE "defss.hlp"

ssgetscr(datlst,relop,chngflg,helpid,helpfile)
  char *datlst[MAXNLFLDS],relop[MAXNLFLDS];
  char helpid,*helpfile,*chngflg;
  {int i,k;
   char c,op;
   dispfrm();
   fill0(datlst);
   for(i=0;;)
      switch(c  = getssfld(datlst,relop,i,chngflg))
        {case ESC:
	 case CTLE:
	    return(c);
	 case CTLQ:
	    help(helpid,helpfile);
	 case CTLX:
	    dispfrm();
	    fill0(datlst);
	    break;
	 case CTLR:
	   if (menu(
	     "Select the relationship of the subset to the data being entered",
	     RELOPS,&op,9,helpfile) != ESC) relop[i] = op;
	   *chngflg = 1;
	   dispfrm();
 	   fill0(datlst);
	   break;
	 case CTLT:
	   if (FIELD[i].tmpltno != DATE) break;
           i -= FIELD[i].tmpltpos;
  	   strcpy(datlst[i],"To");
 	   strcpy(datlst[i+1],"da");
	   strcpy(datlst[i+2],"y ");
	   setmode(CLEAR);
	   k = 0;  
 	   getnum("offset from current date",&k,-31,31,10,HELPFILE);
	   datlst[i+2][1] = k + 48; 
	   dispfrm();
 	   fill0(datlst);
	   *chngflg = 1;
	   break;
	 case TAB:
	 case CR:
	 case LF:  
	    if (i+1 < SYS.nlflds) i++;
	    else	      i = 0;
	    break;
	 case UP:
	 case CTLB:
	    if (i>0) i--;
	    else     i = SYS.nlflds-1;
 	    break;
         }
   }





getssfld(datlst,relop,ifld,chngflg) 
  char *datlst[MAXNLFLDS],relop[MAXNLFLDS];
  char *chngflg; 
  int ifld;
  {int i,j,d,m;
   char c,*data;
   data = datlst[ifld];
   i = FIELD[ifld].ipos; 
   j = FIELD[ifld].jpos;
   for(d=0;;)
     {while(SCREEN[i][j] >= ' ') ++j;
      pos(i,j);
      switch(c = getchar())
        {case CTLQ:
	 case CTLX:
	 case CTLB:
	 case UP:
	 case CTLE:
	 case ESC:
	 case CR:
	 case TAB:
	 case LF:
	 case CTLT:
	 case CTLR:
	   clrblnks(data);
	   return(c);
	 case BACK:
	   if (d>0) 
	     {pos(i,--j);
	      while(SCREEN[i][j] > ' ') pos(i,--j);
	      d--;
	      }
	   break;
	 case FORWARD:
	   if (d+1 < FIELD[ifld].length)
	     {j++;
	      d++;
	      }
	   break;
	 default:
	   if (c<32) break;
	   if (FIELD[ifld].type == STATUS)
	     {if (c == '0') putch(*data = '0');
	      if (c == ' ') 
		{putch(' ');
		 *data = 0;
		 }
	      if (c > '0')  putch(*data = 'x');
  	      *(data+1) = 0;
	      *chngflg = 1;
	      return(TAB);
	      }
	   if (chksstyp(c,SCREEN[i][j]) < 0 || d == FIELD[ifld].length)
	     {putch(BELL);
	      break;
     	      }
	   if (data[d] == 0) data[d+1] = 0;
	   for(m=0;m<d;m++)
	     if (data[m] == 0) data[m] = ' ';
	   data[d] = putch(c);
	   *chngflg = 1;
	   if (d+1 < FIELD[ifld].length)
	     {d++;
	      j++;
	      }
	 }
      }
   }







chksstyp(c,typ)
 char c,typ;
 {if (c == '!' || c == '*' || c == '?') return(0);
  if (typ == NUMERIC && isdigit(c) == 0 && c != ' ') return(-1);
  if (typ == ALPHA && isdigit(c) != 0) return(-1);
  return(0);
  }



