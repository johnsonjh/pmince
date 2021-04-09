#include "constant.inc"
#include "sys.inc"
#include "list.inc"
#include "db.inc"
#include "extsort.inc"
  
extsort(params)
 struct listparams *params;
 {int lowfile,highfile;
  printf("\nHeap Sort v1.3");
  highfile = params->nfiles - 1;
  for(lowfile = 0;lowfile < highfile;)
    {merge(lowfile,highfile,params);
     lowfile += MERGEORDER;
     highfile += 1;
     }
  }
  


				/* Merge the files from lowfile to highfile;
				    if this is the last run put the output on
				    imagefile */
merge(lowfile,highfile,params)
 int lowfile,highfile;
 struct listparams *params;
 {struct heapbuffer *heap[MERGEORDER];
  int i,nfiles;
  nfiles = openheap(heap,lowfile,highfile,params);
  while(nfiles > 0)
    {putkey(heap[0]->key,lowfile,highfile);
     if (nextkey(heap[0]) == EOF)
        {nfiles--;
         for(i=0;i<nfiles;i++)
           heap[i] = heap[i+1];
	 }
     else reheap(heap,nfiles);
     }
  if (lowfile+MERGEORDER <= highfile) fput(EOF);
			/* If this is the last run, write out an integer 0
			     at the end of the sort image file to indicate  
			     end of file */
	                              fputw(0);
  fclose();
  }




	 		/* Set hbuf->key to the next key in the heap buffer;
			     return EOF if no more keys */ 
nextkey(hbuf)  
 struct heapbuffer *hbuf;
 {int nchars;
  char filbuf[MAXFNSZ];
			/* When key is with MAXKEYSZ characters of the
			   end of the file buffer, copy what is left in    
			   the buffer to the beginning and copy in the     
			   next set of keys after it */
  if (hbuf->eob - hbuf->key < MAXKEYSZ)
    {nchars = hbuf->eob - hbuf->key;
     strcpy0(hbuf->keybuf,hbuf->key,nchars);
     rddac(hbuf->fd,hbuf->bufct++,hbuf->keybuf + nchars,FILEBUFSZ);
     hbuf->eob = hbuf->keybuf + FILEBUFSZ + nchars;
     hbuf->key = hbuf->keybuf;
     }
  hbuf->key += 3 + keylen(hbuf->key);
  if (*hbuf->key == EOF) 
    {fabort(hbuf->fd);
     unlink(mkmrgfil(hbuf->fileno,filbuf));
     return(EOF);
     }
  return(0);   
  }


			/* Set up the merge files from lowfile to highfile;
			    set up output file [use imagefile if this is the
			    last run */
openheap(heap,lowfile,highfile,params)
 struct heapbuffer *heap[MERGEORDER];
 int lowfile,highfile;
 struct listparams *params;
 {int i,nfiles;
  char filbuf[MAXFNSZ],*filename;
  nfiles = min(MERGEORDER,highfile-lowfile+1);
  for(i=0;i<nfiles;i++)
    {heap[i] = &HEAPBUFFER[i];
     heap[i]->fileno = lowfile + i;
     if ((heap[i]->fd = open(mkmrgfil(heap[i]->fileno,filbuf),0)) < 0)
        exit(err("Cannot open %s",filbuf));
     heap[i]->bufct = 0;
     rddac(heap[i]->fd,heap[i]->bufct++,heap[i]->keybuf,FILEBUFSZ);
     heap[i]->key = heap[i]->keybuf;
     heap[i]->eob = heap[i]->keybuf + FILEBUFSZ;
     printf("\nRun %d",heap[i]->fileno);
     }
  qhpsort(heap,nfiles);

		/* See if this will be the last set of runs */   
  if (lowfile + MERGEORDER <= highfile)  
		/* If not, another merge file will be standard output */
    filename = mkmrgfil(highfile+1,filbuf);
		/* If so, sort image file will be standard output */
   else 	
    {filename = params->imagefile;
     if ((params->target & IMAGEMASK) == 0) 
	strcpy(params->imagefile,DEFIMAGEFILE);
     }
  if (fopen0(filename,FWRITE) < 0) exit();
  return(nfiles);
  }

    

				/* Reheap nfiles in heap */
reheap(heap,nfiles)
 struct heapbuffer *heap[MERGEORDER];
 int nfiles;
 {int i,j;
  for(i=1;2*i <= nfiles;i=j)
    {j = 2*i;
     if (j < nfiles)
        if (sortcmp(heap[j-1]->key,heap[j]->key) == GREATERTHAN) j += 1;
     if (sortcmp(heap[i-1]->key,heap[j-1]->key) != GREATERTHAN) return;
     exchng(&heap[i-1],&heap[j-1]);
     }
  }




		/* Quick Heap Sort [it is arguable that qsort should be made 
		    to work here, but the keys to be sorted are embedded here
		    in a special purpose data structure and the function is 
		    short enough to have a couple around] */
qhpsort(heap,n)
  struct heapbuffer *heap[MERGEORDER];
  int n;
  {int i,j,imin;
   char *srtmin;
   for(i=0;i<n-1;i++)
     {srtmin = heap[imin=i]->key;
      for(j=i+1;j<n;j++)
        if (sortcmp(heap[j]->key,srtmin) == LESSTHAN)
	   srtmin = heap[imin=j]->key;
      exchng(&heap[imin],&heap[i]);
      }
   }



exchng(p1,p2)
 char **p1,**p2;
 {char *temp;
  temp = *p1;
  *p1 = *p2;
  *p2 = temp;
  }





				/* Put out one sort key to the output file;
				    only put sector number if this is the
				    last run */
putkey(key,lowfile,highfile)
 char *key;
 int lowfile,highfile;
 {int keysz,*sect0;
  keysz = keylen(key);
  if (lowfile + MERGEORDER <= highfile) 
    {fputbuf(key,keysz);
     rawfput(0);
     }
  sect0 = key+keysz+1;
  fputw(*sect0);
  }
