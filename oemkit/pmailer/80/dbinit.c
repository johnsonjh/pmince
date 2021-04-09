#include "gendb.inc"
#include "subset.inc"
#include "htab.inc"

dbinit()     
 {dbcreat();
  htabcreat();
  sscreat();
  lfcreat();
  mfcreat();
  }
 

dbcreat() 
  {char filbuf[MAXFNSZ],buffer[SECTORSZ];
   printf("\nCreating Data Base File");
   close(creat(mkpath(DBDIR,DBFILE,filbuf)));
   if ((DBFD = open(filbuf,2)) < 0) exit();
   setmem(buffer,SECTORSZ,0);
   write(DBFD,buffer,1);
   NEXTSECTOR = 1; 
   strcpy(TODAY.month,"1");
   strcpy(TODAY.day,"1");
   strcpy(TODAY.year,"81");
   wrtdate();
   closedb(); 
   fabort(DBFD);
   }

htabcreat()
  {int i,j,nentrysect;
   char buffer[SECTORSZ];
   struct hashtable htab;
   printf("\nCreating Hash Table");
   mkpath(DBDIR,HTABFILE,htab.filename);
   htab.nbuckets = SYS.nbucket0;
   htab.nextfree = htab.nbuckets;
   close(creat(htab.filename));
   htab.fd = open(htab.filename,2);
   nentrysect = (SECTORSZ/ENTRYSZ);
   setmem(buffer,SECTORSZ,0);
   write(htab.fd,buffer,1);
   for(i=0;i<htab.nbuckets;i++)
     {entry(i,htab)->link = 0;
      entry(i,htab)->sector0 = -1;
      for(j=0;j<MEMIDSZ;j++) 
	entry(i,htab)->memid[j] = 0;
      if (i%nentrysect == nentrysect-1) write(htab.fd,htab.htabbuf,1);
      }
   write(htab.fd,htab.htabbuf,1);
   closehtab(&htab);
   }




sscreat()     
  {int i;
   char filbuf[MAXFNSZ];
   struct ssdirectory sslist;
   printf("\nInitializing Subset Files");
   close(creat(mkpath(DBDIR,SSFILE,filbuf)));
   sslist.nsubsets = 0;
   sslist.nextsssect = 0;
   for(i=0;i<MAXNSS;i++)
      sslist.ssentry[i].sectorst = sslist.ssentry[i].sectorct = 0;
   sslist.ssdesclist[0] = 0;
   wrtmem(mkpath(DBDIR,SSLISTFILE,filbuf),sslist,sizeof(sslist));
   close(creat(mkpath(DBDIR,SSDESCFILE,filbuf)));
   }




lfcreat()       
 {char filbuf[MAXFNSZ];
  printf("\nInitializing List Format Files");
  close(creat(mkpath(DBDIR,LISTFRMFILE,filbuf)));
  close(creat(mkpath(DBDIR,LFRMDSCFILE,filbuf)));
  }


mfcreat()       
 {char filbuf[MAXFNSZ];
  printf("\nInitializing Printer Form Files");
  close(creat(mkpath(DBDIR,MAILFRMFILE,filbuf)));
  close(creat(mkpath(DBDIR,MFRMDSCFILE,filbuf)));
  }

