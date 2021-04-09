#include "constant.inc"

#define MENCURSCOL 10	 	/* Column in which menu cursor apprears */
#define MENOPTSCOL 14		/* Column in which menu options begin */

/*
				------------------------
				| User Input Functions |
				------------------------
*/
/*
getnum(III)	  Get a Number From The User

Usage: 
	   if (getnum(item,&n,minn,maxn,helpid,helpfile)) == ESC) return(ESC);
           if (getnum(item,&n,minn,maxn,helpid,helpfile)) == ESC) break;
 

Parameters:
	char *item; 		String describing item to be input
	int n;			Pointer to integer variable to be input
	int minn;		Minimum value allowed
	int maxn;		Maximum value allowed
	int helpid;		Help screen number
	int helpfile;		File containing help screens

Return Values:
	0		Number between minn and maxn is stored in n
	ESC		User hit escape key

Example:
	if (getnum("number of sort keys",&nkey,
		1,MAXNKEY,NUMHLP0,HELPFILE) == ESC) return(ESC);
*/


getnum(item,n,minn,maxn,helpid,helpfile)
  char *item,*helpfile;
  int *n,minn,maxn,helpid;
  {char str[9],c,errflag;
   int i,n0;
   n0 = *n;
   str[i=0] = 0;
   for(errflag=0;;errflag=0)
     {printf("\nEnter %s: %s",item,str);
      while(errflag == 0)
       switch(c=getchar())
        {case BACK: 
	 case DELETE:
	   if (i>0) 
	      {setmode(DELETE); 
	       str[--i] = 0;
	       }
	   break;
         case CR:
	   *n = atoi(str);    
	   if (*n > maxn)
	     {errflag = err("Number too large: Maximum value = %d",maxn);
	      break;
	      }
	   if (*n < minn)
	     {errflag = err("Number too small: Minimum value = %d",minn);
	      break;
	      }
	   return(c);
	 case ESC:
	   *n = n0;
	   return(ESC);
	 case CTLQ:
	   help(helpid,helpfile);
	 case CTLX:
	   errflag = 1;
	   break;
	 default:
	   if ((c == '-' || isdigit(c)) && i < MAXNDIG) 
  	      str[i++] = putch(c);
	   else		     		                
	      putch(BELL);
	   str[i] = 0;
         }
      }
   }





















/*

getstr(III)	  Get a String From The User

Usage: 
	if (getstr(item,string,maxsz,helpid,helpfile) return(ESC);
	if (getstr(item,string,maxsz,helpid,helpfile) break;
 

Parameters:
        char *item;		String describing item to be input
	char *string;		String to be input                    
	int bufsz; 	        Size of buffer to hold string 
 	int helpid;		Help screen number
	int helpfile;		File containing help screens

Return Values:
 	0		String fewer than bufsz characters is stored in string
	ESC		User hit escape key

Example:
	if (getstr("description of field containing member ID",desc,
		MAXDESCSZ,STRHLP0,HELPFILE) == ESC) return(ESC);
*/


getstr(item,string,bufsz,helpid,helpfile)
  char *item,*string,*helpfile;
  int bufsz,helpid;
  {char c,errflag,string0[LINBUFSZ];
   int i;
   strcpy0(string0,string,LINBUFSZ);
   string[0] = 0;
   for(i=errflag=0;;errflag = 0)
    {printf("\nEnter %s: %s",item,string);
     while(errflag == 0)
       switch(c=getchar())
        {case CR: 
	   string[i] = 0;
	   return(c);
	 case ESC:
	   strcpy0(string,string0,LINBUFSZ);
	   return(ESC);
	 case BACK:
	 case DELETE:
	   if (i > 0) 
	     {string[--i] = 0;
              setmode(DELETE);
	      }
	   break;
	 case CTLQ:
	   help(helpid,helpfile);
	 case CTLX:
	   errflag = 1;
	   break;
	 default:
	   if (i == bufsz-1)    
	     {err("Maximum size is %d characters",bufsz-1);
	      errflag = 1;
	      break;
	      }
	   if (c < ' ' || c > '~')
	      {putch(BELL);
	       break;
	       }
	   string[i++] = putch(c);
	   string[i] = 0;
	 }
      }
   }


















