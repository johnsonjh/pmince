
#include "constant.inc"
#include "sys.inc"
#include "list.inc"

qsort(list,n)
  char *list[MAXLISTSZ];
  int n;
  {int i,j,imin;
   char *srtmin;
   for(i=0;i<n-1;i++)
     {srtmin = list[imin=i];
      for(j=i+1;j<n;j++)
        if (sortcmp(list[j],srtmin) == LESSTHAN)    
	   srtmin = list[imin=j];
      list[imin] = list[i];
      list[i] = srtmin;
      }
   }

sortcmp(s1,s2)
 char *s1,*s2;
 {char r;
  int *n1,*n2;
  while(1)
   {if (*s1 == NUMERIC) 
     {n1 = s1+1;
      n2 = s2+1;
      if ((r = intcmp(*n1,*n2)) != EQUALS) return(r);
      s1 += 3;
      s2 += 3;
      }
    else
      {if (*s1 < *s2) return(LESSTHAN);
       if (*s1 > *s2++) return(GREATERTHAN);
       if (*s1++ == 0) return(EQUALS);
       }
    }
  }
