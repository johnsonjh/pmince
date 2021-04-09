#include "constant.inc"
#include "sys.inc"
#include "mail.inc"
#include "db.inc"
#include "mform.inc"
     		/*  newlinflg == 1 -> Previous character was '\n' */
		/*  stripspaceflg -> Strip spaces and 'n' */
		/*  rawflg -> Print next without processing */
 				/* Print out the letter */
prlet(letbuf,datlst,mform,target)
 char *letbuf,*datlst[MAXNLFLDS],target;
 struct mailform *mform;
 {int i,n,lin,spacect,nchars,rmarg,lmarg,newlinflg;
  char c,stripspaceflg,rawflg;
  char buffer[SYSBUFSZ],*buf;
  rawflg = FORMATFLG = spacect = stripspaceflg = 0; 
  if ((target & HDCPYMASK) && mform->contopt == SEPARATE) 
     err0("Load Form in Printer");
  lmarg = mform->leftmargin;
  rmarg = mform->rightmargin;
  settarg(target);
  setmargin(lmarg);
  putspace(lmarg); 
  lin = 0;
  CARRIAGEPOS = ILABPOS = JLABPOS = 0;
  while(1)          
    {if (rawflg && *letbuf != 0) 
		/*  This traps special characters that have been
		    "escaped" using the backslash */
       {mput(mform,*letbuf++);		
		/* This takes care of periods at the end of 
		   fields inserted from the data base followed
		   by periods that end sentences */
        if (*(letbuf-1) == '.' && *letbuf == '.') letbuf++;
        }
     if ((nchars = preproc(letbuf,datlst,mform,buffer)) < 0) 
          return(initio());
     rawflg = 0;
     letbuf += nchars;
     for(buf = buffer;*buf;)         
      {c = *buf++;
       if (rawflg && c != 0) 
         {mput(mform,c);    
	  rawflg = 0;				
	  if ((c = *buf++) == 0) break;
 	  }
       if (c != ' ' && c != '\n') stripspaceflg = 0;
       newlinflg--;   
       switch(c)
         {case EOF: 
	    MARGIN = 0;
			/* Space to end of page for continuous forms
			   (but not for labels) */
	    if (mform->contopt == CONTINUOUS)
	       while(lin++ < mform->height) mput(mform,'\n');
	    HDCPYSW = FILESW = 0;
	    CRTSW = 1;
            if (mform->nacross > 0) NACROSSCT += 1;
	    return(0);  
	  case '\n':
	    if (stripspaceflg) break;
	    if (FORMATFLG)
              {if (CARRIAGEPOS + spacect <= rmarg) 
	         {if (spacect == 0 && CARRIAGEPOS > lmarg) spacect = 1;
	          break;
	          }
               newlinflg = 2;
               }
          case NEWLINE:
	     if (newlinflg == 1) break;
	     mput(mform,'\n');
	     lin++;
             spacect = 0;
	     stripspaceflg = FORMATFLG;
 	     break;
	  case ' ':
	     if (stripspaceflg) break;
	     if (FORMATFLG)
		{if (CARRIAGEPOS+spacect >= rmarg)  
		   *--buf = '\n';
		 else 
		   if (spacect == 0) spacect = 1;
		 break;
		 }
	     mput(mform,' ');
	     break;
	  case FORMAT:
	     FORMATFLG = 1;
  	     break;
	  case UNFORMAT:
	     stripspaceflg = FORMATFLG = 0;
	     break;
	  case UNDER:
	     setmode(UNDERDSP);
	     break;
	  case NOUNDER:
	     setmode(NORMALDSP);
	     break;
	  case PARAGRAPH:
	     mput(mform,'\n');
	     putspace(PARINDENT);
	     lin++;
  	     spacect = 0;
	     stripspaceflg = FORMATFLG;
	     break;
	  case '|':
	     if (FORMATFLG && CARRIAGEPOS>rmarg)
		{mput(mform,'-');
		 *--buf = '\n';
		 }
	     break;
	  case '.':
	     if (FORMATFLG && (*letbuf == ' ' || *letbuf == '\n')) spacect = 2;
	     mput(mform,'.');
	     break;
	  case '\\': 
	     rawflg = 1;
	     break;
	  default: 
	     if (spacect)
		{putspace(spacect);
		 spacect = 0;
		 }
  	     if (procchar() == ESC) {initio(); return(ESC);}
 	     mput(mform,c);
          }
	}
     } 
  } 



			/* Put one mail character; buffer n-across labels */
mput(mform,c)
 struct mailformat *mform;
 char c;
 {int labno,jpos;
	/* Only have to buffer labels which are more than 1 across */
  if (mform->nacross <= 1) return(putchar(c));
  labno = NACROSSCT % mform->nacross;
  if (ILABPOS == 0 && JLABPOS == 0 && labno == 0)
    if (flushlab(mform) == ESC) return(ESC);
  if (c == '\n') 
    {ILABPOS++;
     JLABPOS = CARRIAGEPOS = 0;
     return(0);
     }
  jpos = JLABPOS + mform->labwidth * labno;
  if (jpos >= LABBUFWD-1 || ILABPOS >= LABBUFHT-1)
    {err0("Label buffer overflow");
     return(ESC);
     }
  LABBUF[ILABPOS][jpos] = c;
  JLABPOS++;
  return(0);
  }


			/* Flush and clear the label buffer */
flushlab(mform)
 struct mailformat *mform;
 {int i,j,jmax;
  if (mform->nacross <= 1) return;  
  for(i=0;i<mform->height;i++)
    {for(jmax = LABBUFWD-1;LABBUF[i][jmax] == ' ' && jmax > 0;jmax--);
     for(j=0;j<=jmax;j++)
       {if (NACROSSCT > 0)
          if (putchar(LABBUF[i][j]) == ESC) return(ESC);
        LABBUF[i][j] = ' ';
        }
     if (NACROSSCT > 0) putchar('\n');
     }
  }
  

				/* Set up the mail targets */
settarg(target)
 char target;
 {if (target & HDCPYMASK) setmode(HDCPYON);
  if (target & FILEMASK)  setmode(FILEON);
  if ((target & CRTMASK) == 0) setmode(CRTOFF);
  }