/*

gettok(III)	  Get a Token from the User 

Usage: 
	if (gettok(item,string,bufsz,helpid,helpfile) return(ESC);
	if (gettok(item,string,bufsz,helpid,helpfile) break;
 

Parameters:
        char *item;		String describing item to be input
	char *token;		Token to be input                    
	int bufsz;		Size of buffer to hold string 
 	int helpid;		Help screen number
	int helpfile;		File containing help screens

Return Values:
	0		String fewer than bufsz characters is stored in string
	ESC		User hit escape key

Example:
	if (gettok("tag of field containg sort key",filename,
		MAXTAGSZ,TOKHLP0,HELPFILE) == ESC) return(ESC);
*/

gettok(item,string,bufsz,helpid,helpfile)
  char *item,*string,*helpfile;
  int bufsz,helpid;
  {char c,errflag,string0[LINBUFSZ];
   int i;
   strcpy0(string0,string,LINBUFSZ);
   string[0] = 0;
   for(i=errflag=0;;errflag = 0)
    {printf("\nEnter %s: %s",item,string);
     while(errflag == 0)
       switch(c=getchar())
        {case CR: 
	   string[i] = 0;
	   return(c);
	 case ESC:
   	   strcpy0(string,string0,LINBUFSZ);
	   return(ESC);
	 case BACK:
	 case DELETE:
	   if (i > 0) 
	     {string[--i] = 0;
              setmode(DELETE);
	      }
	   break;
	 case CTLQ:
	   help(helpid,helpfile);
	 case CTLX:
	   errflag = 1;
	   break;
	 default:
	   if (i == bufsz-1)    
	     {errflag = err("Maximum size is %d",bufsz-1);
	      break;
	      }
	   if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && 
		(c < 'A' || c > 'Z') && c != '.' && c != ':')
	      {putch(BELL);
	       break;
	       }
	   if (isdigit(c) && i == 0)
	     {errflag = err("Digit not allowed as first character of %s",item);
	      break;
	      }
	   string[i++] = putch(c);
	   string[i] = 0;
	 }
      }
   }

















/*
menu,imenu		Display a Menu and Return User Response

Usage:
	if (menu(mesg,opts,&c,helpid,helpfile) == ESC) return(ESC);

	char *mesg;		String containing instructive message
	char *opts;		String containing list of options 
				    (separated by commas) 
	char c;			Return value containing option chosen
 	int helpid;		Help screen number
	int helpfile;		File containing help screens

Return Values:
	0		Char c contains the number of the option chosen
	ESC		User hit escape key

Example:
	if (menu("Mail System",
		"Generate Letters,Generated Addresses,Print Forms Queue",
		&mailopt,MENHLP0,HELPFILE) == ESC) exec("main");

*/






			/* Display a menu and return value as integer */
imenu(mesg,optlist,i,helpid,helpfile)     
 char *mesg,*optlist,*helpfile;
 int *i,helpid;
 {*i=0;
  return(menu(mesg,optlist,i,helpid,helpfile));
  }


			/* Display a menu and return character value */
menu(mesg,optlist,opt,helpid,helpfile)     
 char *mesg,*optlist,*opt,helpid,*helpfile;
   {int nopts;
    char opt0;
    opt0 = *opt;
    nopts = optct(optlist);
    dispmen(mesg,optlist,nopts);
    for(*opt=0;;)         
      {putch(' ');
       pos(menurow(*opt,nopts),MENCURSCOL);
       printf("X%c",BACK);
       switch(tolower(getchar())) 
	{case ' ':
	 case CR:
         case LF:
	    if (*opt < nopts-1) ++*opt;
	    break;  
	 case BACK:
         case UP:
	    if (*opt > 0) --*opt;
	    break;
	 case 'x':
	    setmode(CLEAR);
	    return(0);
	 case ESC:
	    *opt = opt0;
	    setmode(CLEAR);
	    return(ESC);
	 case CTLQ:
	    setmode(CLEAR);
	    if (helpfile == 0) 
	      err0("No Help Available");
	    else
	      help(helpid,helpfile);
	 case CTLX:
            dispmen(mesg,optlist,nopts);
	    break;
         }
       }
    }         



				/* Display the menu */
dispmen(mesg,optlist,nopts)
 char *mesg,*optlist;  
 int nopts;
 {int i;        
  char option[MAXDESCSZ];
  setmode(CLEAR);
  setmode(UNDERDSP);
  puts(mesg); 
  setmode(NORMALDSP);
  for(i=0;i<nopts;i++)
    {pos(menurow(i,nopts),MENOPTSCOL);
     puts(extropt(i,optlist,option));
     }
  }




		/* Return the row number that option opt should appear */
