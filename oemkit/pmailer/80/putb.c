#include "constant.inc"
#include "iovars.inc"

main()
 {initio();
  printf("Banner:\n");
  putbanner();
  }

putbanner(program)   
 char *program;
 {char *string;
  int i;
  puts(program);
  string = "\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
  for(i=0;putchar(string[i] + (i+25));i++);
  }

