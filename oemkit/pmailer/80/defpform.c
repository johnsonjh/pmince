#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "mail.inc"
#include "db.inc"    
#include "mform.inc"
#define HELPFILE "defpform.hlp"

main()
 {struct mailformat mform;
  struct formatdir formdir;
  char descbuf[MFRMDSCBUFSZ],desc[MAXDESCSZ],opt,chngflg;
  int iformat,listsz,i;
  openform(&formdir,DBDIR,MFRMDSCFILE,MAILFRMFILE,descbuf,sizeof(mform));
  for(chngflg=0;;)
    {if (menu("Printer Form Definition",
      "Add a Printer Form,Review/Revise a Printer Form,Delete a Printer Form",
	&opt,0,HELPFILE) == ESC) 
       return(closeform(&formdir,chngflg));
     switch(opt)
       {case 0:
	  if (getstr("description of printer form",desc,
		min(MAXDESCSZ,MFRMDSCBUFSZ-strlen(formdir.desclist)),1,
		HELPFILE) == ESC) break; 
	  cleanopt(desc);
	  setdefmf(&mform);
	  if (getmform(&mform,desc) == DISCARD) break;
	  chngflg = 1;
	  wrtformat(&mform,optct(formdir.desclist),&formdir);
	  addopt(desc,formdir.desclist);
	  break;
	case 1:
	  if (formmenu("Select Printer Form to Review/Revise",&iformat,
		&formdir,2,HELPFILE) == ESC) break;
	  if (menu("Do You Want to Change Form Description","No,Yes",&opt,
		3,HELPFILE) == ESC) break;
	  if (opt == 1)
	    {listsz = strlen(formdir.desclist) + 
		strlen(extropt(iformat,formdir.desclist));
	     if (getstr("new printer form description",desc,
		min(MAXDESCSZ,MFRMDSCBUFSZ-listsz),
		4,HELPFILE) == ESC) break;
	     cleanopt(desc);
	     delopt(iformat,formdir.desclist);
	     insopt(iformat,formdir.desclist,desc);
	     }
	  rdformat(&mform,iformat,&formdir);
	  if (getmform(&mform,
		extropt(iformat,formdir.desclist,desc)) == DISCARD) break;
	  wrtformat(&mform,iformat,&formdir);
   	  chngflg = 1;
	  break;
	case 2:
	  if (formmenu("Select Printer Form to Delete",&iformat,&formdir,
		5,HELPFILE) == ESC) break;
	  delopt(iformat,formdir.desclist);
	  for(i=iformat;i<optct(formdir.desclist);i++)
	     {rdformat(&mform,i+1,&formdir);
	      wrtformat(&mform,i,&formdir);
	      }
	  chngflg = 1;
	  break;
	}
     }
  }


getmform(mform,desc)
 struct mailformat *mform;
 char desc[MAXDESCSZ];
 {char chngflg,opt,sopt,options[SYSBUFSZ],buffer[MAXDESCSZ];
  strcpy(options,"Display Printer Form Definition,");
  strcat(options,"Set Left Margin,Set Right Margin,");
  strcat(options,"Set Default Salutation,");
  strcat(options,"Set Continuous Forms Optiono,");
  strcat(options,"Set Label Dimensions,Save Printer Form Definition");
  for(chngflg = 0;;)
    {if (menu(desc,options,&opt,6,HELPFILE) == ESC) 
       {if (savmenu("Printer Form Definition",&sopt,chngflg,7,HELPFILE) != ESC)
	  return(sopt);
	}
     else switch(opt)
       {case 0:
	  dispmform(mform,desc);
	  break;
	case 1:
	  if (getnum("left margin [measured in 1/10's inch]",
		&mform->leftmargin,0,MAXLMARG,8,HELPFILE) == ESC) break;
	  chngflg = 1;
	  break;
	case 2:
     	  if (getnum("right margin [measured in 1/10's in. from left edge]",
		&mform->rightmargin,
		MINRMARG,MAXRMARG,9,HELPFILE) == ESC) break;
	  chngflg = 1;
	  break;
	case 3:
	  getdefsal(mform);
	  break;
 	case 4:
	  if (mform->nacross > 0) break;   
	  if (menu("Select Form Type",CONTOPTS,&mform->contopt,
		10,HELPFILE) == ESC) break;
	  chngflg = 1;
	  if (mform->contopt == CONTINUOUS) 
	     if (getnum(
		"vertical distance between top of one form and top of next",
		&mform->height,MINFORMHT,MAXFORMHT,11,HELPFILE) == ESC)
	      mform->contopt = SEPARATE;
	  if (mform->contopt == SEPARATE) err0("Separate Sheets Mode");
	  break;
	case 5:
	  getlabform(mform);
	  chngflg = 1;
	  break;
	case 6:
	  return(SAVE);
	}
     }
  }
  


				/* Get a lablel format from the user */