menurow(opt,nopts)
 char opt;
 int nopts;
 {int lskip,topmargin;
  lskip = (SCREENHT-2)/nopts;
  topmargin = 2 + (SCREENHT-2 - nopts*lskip)/2;
  return(topmargin + lskip*opt + lskip/2);
  }




			/* "Toggle Switch" menu */
togmenu(mesg,options,opt,optmask,togopts,helpid,helpfile)
 char *mesg,*options,*opt,optmask,togopts[SYSBUFSZ],helpid,*helpfile;
 {int i,nopts;
  char buffer[MAXDESCSZ];
  togopts[0] = 0;
  nopts = optct(options);
  for(i=0;i<nopts;i++)
    {strcat(togopts,extropt(i,options,buffer));
     if (1 & (optmask >> i)) strcat(togopts,"->ON");
     else		     strcat(togopts,"->OFF");
     if (i < nopts-1) ccat(togopts,',');
     }
  return(menu(mesg,togopts,opt,helpid,helpfile)); 
  }


			/* Make sure the user wants to discard input */
savmenu(item,sopt,chngflg,helpid,helpfile)
 char *item,*sopt,chngflg,helpid,*helpfile;
 {char opts[SECTORSZ];
  if (chngflg == 0) 
    {*sopt = DISCARD;
     return(0);      
     }
  strcpy(opts,"DISCARD Input to ");
  strcat(opts,item);
  strcat(opts,",Save Input to ");
  strcat(opts,item);
  return(menu("Confirm Discard of Recent Input",opts,sopt,helpid,helpfile));
  }



				/* Change commas to semis */
cleanopt(opt)
 char *opt;
 {for(;*opt;opt++) if (*opt == ',') *opt = ';'; 
  }



				/* Add option opt to the end of optlist */ 
addopt(opt,optlist)
 char *opt,*optlist;  
 {if (optlist[0] == 0)
    strcpy(optlist,opt);   
  else
    {ccat(optlist,',');
     strcat(optlist,opt);
     }
  }

					/* Delete the iopt'th option from
					    optlist */      
delopt(iopt,optlist)
 int iopt;
 char *optlist;
 {char *p1,*p2;
  int i;
  p1 = optlist;   
  for(i=0;i<iopt;p1++)
    if (*p1 == ',') i++;
  for(p2=p1;*p2 != ',' && *p2 != 0;p2++);
  if (*p2 == ',')  strcpy(p1,p2+1);  
    else		
      if (i != 0) strcpy(p1-1,p2);
        else *p1 = 0;
  }




					/* Insert opt into the iopt'th 
					     position of optlist */
insopt(iopt,optlist,opt)
 int iopt;
 char *optlist,*opt;
 {int i,optlen,nopts;
  char *p1;
  nopts = optct(optlist);
  p1 = optlist;
  optlen = strlen(opt);
				/* Move out to the iopt'th option */
  for(i=0;i<iopt;p1++)
    if (*p1 == ',' || *p1 == 0) 
      if (++i == iopt) break;
				/* Make a hole in which to put new option */
  advstr(p1,optlen+1);
				/* Make sure comma is there */
  if (i == 0) *(optlist + optlen) = (nopts > 0) ? ',' : 0;
  if (*p1 == 0) *p1 = ',';
  if (i > 0) p1++;
				/* Move the new option in */
  strcpy0(p1,opt,optlen);     
  }


			/* Extract and return the iopt'th option 
			   from options list (store it in buffer) */
extropt(opt,optlist,buffer)
 char opt,*optlist,*buffer;
 {char *p,*b;
  for(p=optlist;opt;) if (*p++ == ',') opt--;
  for(b=buffer;(*b = *p++) != ',' && *b != 0;b++);
  *b = 0;
  return(buffer);
  }






			/* Count the number of options in optlist */
optct(optlist)
 char *optlist;
 {int i;
  if (*optlist == 0) return(0);
  for(i=1;*optlist;) if (*optlist++ == ',') i++;
  return(i);
  }






















/*
			---------------------------
			| Error Display Functions |
			---------------------------
*/




				/* Print error message with argument */
err(mesg,valptr)
 char mesg[],*valptr;
 {printf("\n");
  printf(mesg,valptr);
  printf("%c\n *** Hit any character to continue. ***\n",BELL);
  getchar();
  return(-1);
  }


				/* Print error message with no argument */
