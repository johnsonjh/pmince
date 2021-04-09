#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "listform.inc"
#include "db.inc"
#define HELPFILE "deflform.hlp"


main()
  {char opt,desc[MAXDESCSZ],descbuf[LFRMDSCBUFSZ],chngflg;
   struct formatdir formdir;
   struct listformat format;
   int iformat,i;
   openform(&formdir,DBDIR,LFRMDSCFILE,LISTFRMFILE,descbuf,sizeof(format));
   for(chngflg=0;;)
    {if (menu("List Format Definition",
        "Add a List Format,Review/Revise a List Format,Delete a List Format",
	&opt,0,HELPFILE) == ESC) return(closeform(&formdir,chngflg));
     switch(opt)
				/* Add a format */
      {case 0:
	 if (getstr("format description",desc,
		min(MAXDESCSZ,LFRMDSCBUFSZ-strlen(formdir.desclist)),
		1,HELPFILE) == ESC)
	   break;
	 cleanopt(desc);
	 initlf(&format);
	 if (getformat(&format,desc) == DISCARD) break;
         chngflg = 1;
	 wrtformat(&format,optct(formdir.desclist),&formdir);
	 addopt(desc,formdir.desclist);
	 break;
				/* Revise a format */
       case 1:
	 if (formmenu("Select Format to Review/Revise",
		&iformat,&formdir,2,HELPFILE) == ESC) break; 
         if (menu("Do You Want to Change Format Description?","No,Yes",&opt,
		3,HELPFILE) == ESC) break;
	 if (opt == 1)
	   {if (getstr("new format description",desc,
		min(MAXDESCSZ,LFRMDSCBUFSZ-strlen(formdir.desclist)+
		strlen(extropt(iformat,formdir.desclist,desc))),
		4,HELPFILE) == ESC) break;
	    cleanopt(desc);
	    delopt(iformat,formdir.desclist);
	    insopt(iformat,formdir.desclist,desc);
	    chngflg = 1;
	    }
	 rdformat(&format,iformat,&formdir);
	 if (getformat(&format,
		extropt(iformat,formdir.desclist,desc)) == DISCARD) break;
         compress(&format);
	 wrtformat(&format,iformat,&formdir);
	 chngflg = 1;
	 break;
				/* Delete a format */
       case 2:
	 if (formmenu("Select Format to Delete",&iformat,
		&formdir,5,HELPFILE) == ESC) 
	   break;
         printf("DELETING %s: CONFIRM [y or n]",
		extropt(iformat,formdir.desclist,desc));
         if (putch(tolower(getchar())) != 'y') break;
	 delopt(iformat,formdir.desclist);
	 for(i=iformat;i<optct(formdir.desclist);i++)
	   {rdformat(&format,i+1,&formdir);
	    wrtformat(&format,i,&formdir);
	    }
	 chngflg = 1;
	 break;
      }
    }
  } 








getformat(lf,desc)
 struct listformat *lf;
 char *desc;
 {char opt,sopt,chngflg,*bufmax,options[SYSBUFSZ];
  bufmax = lf->strbuf + STRBUFSZ;
  strcpy(options,"Display List Format,Define List Heading Format Block,");
  strcat(options,"Define Page Format,Define List Member Format Block,");
  strcat(options,"Define Sort Keys,Save List Format Definition");
  for(chngflg = 0;;)
    {if (menu(desc,options,&opt,6,HELPFILE) == ESC) 	
       {if (savmenu("List Format Definition",&sopt,chngflg,7,HELPFILE) != ESC)
 	  return(sopt);
	}
     else switch(opt)
       {case 0: 
	  displf(lf,desc);
	  break;
	case 1:
          getfldvec("List Heading",lf->listhead.headfields,
		&lf->listhead.nheadfields,MAXNHEADFIELDS,
		lf->strbuf,bufmax,&chngflg);
	  break;
	case 2:
	  getpghead(&lf->page,lf->strbuf,bufmax,&chngflg);
	  break;
	case 3:
  	  getfldvec("List Member",lf->memform.memfields,
		&lf->memform.nmemfields,MAXNMEMFIELDS,
		lf->strbuf,bufmax,&chngflg);
	  break;
        case 4:
	  getkeys(&lf->keys,lf->strbuf,bufmax,&chngflg);
	  break;
	case 5:
	  return(SAVE);
        }
      }
   }
     





