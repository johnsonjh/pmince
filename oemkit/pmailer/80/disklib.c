#include "constant.inc"
#include "iovars.inc"

/*
			-------------------------------
			| Direct Access I/O Functions |
			-------------------------------
*/
				/* Read nbytes into memory location p1 
				    from the n'th "slot" of file fd  */
rddac(fd,n,p1,nbytes)
 int fd,n,nbytes;
 char *p1;
 {int nsect;
  char buffer[SECTORSZ];
  nsect = sectct(nbytes);
  if (seek(fd,n*nsect,0) < 0) exit();
  if (nbytes%SECTORSZ == 0) 
     return(read(fd,p1,nsect));
  if (nsect > 1) read(fd,p1,nsect-1);
  read(fd,buffer,1);
  strcpy0(p1+ (nsect-1)*SECTORSZ,buffer,nbytes%SECTORSZ);
  }


				/* Write nbytes memory locations from p1
				    to the n'th "slot" of file fd */
wrtdac(fd,n,p1,nbytes)
 int fd,n,nbytes;
 char *p1;
 {int nsect;
  nsect = sectct(nbytes);
  if (seek(fd,n*nsect,0) < 0) exit();
  write(fd,p1,nsect);
  }





/*
			------------------------
			| Buffer I/O Functions |
			------------------------
*/
				/* Write nbytes from location cp to file */
wrtmem(file,cp,nbytes)
 char file[],*cp;
 int nbytes;
 {if (fopen0(file,FWRITE) < 0) return(-1);
  while(nbytes--) rawfput(*cp++);
  fclose();
  return(0);
  }

				/* Read nbytes into location cp from file */
rdmem(file,cp,nbytes)
 char file[],*cp;
 int nbytes;
 {if (fopen0(file,FREAD) < 0) return(-1);
  while(nbytes--) *cp++ = rawfget();
  fclose();
  return(0);
  }


				/* Read file filename in to buffer */
rdfil(filename,buffer,bufsz)
  char *filename,*buffer;
  int bufsz;
  {int i;
   *buffer = 0;
   if (fopen0(filename,FREAD) < 0) return(-1);
   for(i=0;i<bufsz;i++)
      if ((*buffer++ = fget()) == EOF) break;   
   if (i == bufsz)
      {err("Not enough room to store contents of %s",filename);
       return(-1);
       }
   *(buffer-1) = 0;
   fclose();
   return(0);
   }



				/* Open a file for buffer i/o */
fopen0(file,mode)
   char *file;
   int mode;
   {int i;
    if (mode == FREAD)
     if ((IOFD = open(file,FREAD)) < 0) 
       return(err("Cannot open %s for reading",file));
    if (mode == FWRITE) 
     if ((IOFD = creat(file)) < 0) exit();
    if (mode == FAPPEND)
     {if ((IOFD = open(file,FAPPEND)) < 0) 
        return(fopen0(file,FWRITE));
      IOPOS = SECTORSZ;
      for(i=0;fget() != EOF;i++);
      IOPOS = (IOPOS > 0) ? IOPOS-1 : SECTORSZ;
      if (i>0) seek(IOFD,-1,1);
      }
    IOMODE = mode;
    if (IOMODE == FWRITE) IOPOS = 0; 
    if (IOMODE == FREAD)  IOPOS = SECTORSZ;
    return(0);
    }


  









				/* Close a buffered i/o file */
fclose()
 {if (IOFD < 0) return;
  if (IOMODE == FREAD)
    return(fabort(IOFD));
  while(IOPOS < SECTORSZ) IOBUF[IOPOS++] = EOF;
  if (IOPOS > 0) write(IOFD,IOBUF,1);      
  close(IOFD);     
  IOFD = -1;
  }

  


  


				/* Fget a 2 byte word */
fgetw()
 {char *cp;
  int k;
  cp = k;
  *cp++ = fget();
  *cp = fget();
  return(k);
  }


				/* Fget the nbytes and put them at p1 */
fgetbuf(p1,nbytes)
 char *p1;
 int nbytes;
 {while(nbytes--) *p1++ = rawfget();
  }


				/* Fput the nbytes from p1 */
fputbuf(p1,nbytes)
 char *p1;
 int nbytes;
 {while(nbytes--) rawfput(*p1++);
  }


				/* Fget a string */
fgetstr(buffer)
 char *buffer;
 {while(*buffer++ = fget());
  }



				/* Get one buffered i/o character */
fget()
  {char c;
   if (IOPOS < SECTORSZ)  c  = IOBUF[IOPOS++];
   else
      {if (read(IOFD,IOBUF,1) <= 0) return(EOF);
       IOPOS = 0;
       c = IOBUF[IOPOS++];
       }
   if (c == CR) return(fget());
   else	        return(c);
   } 




				/* Fput a string */
fputstr(s)
 char *s;
 {char *s0;
  s0 = s;
  while(fput(*s++));
  return(s-s0);
  }


				/* Fput a 2 byte word */
fputw(k)
 int k;
 {char *cp;
  cp = &k;
  rawfput(*cp++);
  rawfput(*cp);
  return(k);
  }

				/* Put one character to buffered i/o */
fput(c)
  char c;
  {if (c == '\n') fput(CR);
   if (IOPOS == SECTORSZ)
      {write(IOFD,IOBUF,1);
       IOPOS = 0;
       }
   IOBUF[IOPOS++] = c;
   return(c);
   } 
  


				/* Get one buffered i/o character 
				    without translating cr-lf */
rawfget()
  {char c;
   if (IOPOS < SECTORSZ)  c  = IOBUF[IOPOS++];
   else
      {if (read(IOFD,IOBUF,1) <= 0) return(EOF);
       IOPOS = 0;
       c = IOBUF[IOPOS++];
       }
   return(c);
   } 


				/* Put one buffered i/o character 
				    without translating cr-lf */
rawfput(c)
  char c;
  {if (IOPOS < SECTORSZ) IOBUF[IOPOS++] = c;
   else
      {write(IOFD,IOBUF,1);
       IOPOS = 0;
       return(rawfput(c));
       }
   return(c);
   } 
  










				/* Count the number of sectors nchar 
				     characters will occupy */
sectct(nchar)
 int nchar;
 {int nsect;
  nsect = nchar/SECTORSZ;
  if (nchar%SECTORSZ) nsect++;
  return(nsect);
  }




