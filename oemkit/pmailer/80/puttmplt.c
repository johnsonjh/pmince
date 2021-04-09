#include "gendb.inc"

puttmplt(i0,j0)
  int i0,j0;
  {int i,itmpl;
   char buffer[SYSBUFSZ];
   mktmopts(buffer);
   if (imenu("Choose a template",buffer,&itmpl,6,HELPFILE) == ESC) 
	return(ESC);
   puttm(i0,j0,itmpl);
   }


puttm(i0,j0,itmpl)	/* Put template tmpl at screen position i0,j0 */
  int i0,j0,itmpl;
  {int i,j,jj,ifld,nflds,k,maplen,ndesc,ndat;
   char newlinflg,*map0,*map,tmtag[MAXTOKSZ];
   struct template *tmpl;
   tmpl = &TMPLTLST.templat[itmpl];
   if (SYS.nlflds + tmpl->nfields > MAXNLFLDS)
     return(err("Not enough room (MAXNLFLDS = %d fields)",MAXNLFLDS));
   if (tmsize(tmpl) + memsize() > MEMBUFSZ)
     return(err("Not enough room (MEMBUFSZ = %d characters)",MEMBUFSZ));
   if (intmplt(i0,j0))
     return(err0("Move outside of template"));
   while(1)
     {if (gettok("template tag",tmtag,MAXTOKSZ,7,HELPFILE) == ESC)
	 return;
      if (chktag(tmtag) == 0) break;  
      }
   i = i0; j = j0;
   nflds = tmpl->nfields;
   ifld = fndfld(i0,j0)+1;
   insfld(ifld-1,nflds);
   SYS.nlflds += nflds;
   for(k=0;k<nflds;k++)
     {newlinflg = 0;
      map0 = &TMPLTLST.mapbuf.buffer[tmpl->tmfld[k].mapindex]; 
				/* See how much space on the screen the 
			  	     template will occupy  */
      ndat = tmpl->tmfld[k].lfield.length;
      ndesc = 0;
      for(map=map0;*map;map++)
	 if (*map > ' ' && *map != FILLMAP) ndesc++;
      maplen = ndat+ndesc;     
      if (j + maplen > SCREENWD-1 || tmpl->tmfld[k].positflg == NEWLINE)
			       /* if newlinflg == 1 => this field           
	  		          must go onto the next line */            
	 {newlinflg = 1;
          for(jj=j;jj<SCREENWD;jj++)
	     if (SCREEN[i][jj] < ' ') 
	        return(noroom(i0,j0,i,j,ifld,nflds,"Field in the way"));
	  i++;
	  j = j0;
	  }
				/* Check space constraints */
      if (j+maplen > SCREENWD-1)
	  return(noroom(i0,j0,i,j,ifld,nflds,"No room on line"));
      if (newlinflg == 1 && i == SCREENHT)
	  return(noroom(i0,j0,i,j,ifld,nflds,"No room on page"));
      if (newlinflg == 1 && tmpl->tmfld[k].positflg == ADJACENT)
	  return(noroom(i0,j0,i,j,ifld,nflds,"Cannot split template"));
      for(jj=j;jj<j+maplen;jj++)
        if (SCREEN[i][jj] < ' ')
          return(noroom(i0,j0,i,j,ifld,nflds,"Field in the way"));
				/* Put this template in FIELD */
      settmfld(&FIELD[ifld+k],itmpl,k,i,j,tmtag);
				/* Fill in the screen map */
      for(map=map0;*map;map++)
	 {if (*map == FILLMAP) 
	    {while(ndat--) SCREEN[i][j++] = tmpl->tmfld[k].lfield.type;
	     break;
	     }
	  else
	    if ((SCREEN[i][j++] = *map) < ' ') ndat--;
	  }
      }
   if (taglen(SYS.nlflds) > TAGBUFSZ)
     return(noroom(i0,j0,i,j,ifld,nflds,"Tag too long"));
   }


				/* Set one template field */
settmfld(lfld,itmpl,ifld,i,j,tmtag) 
  struct logfield *lfld;  
  int itmpl,ifld,i,j;
  char *tmtag;
  {struct templatfld *tmfield;
   tmfield = &TMPLTLST.templat[itmpl].tmfld[ifld]; 
   fldcpy(lfld,&tmfield->lfield);
   lfld->tmpltno = itmpl;
   lfld->tmpltpos = ifld;
   lfld->ipos = i;
   lfld->jpos = j;
   strcpy(lfld->tag,tmtag);
   strcat(lfld->tag,".");
   strcat(lfld->tag,tmfield->tag0);
   }



noroom(i0,j0,i1,j1,ifld,nflds,mesg)
  int i0,j0,i1,j1,ifld,nflds;
  char *mesg;
  {int i,j,jmax;
   dispscr();
   pos(i0,0);
   err0(mesg);
   delfld(ifld,nflds);
   SYS.nlflds -= nflds;
   for(i=i0;i<=i1;i++)
     {jmax = (i == i1) ? j1 : SCREENWD;
      for(j=j0;j<jmax;j++)
         SCREEN[i][j] = ' ';
      }
   }
