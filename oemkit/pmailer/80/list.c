#include "constant.inc"
#include "sys.inc"
#include "list.inc"
#include "format.inc"
#include "listform.inc"
#include "subset.inc"
#include "db.inc"

main()
 {struct subset sub;
  struct ssdirectory sslist;
  struct listformat format;
  struct formatdir formdir;
  struct listparams params;
  char descbuf[LFRMDSCBUFSZ],opt,mode;
  int list[MAXLISTSZ];
  openss(&sslist,DBDIR,USESUBSETS);
  openform(&formdir,DBDIR,LFRMDSCFILE,LISTFRMFILE,descbuf,sizeof(format));
  while(1)
   while(1)
    {if (getform("Select List Format",&format,&params.iformat,&formdir,
	0,HELPFILE) == ESC) 
       {fabort(sslist.ssfd);
	fabort(formdir.formfd);
	return;
	}
     if (getss("Select Subset to List",&sub,
	&params.iss,&sslist,FIELD,SYS.nlflds,1,HELPFILE) == ESC) break;
     extropt(params.iss,sslist.ssdesclist,params.ssdesc);
     gettarg(&params);                                   
     if ((mode = sort(&sub,&format,&params,list)) == ESC) break;
     if (mode == INFILE) extsort(&params);
     else
       if (params.target & IMAGEMASK) putimage(list,params.imagefile);
     genlist(list,&format,&params);
     }
  }




			/* Get the list target options from the user */
gettarg(params)           
 struct listparams *params;
 {char opt,togbuf[SYSBUFSZ];
  params->target = CRTMASK;
  while(1)
    {if (togmenu("Select List Targets","CRT,Printer,File,Sort Image",&opt,
		params->target,togbuf,2,HELPFILE) == ESC) return;
     switch(opt)
       {case 0:
 	  toggle(&params->target,CRTMASK);
	  break;
        case 1:
 	  toggle(&params->target,HDCPYMASK);
	  break;
	case 2:
	  if (params->target & FILEMASK) 
	    {toggle(&params->target,FILEMASK);
	     fclose();
	     break;
	     }
	  if (gettok("List Output File Name",params->listfile,
		MAXFNSZ,3,HELPFILE) == ESC) break;
	  toggle(&params->target,FILEMASK);
	  break;
	case 3:
	  if (params->target & IMAGEMASK)
	    {toggle(&params->target,IMAGEMASK);
	     break;
	     }
	  if (gettok("Sort Image Output File Name",params->imagefile,
		MAXFNSZ,4,HELPFILE) == ESC) break;
	  toggle(&params->target,IMAGEMASK);
	  break;
        }
     }
  }

   
  



					/* Generate a list */
genlist(list,format,params)    
 int list[MAXLISTSZ];
 struct listparams *params;
 struct listformat *format;
 {int i,bufno,line,fd;
  char mem[MEMBUFSZ],mem0[MEMBUFSZ];
  char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS];
				/* Don't waste time if no member format */
  if (format->memform.nmemfields == 0) return;
  lnkdat(mem,datlst,FIELD,SYS.nlflds);
  lnkdat(mem0,datlst0,FIELD,SYS.nlflds);
  if (params->target == CRTMASK) err0("");
  settarg(params);          
  for(i=0;i<SYS.nlflds;i++) 		/* Set up mismatch so all subheadings
					    are initially generated */  
    {strcpy(datlst[i],"a");
     strcpy(datlst0[i],"b");
     }
  line = 0;
  if (params->nfiles > 0)
    if ((fd = open(params->imagefile,0)) < 0) exit();
  genlfblk(format->listhead.headfield,format->listhead.nheadfields,
	datlst,params->ssdesc,format->strbuf,&line,format->page.formht);
  genlfblk(format->page.pagehead.headfield,format->page.pagehead.nheadfields,
	datlst,params->ssdesc,format->strbuf,&line,format->page.formht);
  for(bufno = 0;;bufno++)
    {if (params->nfiles > 0) rddac(fd,bufno,list,MAXLISTSZ*2);
      for(i=0;i < MAXLISTSZ && list[i] != 0;i++)
       {rdmbr0(datlst,list[i],SYS.nlflds,DBFD);
        genpghead(&format->page,datlst,params->ssdesc,
		format->strbuf,&line);
        gensubhead(datlst,datlst0,&format->keys,params->ssdesc,
   	  format->strbuf,&line,format->page.formht);
        genlfblk(format->memform.memfield,format->memform.nmemfields,
   	    datlst,params->ssdesc,format->strbuf,&line,
	    format->page.formht);
        datcpy(datlst0,datlst);
        if (procchar() == ESC) goto out;
        } 
     if (i < MAXLISTSZ && list[i] == 0) break;
     }
out:
  if (params->target != CRTMASK) 
    while(line++ % format->page.formht) putchar('\n');
  if (params->nfiles > 0) fabort(fd);
  setmode(FILEOFF);
  err0("");
  }