getpghead(page,strbuf,bufmax,chngflg)
 struct pageformat *page;
 char *strbuf,*bufmax,*chngflg;
 {char opt;
  while(1)
    {if (menu("Page Format Definition",
	"Set Page Size,Set Bottom Margin,Define Page Heading Format Block",
	&opt,27,HELPFILE) == ESC)
       return;
     switch(opt)
       {case 0:
	  getnum("page size",&page->formht,MINPAGESZ,MAXPAGESZ,28,HELPFILE);
	  break;
        case 1:
	  getnum("bottom margin",&page->bottommarg,0,MAXPAGESZ,29,HELPFILE);
	  break;
	case 2:
	  getfldvec("Page Heading",page->pagehead.headfields,
		&page->pagehead.nheadfields,MAXNHEADFIELDS,
		strbuf,bufmax,chngflg);
	  break;
	}
     }
  }










				/* Get a field structure from the user */
getfldvec(mesg,fldvec,nfields,maxnfields,strbuf,bufmax,chngflg)
 char *mesg,*strbuf,*bufmax,*chngflg;
 struct lffield *fldvec;
 int *nfields,maxnfields;
 {char opt,desc[MAXDESCSZ];
  int fldno,i; 
  struct lffield lffld;
  strcpy(desc,mesg);
  strcat(desc," Format Block Definition");
  while(1)
    {if (menu(desc,
	"Add a List Format Field,Change a List Format Field,\
Insert a List Format Field,Delete a List Format Field",
	&opt,8,HELPFILE) == ESC) return;
     switch(opt)
				/* Add a New List Format Field (at end) */
       {case 0:
          if (*nfields + 1 >= maxnfields)
  	     {err("Room only for %d fields",maxnfields);
	      break;
	      }
	  initof(&fldvec[*nfields]);
	  if (getlffield(mesg,&fldvec[*nfields],*nfields+1,
		strbuf,bufmax,chngflg) != ESC)
	     *nfields += 1;
	  break;
				/* Change an Existing List Format Field */
 	case 1:
	  if (*nfields == 0)
	     {err0("No fields have been defined");
	      break;
	      }
	  if (getnum("number of format field to change",&fldno,
		1,*nfields,9,HELPFILE) == ESC) break;
	  getlffield(mesg,&fldvec[fldno-1],fldno,
		strbuf,bufmax,chngflg);
	  break;
				/* Insert a new List Format Field */
	case 2:
	  if (*nfields == 0)
	     {err0("No fields have been defined");
	      break;
	      }
          if (*nfields + 1 >= maxnfields)
  	     {err("Room only for %d fields",maxnfields);
	      break;
	      }
	  if (getnum("number of field before which to insert new field",
		&fldno,1,*nfields,10,HELPFILE) == ESC) return;
	  initof(&lffld);
	  if (getlffield(mesg,&lffld,fldno,
		strbuf,bufmax,chngflg) == ESC)
	     break;
	  for(i=*nfields;i>=fldno;i--)
	    lffldcpy(&fldvec[i],&fldvec[i-1]);
	  lffldcpy(&fldvec[fldno-1],&lffld);
	  *nfields += 1;
	  break;
					/* Delete an Output Field */
	case 3:
	  if (*nfields == 0)
	     {err0("No fields have been defined");
	      break;
	      }
	  if (getnum("format field to delete",&fldno,
		1,*nfields,11,HELPFILE) == ESC) break;  
	  for(i=fldno-1;i<*nfields;i++)
	    lffldcpy(&fldvec[i],&fldvec[i+1]);
	  *nfields -= 1;
	  *chngflg = 1;
	  break;
	} 
      }
   }





