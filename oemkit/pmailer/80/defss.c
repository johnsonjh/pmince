#include "constant.inc"
#include "sys.inc"
#include "subset.inc"
#include "db.inc"
#define HELPFILE "defss.hlp"

main()
 {char c,opt,desc[MAXDESCSZ],chngflg;
  int i,iss;
  struct subset sub;
  struct ssdirectory sslist;
  openss(&sslist,DBDIR,DEFINESUBSETS);
  for(chngflg = 0;;)
    {if (menu("Subset Definition Module",
	"Add a Subset,Review/Revise a Subset,Delete a Subset",
	&opt,0,HELPFILE) == ESC) return(closess(&sslist,chngflg)); 
     switch(opt)
       {case 0:
	  if (sslist.nsubsets+1 == MAXNSS)
	     {err("No more subsets allowed (Maximum = %d)",MAXNSS);
	      break;
	      }
	  if (getstr("description of subset",desc,
		min(MAXDESCSZ,SSDSCLSTSZ-SSOFFSET-strlen(sslist.ssdesclist)),
		1,HELPFILE) == ESC) break;
	  cleanopt(desc);
	  sszero(&sub,FIELD,SYS.nlflds);
	  if (getssdef(&sub,desc) == DISCARD) 
	     break;
	  chngflg = 1;
	  addopt(desc,sslist.ssdesclist);
	  wrtss(&sub,NEWENTRY,&sslist,FIELD,SYS.nlflds);
	  break;
	case 1:
	  if (ssmenu("Select Subset to Review/Revise",&iss,&sslist,
		2,HELPFILE) == ESC) break;
	  if (menu("Do You Want to Change Subset Description?","No,Yes",&opt,
		3,HELPFILE) == ESC) break;
	  if (opt == 1)
	    {if (getstr("new description of subset",desc,
		min(MAXDESCSZ,SSDSCLSTSZ-SSOFFSET-strlen(sslist.ssdesclist)+
			strlen(extropt(iss,sslist.ssdesclist,desc))),
		4,HELPFILE) == ESC) break;
	     cleanopt(desc);
	     delopt(iss,sslist.ssdesclist);
	     insopt(iss,sslist.ssdesclist,desc);
	     chngflg = 1;
	     }
	  rdss(&sub,iss,&sslist,FIELD,SYS.nlflds);
	  if (getssdef(&sub,extropt(iss,sslist.ssdesclist,desc)) == DISCARD)
	    break;
	  chngflg = 1;
 	  wrtss(&sub,iss,&sslist,FIELD,SYS.nlflds);
	  break;
	case 2:
	  if (ssmenu("Select Subset to Delete",&iss,&sslist,
		5,HELPFILE) == ESC) break;
	  printf("\nDELETING %s--CONFIRM [yes or no]: ",
		extropt(iss,sslist.ssdesclist,desc));
	  if (putch(tolower(getchar())) != 'y') break;
	  chngflg = 1;
	  delopt(iss,sslist.ssdesclist);
	  sslist.nsubsets--;
 	  for(i = iss;i<sslist.nsubsets;i++)
	    {sslist.ssentry[i].sectorst = sslist.ssentry[i+1].sectorst;
	     sslist.ssentry[i].sectorct = sslist.ssentry[i+1].sectorct;
	     }	
	  break;
        }
     } 
  }



			/* Get a Subset Definition from the User */
getssdef(sub,desc)
 struct subset *sub;
 char *desc;
 {char *datlst[MAXNLFLDS],opt,savopt,chngflg;
  int imem;
  for(chngflg=0;;)
    {if (menu(desc,
	"Display Subset Definition,Define Primary Selection Criteria,\
Define Limit Selection Criteria,Save Subset",
	&opt,6,HELPFILE) == ESC) 
	  {if (savmenu("Subset Definition",&savopt,chngflg,7,HELPFILE) != ESC)
	     return(savopt);
	   }
     else 
       switch(opt)
	 {case 0:
	    dispss(sub,desc);
	    break;
	  case 1:
	  case 2:
            lnkdat(sub->ssmem[imem=opt-1],datlst,FIELD,SYS.nlflds);
            ssgetscr(datlst,sub->relop[imem],&chngflg,8,HELPFILE);
	    break;
	  case 3:
	    return(SAVE);
	  }
     }
  }



