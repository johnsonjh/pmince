#include "constant.inc"
#include "sys.inc"
#include "htab.inc"
#include "format.inc"
#include "subset.inc"
#include "db.inc"
#include "mform.inc"
#define HELPFILE "mail.hlp"

main()
 {char descbuf[MFRMDSCBUFSZ],target;
  struct ssdirectory sslist;
  struct formatdir formdir;
  struct mailformat mform;
  struct hashtable htab;
  int hitlist[HITBUFSZ],iformat;
  initmail(&sslist,&formdir,descbuf,&htab);
  for(hitlist[0] = 0;;)
    {if (getform("Select Printer Form",&mform,&iformat,&formdir,
	 0,HELPFILE) == ESC) 
       {fabort(sslist.ssfd);
        fabort(formdir.formfd);
        fabort(htab.fd);
	return; 
	}
     mail(&mform,&htab,&sslist,target,hitlist);
     }
  }


initmail(sslist,formdir,descbuf,htab) 
 struct ssdirectory *sslist;
 struct formatdir *formdir;
 char *descbuf;
 struct hashtable *htab;
 {struct mailformat *mf;
  strcpy(htab->filename,HTABFILE);
  htab->nbuckets = SYS.nbucket0;
  openss(sslist,DBDIR,USESUBSETS);
  openhtab(htab);
  initkey();
  openform(formdir,DBDIR,MFRMDSCFILE,MAILFRMFILE,descbuf,sizeof(*mf));
  }