getkeys(srtkeys,strbuf,bufmax,chngflg)
 struct sortkeys *srtkeys;
 char *strbuf,*bufmax,*chngflg;
 {char opt;
  int i,keyno;
  struct key0 srtkey;
  while(1)
    {if (menu("Sort Key Definition",
	"Add a Sort Key,Change a Sort Key,Insert a Sort Key,Delete a Sort Key",
	&opt,12,HELPFILE) == ESC) return;
     switch(opt)
       {case 0:
          if (srtkeys->nkeys+1 == MAXNKEYS)
	    {err("Only %d sort keys allowed",MAXNKEYS);
	     break;
	     }
	  srtkey.fldno = -1;
	  srtkey.nsubhdflds = 0;
          getkey(&srtkey,srtkeys->nkeys+1,strbuf,bufmax,chngflg);
	  if (srtkey.fldno >= 0) 
	    keycpy(&srtkeys->key[srtkeys->nkeys++],&srtkey);
          break;
        case 1:
          if (srtkeys->nkeys == 0)
	    {err0("No keys have been defined");
	     break;
	     }
          if (getnum("sort key to change",&keyno,1,
		srtkeys->nkeys,13,HELPFILE) == ESC) break;
          getkey(&srtkeys->key[keyno-1],keyno,strbuf,bufmax,chngflg);
          break;
	case 2:
          if (srtkeys->nkeys == 0)
	    {err0("No keys have been defined");
	     break;
	     }
          if (srtkeys->nkeys+1 == MAXNKEYS)
	    {err("Only %d sort keys allowed",MAXNKEYS);
	     break;
	     }
          if (getnum("sort key before which to insert new key",&keyno,1,
		srtkeys->nkeys,26,HELPFILE) == ESC) break;
	  srtkey.fldno = -1;
	  srtkey.nsubhdflds = 0;
          getkey(&srtkey,keyno,strbuf,bufmax,chngflg);
	  if (srtkey.fldno < 0) break;
	  for(i=srtkeys->nkeys;i>=keyno;i--)
	    keycpy(&srtkeys->key[i],&srtkeys->key[i-1]);
          keycpy(&srtkeys->key[keyno-1],&srtkey);
	  srtkeys->nkeys += 1;
	  break;
        case 3:
          if (srtkeys->nkeys == 0)
	    {err0("No keys have been defined");
	     break;
	     }
          if (getnum("sort key to delete",&keyno,1,
		srtkeys->nkeys,14,HELPFILE) == ESC) break;
          for(i=keyno-1;i<srtkeys->nkeys;i++)
 	    keycpy(&srtkeys->key[i],&srtkeys->key[i+1]);
          srtkeys->nkeys -= 1;
        }
     }
  }

 
				/* Get a single sort key from the user */
getkey(ky,keyno,strbuf,bufmax,chngflg) 
 struct key0 *ky;
 char *strbuf,*bufmax,*chngflg;
 int keyno;
 {char opt,mesg[MAXDESCSZ];
  while(1)
    {strcpy(mesg,"Sort Key #");
     itoa(mesg+strlen(mesg),keyno);
     if (menu(mesg,"Specify Sort Key,Specify Subheading Format Block",&opt,
		15,HELPFILE) == ESC) 
       return;
     switch(opt)
      {case 0:
	 if (menu("Select Sort Key Type",KEYTYPES,
		&ky->type,16,HELPFILE) == ESC) break;
	 ky->type += 1;
 	 if (ky->type == LOGFIELD) 
	   if (accfldtag("tag of sort key field",&ky->fldno,
		17,HELPFILE) == ESC) break; 
	 if (ky->type == TEMPLATE)
 	   if (acctmptag("tag of template to sort by",&ky->fldno,
		18,HELPFILE) == ESC) break;
	 ky->direction = FORWARD;
	 if (FIELD[ky->fldno].type == NUMERIC)
	   if (menu("Select Sort Direction",SORTDIRS,&ky->direction,
		30,HELPFILE) == ESC) break;
         *chngflg = 1;
	 break;
       case 1:
	 getfldvec(strcat(mesg," Subheading"),ky->subhead,&ky->nsubhdflds,
		MAXNSUBHEADFLDS,strbuf,bufmax,chngflg);
	 break;
       }
     }
  }












  				/* Get an output field spec from the user */