getlabform(mform)
 struct mailformat *mform;
 {if (getnum("number of labels across",&mform->nacross,1,MAXNACROSS,
	12,HELPFILE) == ESC) return;
  if (mform->nacross > 1)
    if (getnum("label width (including space between labels)",  
	&mform->labwidth,MINLABWD,MAXLABWD,13,HELPFILE) == ESC) return;
  if (getnum("vertical distance between top of one label and top of next",
	&mform->height,MINFORMHT,MAXFORMHT,14,HELPFILE) == ESC) return;
  mform->contopt = CONTINUOUS; 
  mform->leftmargin = 0;
  mform->rightmargin = LINBUFSZ;
  }
 



				/* Display the mail format definition */
dispmform(mform,desc)
 struct mailformat *mform;
 char *desc;
 {char buffer[MAXDESCSZ];
  hdcpyopt();
  if (mform->contopt != SEPARATE)   
    printf("\nForm Height = %d lines",mform->height);
  if (mform->nacross == 0)   
    {printf("\nLeft Margin = %d.%d in.",
	mform->leftmargin/10,mform->leftmargin%10);
     printf("\nRight Margin = %d.%d in. [From left edge]",
	mform->rightmargin/10,mform->rightmargin%10);
     printf("\nDefault Salutation = %s",mform->defsalut);
     printf("\n%s",extropt(mform->contopt,CONTOPTS,buffer));
     }
  else
    {printf("\nLabel Forms");
     printf("\n%d Label%c Across",
	mform->nacross,mform->nacross > 1 ? 's' : BELL);
     if (mform->nacross > 1) printf("; Labels %d.%d in. apart",
	  mform->labwidth/10,mform->labwidth%10);
     }
  err0("");
  }
 


				/* Set a default mail format values */
setdefmf(mform)
 struct mailformat *mform;
 {mform->leftmargin = 15;
  mform->rightmargin = 70;
  strcpy(mform->defsalut,"Dear Person");
  mform->nacross = 0;
  mform->contopt = SEPARATE;
  }





getdefsal(mform)
 struct mailformat *mform;
 {char opt,options[SYSBUFSZ];
  strcpy(options,"Dear Person,To Whom It May Concern,Dear Sir,");
  strcat(options,"Gentlemen,OTHER");
  if (menu("Select Default Salutation",options,&opt,16,HELPFILE) == ESC)
    return;
  switch(opt)
    {case 0:
       strcpy(mform->defsalut,"Dear Person");
       return;
     case 1:
       strcpy(mform->defsalut,"To Whom It May Concern");
       return;
     case 2:
       strcpy(mform->defsalut,"Dear Sir");
       return;
     case 3:
       strcpy(mform->defsalut,"Gentlemen");
       return;
     case 4:
       getstr("default salutation",mform->defsalut,
	  MAXDESCSZ,17,HELPFILE);
       return;
     }
  }
