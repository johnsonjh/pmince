#include "constant.inc"
#include "term.inc"
#include "iovars.inc"


/*
			-----------------------------
			| Terminal Access Functions |
			-----------------------------
*/


				/* Position cursor at line i, column j */
pos(i,j)
 int i,j;
 {char *str;                                   
  i += TERM->ioffset;
  j += TERM->joffset;
  str = TERM->cursaddr;
  for(;*str;str++)
    {if (*str == XCOORDINATE) putch(j);
      else if (*str == YCOORDINATE) putch(i);
       else putch(*str);
     }
  delay(TERM->cursdelay);
  }




				/* Set terminal mode */
setmode(mode)
char mode;
 {switch(mode)
     {case DELETE:
	setmode(CRSLEFT);
	putch(' ');
	setmode(CRSLEFT);
	return;
      case CRSLEFT:
	putscr(TERM->cursleft);
	return;
      case CLEAR:
	putscr(TERM->clear);
        delay(TERM->cleardelay);
	return;
      case NORMALDSP:
	putscr(TERM->normaldsp);
	UNDERSW = 0;
	return;
      case UNDERDSP:
	putscr(TERM->underline);
	UNDERSW = 1;
	return;
      case HALFDSP:
	putscr(TERM->halfdsp);
	return;
      case HDCPYON:  
	HDCPYSW = 1;
	pput(CR);
	return;
      case HDCPYOFF:   
	HDCPYSW = 0;
	return;
      case FILEON:
	FILESW = 1;
	return;
      case FILEOFF:
	FILESW = 0;
	fclose();
	return;
      }
  }




delay(n)
 int n;
 {int i;
  for(i=0;i<n;i++) putch(0);
  }