getlffield(mesg,lffld,fldno,strbuf,bufmax,chngflg)
 char *mesg,*strbuf,*bufmax,*chngflg;
 struct lffield *lffld;
 int fldno;
 {char opt,retcode,mesg0[SCREENWD];
  strcpy(mesg0,"Field #");
  itoa(mesg0 + strlen(mesg0),fldno);
  strcat(mesg0,", ");
  strcat(mesg0,mesg);
  strcat(mesg0," Format Block");
  for(retcode = ESC;;)
    {if (menu(mesg0,"Specify Field Contents,Specify Field Characteristics",
	&opt,31,HELPFILE) == ESC) return(retcode);
     if (opt == 0) 
       if (getlfcont(mesg0,lffld,strbuf,bufmax,chngflg) != ESC)
	retcode = 0;
     if (opt == 1) getfldspec(mesg0,lffld,chngflg);
     }
  }
   


getlfcont(mesg,lffld,strbuf,bufmax,chngflg)
 char *mesg,*strbuf,*bufmax,*chngflg;
 struct lffield *lffld;
 {char mesg0[SCREENWD];
  strcpy(mesg0,"Select Field Type for ");
  strcat(mesg0,mesg);
  if (menu(mesg0,FIELDTYPES,&lffld->type,19,HELPFILE) == ESC) return(ESC);
  switch(lffld->type)
    {case STRING:
        *chngflg = 1;
	lffld->fldid = buflen(strbuf);
	getfrmstr(mesg,strbuf + lffld->fldid,bufmax,chngflg,20,HELPFILE);
	break;
     case LOGFIELD:
	if (accfldtag("tag of logical field comprising format field",
		&lffld->fldid,21,HELPFILE) == ESC) return(ESC);
        *chngflg = 1;
	break;
     case TEMPLATE:
	if (acctmptag("tag of template to insert in format field",
		&lffld->fldid,22,HELPFILE) == ESC) return(ESC);
        *chngflg = 1;
	break;
     case SSNAME:
     case TODAYDATE:
	*chngflg = 1;
	break;
     }
  return(0);
  }
	



				/* Get field characteristics */
getfldspec(mesg,lffld,chngflg)
 struct lffield *lffld;
 char *mesg,*chngflg;
 {char opt,togbuf[SYSBUFSZ],mesg0[LINBUFSZ];  
  strcpy(mesg0,"Specify Characteristics for ");
  strcat(mesg0,mesg);
  while(1)
    {if (togmenu(mesg0,"Underline Mode,Set Field Length",&opt,
	lffld->dispmode,togbuf,23,HELPFILE) == ESC) return;     
     switch(opt)
       {case 0:
          toggle(&lffld->dispmode,UNDERMASK);
          *chngflg = 1;
          break;
        case 1:
	  if (lffld->posmode != NOPOSIT)
	     {toggle(&lffld->dispmode,LENGTHMASK);
	      lffld->posmode = NOPOSIT;
	      break;
	      }
          if (getnum("field length",&lffld->fldlen,0,LINBUFSZ,
		24,HELPFILE) == ESC) break; 
          *chngflg = 1;
          menu("Select Positioning Mode",POSMODES,&lffld->posmode,
		25,HELPFILE);
	  if (lffld->posmode != NOPOSIT) toggle(&lffld->dispmode,LENGTHMASK);
	  break;
        }
     }
  }






