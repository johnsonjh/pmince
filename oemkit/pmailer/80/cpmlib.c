/*
 * These functions allow CP/M version 2.x
 *   to access files more than 512K long.
 */


setversion()
 {int i;
  SYS.version = 1;
  if (bdos(12) >= 20) SYS.version = 2;
  printf("\nCP/M version %d.x",SYS.version);
  }




cpmread(fd,buf,n)
 int fd,n;
 char *buf;
 {if (SYS.version == 2) return(rread(fd,buf,n));
  return(read(fd,buf,n));
  }


cpmseek(fd,offset,origin)
 int fd,offset,origin;
 {if (SYS.version == 2) return(rseek(fd,offset,origin));
  return(seek(fd,offset,origin));
  }


cpmwrite(fd,buf,n)
 int fd,n;
 char *buf;
 {if (SYS.version == 2) return(rwrite(fd,buf,n));
  return(write(fd,buf,n));
  }
 