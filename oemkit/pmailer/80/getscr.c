#include "constant.inc"
#include "ascii.inc"
#include "accind.inc"
#include "sys.inc"
#include "db.inc"
     
				    /* Allow user to enter member fields */
getscr(datlst,datlst0,accopt,helpid,helpfile)
   char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS];
   char accopt,helpid,*helpfile;
   {int i;      
    char c,savopt,chngflg,buffer[SCREENWD];
    dispfrm();
    fill0(datlst);
    for(i=chngflg=0;;)
       {switch(c=getfld(datlst,i,&chngflg))
	   {case CTLO:  
		if (accopt == ADDMEM) break;
	 	return(CTLO);
	    case ESC:   
		if (savmenu("Member",&savopt,chngflg,6,HELPFILE) != ESC)
		   return(savopt);
		dispfrm();
		fill0(datlst);
	        break;
	    case CTLE:			    
		setmode(CLEAR);
		datcpy(datlst0,datlst);
		return(SAVE);
	    case CTLT:
		if (FIELD[i].tmpltno != DATE) break;
	        i -= FIELD[i].tmpltpos;
		strcpy(datlst[i],TODAY.month);
		strcpy(datlst[i+1],TODAY.day);
		strcpy(datlst[i+2],TODAY.year);
		fill0(datlst);
	  	chngflg = 1;
		break;
	    case CTLC:
		strcpy(datlst[i],datlst0[i]);
		filldat(datlst,i);
		chngflg = 1;
	  	if (i+1 < SYS.nlflds) i++;
		else	          i = 0;
		break;
	    case CTLS:
		getserial(buffer);
		if (strlen(buffer) > FIELD[i].length) break;
		strcpy(datlst[i],buffer);
		filldat(datlst,i);
 		nextserial();
		chngflg = 1;
		break;
	    case CTLQ:
		help(helpid,helpfile);
	    case CTLX:
	  	dispfrm();
		fill0(datlst);
		break;
 	    case CTLP:
		prmbr(datlst);
		dispfrm();
		fill0(datlst);
		break;
	    case LF:
      	    case TAB:   
	    case CR:
	  	if (i+1 < SYS.nlflds) i++;
		else	          i = 0;
		break;
	    case UP:
	    case CTLB:
		if (i > 0) i--; 
		else       i = SYS.nlflds-1;
		break;
	    }
        }
   }



				/* Print the member on the printer */
prmbr(datlst)
  char *datlst[MAXNLFLDS];
  {int i,j,ifld,fpos;
   char c;
   setmode(CLEAR);
   setmode(HDCPYON);
   for(i=0;i<SCREENHT;i++)
     {for(j=0;j<SCREENWD;j++)
        {if ((c = SCREEN[i][j]) == 0) break;
         if (c >= ' ') putchar(c);
         else          
	    {ifld = fndfld(i,j,&fpos);
	     setmode(BOLDDSP);
	     if (strlen(datlst[ifld]) > fpos) putchar(datlst[ifld][fpos]);
	     else			      putchar(' ');
	     setmode(NORMALDSP);
             }
         }
      if (i < SCREENHT-1) putchar('\n');
      }
   printf("\n\n");
   setmode(HDCPYOFF);
   } 





				/* Find which field is at screen position
				     i,j; return position in field as fpos */
fndfld(i,j,fpos)
  int i,j,*fpos;
  {int ifld,jj;
   for(ifld=0;ifld<SYS.nlflds;ifld++)
     if (FIELD[ifld].ipos > i || 
	(FIELD[ifld].ipos == i && FIELD[ifld].jpos > j)) break;
   ifld--;
   *fpos = 0;
   for(jj=FIELD[ifld].jpos;jj<j;jj++)
      if (SCREEN[i][jj] < ' ') (*fpos)++;
   return(ifld);
   }





				/* Copy data from datlst to datlst0 */
datcpy(datlst0,datlst)
 char *datlst0[MAXNLFLDS],*datlst[MAXNLFLDS];
 {int i;
  for(i=0;i<SYS.nlflds;i++)
    strcpy(datlst0[i],datlst[i]);
  }


				/* Allow user to enter data field ifld */
getfld(datlst,ifld,chngflg)
  char *datlst[MAXNLFLDS],*chngflg;
  int ifld;
  {int i,j,d,m;
   char c,*data;
   data = datlst[ifld];
   i=FIELD[ifld].ipos; j=FIELD[ifld].jpos;
   for(d=0;;)
     {while(SCREEN[i][j] >= ' ') ++j;
      pos(i,j);
      switch(c=getchar())
       {case CTLO:  	
        case CTLP:
        case CTLQ: 
   	case CTLX:
	case CTLC:
	case CTLS:
	case CTLT:
	case ESC:	
	   return(c);
	case CTLE:
	case CTLB:	
	case CR:	
	case TAB:    
	case LF:
	case UP:
	   if (nchkfld(datlst,ifld) < 0) break;
	   return(c);
        case BACK:
	   if (d>0) 
	    {pos(i,--j);
 	     while(SCREEN[i][j] >= ' ') pos(i,--j);
	     d--;
	     }
	   break;
	case FORWARD:	
	   if (d<FIELD[ifld].length-1)
	    {j++;
	     d++;
	     }
	   break;
	default:
	   if (c < 32) break;
	   if (FIELD[ifld].type == STATUS) 
	      {if (c == ' ')
	          {*data = 0;
		   putch(' ');
		   }
		else
		  {putch(*data = 'x');
		   *(data+1) = 0;
		   }
	        *chngflg = 1;
		return(TAB);
		}
	    if (chktyp(c,SCREEN[i][j],d) < 0)
	       {putch(BELL);
		break;
		}
 	    if (data[d] == 0) data[d+1] = 0;
	    for(m=0;m<d;m++) if (data[m] == 0) data[m] = ' ';
  	    data[d] = putch(c);
	    if (d < FIELD[ifld].length-1) 
		{d++;
		 j++;
		 }
	    *chngflg = 1;
        }
     }
 }



		/* Check that char c is of type typ */
chktyp(c,typ,fldpos)  	
  char c,typ;  
  int fldpos;
  {if (c == ' ') return(1);
   if (typ == NUMERIC)
     {if (isdigit(c) || (fldpos == 0  && c == '-')) return(1);
      return(-1);
      }
   if (typ == ALPHA && isdigit(c) != 0)   return(-1);
   return(1);
   }


		  /* Check to see if field ifld is within numeric bounds */
nchkfld(datlst,ifld)
  char *datlst[MAXNLFLDS];
  int ifld;
  {int n,min,max;
   if (*datlst[ifld] == 0) return(0);
   if (FIELD[ifld].type != NUMERIC) return(0);
   if (FIELD[ifld].length > 4) return(0);
   n = atoi(datlst[ifld]);
   min = FIELD[ifld].minval;
   max = FIELD[ifld].maxval;
   if (n >= min && n <= max) return(0);
   setmode(CLEAR);
   if (n > max) err("Maximum value for this field is %d",max);
   else	        err("Minimum value for this field is %d",min);
   *datlst[ifld] = 0;
   dispfrm();
   fill0(datlst);
   return(-1);
   }

