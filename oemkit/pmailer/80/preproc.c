#include "constant.inc"
#include "sys.inc"
#include "mail.inc"
#include "db.inc"
#include "mform.inc"
		/* Pre-process the piece of letter pointed to by letbuf;
		    put output in buffer and return number of characters
		    processed, or -1 on error */
preproc(letbuf,datlst,mform,buffer) 
 char *letbuf,*datlst[MAXNLFLDS],*buffer;
 struct mailformat *mform;
 {int ifld,nchars,i;
  char token[MAXTAGSZ],argument[MAXTAGSZ];
  *buffer = 0;
  while(1) 
    {if (*letbuf > 0 && *letbuf < ' ' && *letbuf != '\n')
	return(err("Bad character [Ascii %d]",*letbuf));
     if (*letbuf != '{') 
         {ccat(buffer,*letbuf != 0 ? *letbuf : EOF);
	  return(1);
	  }
     if (grabtok(letbuf,token,argument,&nchars) < 0) 
       return(err0("Command not recognized"));
     switch(tokparse(token,&ifld))
	{case LOGFIELD: 
            strcpy(buffer,datlst[ifld]);
		/* If formatting is not in effect, then pad each
		     field with blanks to fill field length */
/*  Do this with some other report type; too awkward here 
  	    if (FORMATFLG == 0)			
              for(i=strlen(datlst[ifld]);i<FIELD[ifld].length;i++)
 	        ccat(buffer,' ');
*/	    break;
	 case TEMPLATE:
	    instmplt(buffer,datlst,ifld);
   	    break;
	 case KEYWORD:
	    ccat(buffer,KEYWORDS[ifld].keychar);
	    break; 
	 case TODAYDATE:
	    insdate(buffer,&TODAY);
	    break;
	 case SALUTATION:
	    if (inssalut(buffer,argument,datlst,mform) < 0) return(-1);
	    break;
	 case -1:
	    return(err("Command %s not recognized",token));
         }
      filterbuf(buffer);
      return(nchars);
      }
   } 



				/* Grab a command from the letter */
grabtok(letbuf,token,argument,nchars)
 char *letbuf,token[MAXTAGSZ],argument[MAXTAGSZ];
 int *nchars;
 {int i;
  *letbuf++;				/* Skip open brace */
  *nchars = 2;				/* To account for braces */
  *token = 0;
  for(i=0;i<MAXTAGSZ;i++)
    {if (*letbuf == '}') return(0);
     if (*letbuf == '(')
       return(grabarg(letbuf,argument,nchars));
     if (*letbuf == 0) return(-1);
     ccat(token,tolower(*letbuf++));
     (*nchars)++;
     }
  return(-1);
  }



				/* Grab a command argument */
grabarg(letbuf,argument,nchars)
 char *letbuf,argument[MAXTOKSZ];
 int *nchars;
 {int i;
  *argument = 0;
  *letbuf++;				/* Skip the open paren */
  *(nchars) += 2;			/* Account for parens */
  for(i=0;i<MAXTOKSZ;i++)
    {if (*letbuf == ')') return(0);
     if (*letbuf == 0) return(-1);
     ccat(argument,tolower(*letbuf++));
     (*nchars)++;
     }
  return(-1);
  }



tokparse(token,ifld)		/* See what kind of token we have */
  char *token;
  int *ifld;
  {if ((*ifld = fldmatch(token)) >= 0) return(LOGFIELD);
   if ((*ifld = tmpmatch(token)) >= 0) return(TEMPLATE);
   if ((*ifld = keymatch(token)) >= 0) return(KEYWORD);
   if (strcmp(token,"today") == 0) return(TODAYDATE);
   if (strcmp(token,"salutation") == 0) return(SALUTATION);
   return(-1);
   }

				/* See if token is a keyword */
keymatch(token)	
 char *token;
 {int i;
  for(i=0;i<NKEYWORDS;i++) 
    if (strcmp(KEYWORDS[i].keytag,token) == 0) return(i); 
  return(-1);
  }


inssalut(buffer,argument,datlst,mform) 
 char *buffer,*argument,*datlst[MAXNLFLDS];
 struct mailformat *mform;
 {int ifld;
  struct _name *name;
  char *salut;  
  if ((ifld = tmpmatch(argument)) < 0)
    return(err("Salutation argument error: %s not recognized",argument));
  if (FIELD[ifld].tmpltno != NAME)
    return(err("Salutation argument error: %s is not a name template",
	argument));
  name = datlst[ifld];
  strcpy(buffer,"Dear ");
  if (*name->salutation) strcat(buffer,name->salutation);
  else
    if (*name->title) 
      {strcat(buffer,name->title);    
       ccat(buffer,' ');
       strcat(buffer,name->lastname);
       }
    else 
       strcpy(buffer,mform->defsalut);
  return(0);
  }
















			/* Filter the special characters in buffer */
filterbuf(buffer)
 char *buffer;
 {char buf0[SYSBUFSZ],*b,*b0;
  b0 = buf0;
  for(b=buffer;;)
    {if (*b == '.') *b0++ = '\\';
     if ((*b0++ = *b++) == 0) break; 
     }
  strcpy(buffer,buf0);
  }
 







