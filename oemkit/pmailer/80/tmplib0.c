

			/* Check to see if tag is already used 
			    used in a template definition */
chktmtag(tag)
 char *tag;
 {int i,j;
  for(i=0;i<TMPLTLST.ntmplts;i++)
    {if (tag != TMPLTLST.templat[i].tag0)    /* Don't compare with self */
       if (strcmp(tag,TMPLTLST.templat[i].tag0) == 0)
   	 return(err("Tag %s has already been used as a template tag",tag));
     for(j=0;j<TMPLTLST.templat[i].nfields;j++)
       if (tag != TMPLTLST.templat[i].tmfld[j].tag0)    /* "" */
         if (strcmp(tag,TMPLTLST.templat[i].tmfld[j].tag0) == 0)
	   return(err("Tag %s has already been used as a template field tag",
			tag));
      }
  return(0);
  }






			/* Read in the template list */
rdtmplt()
 {return(rdmem("template.sav",&TMPLTLST,sizeof(TMPLTLST)));
  }

			/* Write out the template list */
wrttmplt()
 {wrtmem("template.sav",&TMPLTLST,sizeof(TMPLTLST));
  }










				/* Make a menu from the template list */
mktmopts(opts)
 char *opts;
 {int i;
  *opts = 0;
  for(i=0;i<TMPLTLST.ntmplts;i++)	        /* Build a menu list from */
     {strcat(opts,TMPLTLST.templat[i].tag0+1);     /* the template list */
      if (i<TMPLTLST.ntmplts-1) strcat(opts,",");
      }
  }


