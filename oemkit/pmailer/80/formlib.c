#include "constant.inc"
#include "sys.inc"
#include "format.inc"
#include "iovars.inc"


					/* Display the format descriptions
					    as a menu and accept selection;
					    read in selected format  */
getform(mesg,format,iformat,formdir,helpid,helpfile)
 char *mesg,helpid,*helpfile;
 char *format;
 int *iformat;
 struct formatdir *formdir;
 {if (formmenu(mesg,iformat,formdir,helpid,helpfile) == ESC) return(ESC);
  rdformat(format,*iformat,formdir);
  return(0);
  }




				/* Display the menu of formats to the user */
formmenu(mesg,iformat,formdir,helpid,helpfile)
 char *mesg,helpid,*helpfile;
 int *iformat;
 struct formatdir *formdir;
 {if (optct(formdir->desclist) == 0)
    {err0("No formats have been defined");
     return(ESC);
     }
  if (imenu(mesg,formdir->desclist,iformat,helpid,helpfile) == ESC) 
     return(ESC);
  return(0);
  }
 




				/* Read in a list format */
rdformat(format,iformat,formdir) 
 char *format;
 int iformat;
 struct formatdir *formdir;
 {rddac(formdir->formfd,iformat,format,formdir->formsz);
  }


				/* Write out a list format */
wrtformat(format,iformat,formdir)
 char *format;
 int iformat;
 struct formatdir *formdir;
 {wrtdac(formdir->formfd,iformat,format,formdir->formsz);
  }
  

				/* Open format file and read in desc's */
openform(formdir,directory,dscfil,frmfil,dscbuf,frmsz)
 struct formatdir *formdir;
 char *directory,*dscfil,*frmfil,*dscbuf;
 int frmsz;
 {char filbuf[MAXFNSZ];
  mkpath(directory,dscfil,formdir->descfile);
  formdir->desclist = dscbuf;
  formdir->formsz = frmsz;
  if ((formdir->formfd = open(mkpath(directory,frmfil,filbuf),2)) < 0) 
    exit(err("Cannot open file %s",frmfil));
  return(rdfil(formdir->descfile,formdir->desclist,EXTENTSZ));
  }



				/* Close format file and write out desc's */
closeform(formdir,chngflg)
 struct formatdir *formdir;
 char chngflg;
 {if (chngflg == 0) return(fabort(formdir->formfd));
  close(formdir->formfd);
  wrtmem(formdir->descfile,formdir->desclist,strlen(formdir->desclist));
  }




