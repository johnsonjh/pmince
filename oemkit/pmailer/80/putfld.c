#include "gendb.inc"

putfld(i0,j0)				/* put a data field at 
					   screen position i0,j0 */
  int i0,j0;
  {int i,j,ifld,len,maxlen,min0,max0;

   char typ,tagbuf[MAXTOKSZ];
   if (SYS.nlflds == MAXNLFLDS)
      return(err("No more fields allowed (MAXNLFLDS = %d fields)",MAXNLFLDS));
   if (intmplt(i0,j0))
      return(err0("Must position field outside template"));
   for(
     maxlen=0;j0+maxlen < SCREENWD-1 && SCREEN[i0][j0+maxlen] >= ' ';maxlen++
        );
   if (menu("Select field type",FLDTYPES,&typ,8,HELPFILE) == ESC) 
      return(ESC);
   typ += 1;
   while(1)
     {if (gettok("field tag",tagbuf,
		min(MAXTOKSZ,TAGBUFSZ-taglen(SYS.nlflds)),
		9,HELPFILE) == ESC) return(ESC);
      if (chktag(tagbuf) == 0) break;  
      }
   if (typ == STATUS) 
     {len = 3;
      if (j0 + len >= SCREENWD) return(err0("Not enough room"));
      }
   else
     if (getnum("field length",&len,1,min(MEMBUFSZ-memsize(),maxlen),
		10,HELPFILE) == ESC) return(ESC);
   if (typ == NUMERIC && len < 5)
     {if (getnum("minimum data field value allowed",&min0,
		-MAXINT,MAXINT,11,HELPFILE) == ESC) return(ESC);
      if (getnum("maximum data field value allowed",&max0,
		-MAXINT,MAXINT,11,HELPFILE) == ESC) return(ESC);
      }
   for(j=j0;j<j0+len;j++)
     if (SCREEN[i0][j] < ' ')
       return(err0("Field in the way"));
   ifld = fndfld(i0,j0)+1;
   insfld(ifld-1,1);
   FIELD[ifld].minval = min0;
   FIELD[ifld].maxval = max0;
   FIELD[ifld].type = typ;
   FIELD[ifld].length = (typ != STATUS) ? len : 1;
   FIELD[ifld].tmpltno = NOTEMPLATE;
   FIELD[ifld].ipos = i0;
   FIELD[ifld].jpos = j0;
   strcpy(FIELD[ifld].tag,tagbuf);
   SYS.nlflds++;
   if (typ == STATUS)
     {SCREEN[i0][j0] = '[';
      SCREEN[i0][j0+2] = ']';
      SCREEN[i0][j0+1] = STATUS;
      }
   else
    for(j=j0;j<j0+len;j++)
      SCREEN[i0][j] = typ;
   return(0);
   }






