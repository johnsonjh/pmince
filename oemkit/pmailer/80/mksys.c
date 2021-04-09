#include "gendb.inc"

			/* Read in a data base image */ 
rdimg()
 {char filbuf[MAXFNSZ];
  int i,j;
  if (fopen0(mkpath(DBDIR,DEFFILE,filbuf),FREAD) < 0) return;
  rdsys(&SYS);
  rdfld(FIELD,SYS.nlflds);
  rdtag();
  rdscr(SCREEN);
  fclose();
  for(i=0;i<SCREENHT;i++)
    for(j=strlen(SCREEN[i]);j<SCREENWD;j++)
       SCREEN[i][j] = ' ';
  }

	   
				/* Read in the field tags */
rdtag()
 {int i;
  for(i=0;i<SYS.nlflds;i++)
    fgetstr(FIELD[i].tag = TAG[i]);
  }




  			/* Write out the data base definition */
wrtimg()
 {char filbuf[MAXFNSZ];
  prdbtxt(FILEON);
  printf("\n\n\n\nWriting %s",DEFFILE);
  if (fopen0(mkpath(DBDIR,DEFFILE,filbuf),FWRITE) < 0) exit();
  wrtsys(&SYS);
  wrtfld();
  wrttag();
  wrtscr();
  fclose();
  }




      



  
			/* Print a textual description of the data base */
prdbtxt(mode)	
 char mode;
 {int i;
  char filbuf[MAXFNSZ];
  if (mode == FILEON) 
    {if (fopen0(mkpath(DBDIR,TXTFILE,filbuf),FWRITE) < 0) return;
     printf("\n\nWriting db.txt");
     setmode(FILEON);
     }
  else
     hdcpyopt();
  dispelem();
  disptag();
  if (mode == CRTON)           
    {err0("");
     return;
     }
  dispsys();
  setmode(FILEOFF);
  printf("\n\n\n\nWriting member.h");
  if (fopen0(mkpath(DBDIR,"member.h",filbuf),FWRITE) < 0) return;
  setmode(FILEON);
  dispstr();
  setmode(FILEOFF);
  }

				/* Display the member elements */
dispelem()
 {int i,k,itmplt;
  char buf[MAXDESCSZ];
  printf("\nI. Member Elements");
  for(i=k=0;i<SYS.nlflds;)
     {printf("\n%d:  ",++k);
      if ((itmplt = FIELD[i].tmpltno) != NOTEMPLATE)
	{printf("%s; %s template; size = %d",
		mktmtag(FIELD[i].tag,buf),TMPLTLST.templat[itmplt].tag0,
		tmsize(&TMPLTLST.templat[itmplt]));
	 i += TMPLTLST.templat[itmplt].nfields;
         }
      else
         {printf("%s; %s field; size = %d",
		FIELD[i].tag,extropt(FIELD[i].type-1,FLDTYPES,buf),
		FIELD[i].length);
	  if (FIELD[i].type == NUMERIC && FIELD[i].length < MAXNDIG)
	     printf("; Min=%d, Max=%d",
		FIELD[i].minval,FIELD[i].maxval);
	  i++;
	  procchar();
	  }
      }
  printf("\nTOTAL MEMBER SIZE = %d [of %d Available]",memsize(),MEMBUFSZ);
  printf("\nTOTAL FIELDS USED = %d [of %d Available]",SYS.nlflds,MAXNLFLDS);
  }


				/* Display the field tags */
disptag()
 {int i;
  printf("\n\n\nII. Logical Field Tags");
  for(i=0;i<SYS.nlflds;i++)
    {procchar();
     printf("\nField %d: %s",i+1,FIELD[i].tag);
     }
  printf("\nTOTAL TAG LENGTH = %d [of %d Available]",
	taglen(SYS.nlflds),TAGBUFSZ);
  }

			/* Display the member structure */
dispstr()
 {int i,itmplt;
  char tagbuf[MAXTAGSZ];
  printf("\nstruct member\n  {");
  for(i=0;i<SYS.nlflds;)
    {if ((itmplt = FIELD[i].tmpltno) != NOTEMPLATE)
       {printf("struct %s %s",TMPLTLST.templat[itmplt].tag0,
		mktmtag(FIELD[i].tag,tagbuf));
	i += TMPLTLST.templat[itmplt].nfields;
	}
     else
       {printf("char %s[%d]",TAG[i],FIELD[i].length+1);
	i++;
	}
     printf(";\n   ");
     }
  printf("};\n\n");
  }
				/* Calculate the size of the member */
memsize()
 {int i,sz;
  for(i=sz=0;i<SYS.nlflds;i++)
    sz += FIELD[i].length+1;
  return(sz);
  }


				/* Calculate the size of a template */
tmsize(tmplt)
 struct template *tmplt;
 {int i,sz;
  for(i=sz=0;i<tmplt->nfields;i++)
    sz += tmplt->tmfld[i].lfield.length+1;
  return(sz);
  }