err0(mesg)
 char mesg[];
 {setmode(HDCPYOFF);
  printf("\n%s",mesg);
  printf("%c\nHit any character to continue.\n",BELL);
  getchar();
  return(-1);
  }
















/*
			--------------------------------------
			| Miscellaneous User Query Functions |
			--------------------------------------
*/

				/* Confirm abort, then abort */
abort()
  {setmode(CLEAR);
   printf("TYPE 'y' TO ABORT, any other character to continue program%c",BELL);
   if (getchar() == 'y') exit();
   }




			/* Allow the user the option of hard copy */
hdcpyopt()	
  {char opt;
   if (menu("Do You Want Hard Copy Output?",
	"No--Print on Screen Only,Yes--Sent Output to Printer",
	&opt,0,0) == ESC) return;
   if (opt == 1) setmode(HDCPYON);
   }



			/* Clear leading and trailing blanks */
clrblnks(s)
  char *s;
  {char *p; 
			/* Strip leading blanks */
   for(p=s;*p == ' ';p++);
   strcpy(s,p);
			/* Strip trailing blanks */
   for(p=s;*p;p++);
   for(p--;*p == ' ' && p >= s;p--) *p = 0;
   return(s);
   }



				/* Count the number of lines in buffer */
linect(buffer)
 char *buffer;
 {int nlines;
  for(nlines=0;*buffer;buffer++)
    if (*buffer == '\n') nlines++;
  return(nlines);
  }



				/* Count the number of sectors nchar 
				     characters will occupy */
sectct(nchar)
 int nchar;
 {int nsect;
  nsect = nchar/SECTORSZ;
  if (nchar%SECTORSZ) nsect++;
  return(nsect);
  }




				/* Toggle the bit in mode masked by mask */
toggle(mode,mask)
 char *mode;
 int mask;
 {if (*mode & mask) *mode &= ~mask;
  else		    *mode |= mask;
  }





				/* Return the length of the strbuf */
buflen(strbuf)
 char *strbuf;
 {int i;
  for(i=0;strbuf[i] != EOB;i++);
  return(i);
  }







/*
			-------------------------------------
			| Printf Function From Library Deff |
			-------------------------------------
*/

printf(format)
 char *format;
 {char buffer[LINBUFSZ];
  _spr(buffer,&format);
  puts(buffer);
  }




				/* Move string up n positions */
advstr(string,n)
 char *string;
 int n;
 {char *p; 
  for(p = string + strlen(string) + n;p >= string+n;p--)
     *p = *(p-n);
  } 


				/* Make a path name (from directory and
				     file name) */
mkpath(dir,fname,pathname)
 char *dir,*fname,*pathname;
 {*pathname = 0;
  if (tolower(*dir) != 'x')
    {ccat(pathname,*dir);
     ccat(pathname,':');
     }
  strcat(pathname,fname);
  return(pathname);
  }




/*
	swapin:
	This is the swapping routine, to be used by the root
	segment to swap in a code segment in the area of memory
	between the end of the root segment and the start of the
	external data area. See the document "SWAPPING.DOC" for
	detailed info on the swapping scheme.


	This version does not check to make sure that the code
	yanked in doesn't overlap into the extenal data area (in
	the interests of keeping the function short.) But, if you'd
	like swapin to check for such problems, note that memory 
	locations ram+115h and ram+116h contain the 16-bit address
	of the base of the external data area (low order byte first,
	as usual.) By rewriting swapin to read in one sector at a time
	and check the addresses, accidental overlap into the data area
	can be avoided.
*/

swapin(name,addr)
char *name;		/* the file to swap in */
{
	int fd;
	if (( fd = open(name,0)) < 0)      
		return(err("Cannot find %s",name)); 
	if ((read(fd,addr,9999)) < 0) exit();
	close(fd);
	return(0);
 }





				/* Change the "directory" [i.e. home disk]
				     to disk */
chdir(disk)
 char *disk;
 {bdos(14,tolower(*disk)-'a');
  }




dump(buffer,nchars)
 char *buffer;
 int nchars;
 {int i;
  err0("About to dump buffer");
  for(i=0;i<nchars;i++)
    {if (i%8 == 0) putchar('\n');
     printf("%d [%c]    ",buffer[i],buffer[i]);
     }
  err0("");
  }
