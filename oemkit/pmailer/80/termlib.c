#include "constant.inc"
#include "ascii.inc"
#include "iovars.inc"


				/* Print string on the screen only */
putscr(string)
 char *string;
 {while(*string) putch(*string++);
  }


   

 


			/* Increment the system left margin by mrginc */
setmargin(marginc)
 int marginc; 
 {MARGIN += marginc;
  }

			/* Put a string to the output */
puts(string)
 char *string;
 {while(*string)
    putchar(*string++);
  }



				/* Print a character */
putchar(c)
 char c;
 {int i;
  char bold0,under0;
  if (c == '\n') putchar(CR);
  putch(c);
  pput(c);
  if (c == '\n') 
    {CARRIAGEPOS = 0;
     putspace(MARGIN);
     }
  else  
     CARRIAGEPOS++;
  return(c);
  }

				/* Put n spaces */
putspace(n)
 int n;
 {while(n--) putchar(' ');
  }





			/* Process a keyboard entry made while output is
			    being generated */
procchar()
 {if (kbhit() == 0) return(0);
  switch(getchar())
    {case ESC:
	return(ESC);
     case CTLS:
	return(getchar());                    
     }
  }








				/* Initialize i/o flags */
initio()
 {HDCPYSW = FILESW = MARGIN = CARRIAGEPOS = 0;
  IOFD = -1;
  return(ESC);
  }  



				/* Get one character from the keyboard */
getchar()
 {char c;
  c = bios(3);
  if (c > 128) c -= 128;
  return(c);       
  }
 
				/* Put a character to the printer
				     and file  */
pput(c)
 char c;
 {if (HDCPYSW) bdos(5,c);                     
  if (FILESW) rawfput(c);
  if (UNDERSW && c > ' ' && c != '_')
     {pput(BACK);
      pput('_'); 
      }
  }


				/* Put one character to the screen */
putch(c)
 char c;
 {bios(4,c);
  return(c);
  }




kbhit()
 {return(bios(2));          
  }
