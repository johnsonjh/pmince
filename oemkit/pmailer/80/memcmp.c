#include "constant.inc"
#include "sys.inc"
#include "subset.inc"
#include "listform.inc"
#include "db.inc"

memcmp(datlst,relop,datlst0,field,nlflds)
 char *datlst[MAXNLFLDS],relop[NSSMEM][MAXNLFLDS],*datlst0[NSSMEM][MAXNLFLDS];
 struct logfield field[MAXNLFLDS];
 int nlflds;
 {int k;
  for(k=0;k<NSSMEM;k++)
    if(memcmp0(datlst,relop[k],datlst0[k],field,nlflds) == 0) return(0);
  return(1);
  }


memcmp0(datlst,relop,datlst0,field,nlflds)
 char *datlst[MAXNLFLDS],relop[MAXNLFLDS],*datlst0[MAXNLFLDS];
 struct logfield field[MAXNLFLDS];
 int nlflds;
 {int i;
  for(i=0;i<nlflds;i++)
    if (datlst0[i][0])
      if (compare(relop[i],datlst,datlst0,&i,field) == 0) return(0);
  return(1);
  }



		/* See if the relational operator specified by the subset
		    definition (relop0) jives with the actual relationship
                    of the fields */
compare(relop0,datlst,datlst0,ifld,field)
 char relop0,*datlst[MAXNLFLDS],*datlst0[MAXNLFLDS];
 int ifld;
 struct logfield field[MAXNLFLDS];
 {char rel;
  rel = fldcmp(datlst,datlst0,ifld,TEMPLATE,field);
  switch(relop0)
   {case EQ:
      if (rel == EQUALS) return(1);
      return(0);
    case NE:
      if (rel == EQUALS) return(0);
      return(1);
    case GT:
      if (rel == GREATERTHAN) return(1);
      return(0);
    case LT:
      if (rel == LESSTHAN) return(1);
      return(0);
    case GE:
      if (rel == LESSTHAN) return(0);
      return(1);
    case LE:
      if (rel == GREATERTHAN) return(0);
      return(1);
    }
  }



		/* Compare field *ifld of data list datlst and data
		    list datlst0; if cmtyp == TEMPLATE compare date
		    and time as templates if they are fully specified */
fldcmp(datlst,datlst0,ifld,cmtyp,field)
 char *datlst[MAXNLFLDS],*datlst0[MAXNLFLDS],cmtyp;
 int *ifld;
 struct logfield field[MAXNLFLDS];
 {int r;
  if (cmtyp == TEMPLATE)
    switch(field[*ifld].tmpltno)
     {case DATE:
        if (datlst0[(*ifld)+1][0] && datlst0[*(ifld)+2][0])
          {r = intcmp(datect(datlst[*ifld]),datect(datlst0[*ifld]));
           (*ifld) += 2;
           return(r);
           }
        break;
      case TIME:
        r = intcmp(timect(datlst[*ifld]),timect(datlst0[*ifld]));
        (*ifld) += 3;
        return(r);
      }
  if (field[*ifld].type == NUMERIC && field[*ifld].length < 5)
     return(intcmp(atoi(datlst[*ifld]),atoi(datlst0[*ifld])));
  return(alpcmp(datlst[*ifld],datlst0[*ifld]));
  }


			/* Compare 2 integers */
intcmp(n1,n2)
 int n1,n2;
 {if (n1 == n2) return(EQUALS);
  if (n1 < n2) return(LESSTHAN);
  return(GREATERTHAN);
  }


			/* Return the number of days in a date */
datect(date)
 struct _date *date;
 {int i,n;
  if (*date->month == 'T') return(datect(&TODAY) + date->year[1] - 48);
  n = 365 * (atoi(date->year) - 80);
  for(i=0;i<atoi(date->month)-1;i++)
    n += DAYSPERMN[i];
  n += atoi(date->day) -1;
  return(n);
  }


			/* Return the number of minutes in a time */
timect(time)
 struct _time *time;
 {return(((*time->pmst ? 12 : 0) + atoi(time->hour)) * 60 +
        atoi(time->minute));
  }




			/* Compare two strings  [alphabetically] */
alpcmp(s1,s2)
 char *s1,*s2;
 {while(1)
    {while(*s2 == '?' && *s1 != 0) {*s1++; *s2++;}
     if (*s2 == '*') return(EQUALS);
     if (*s1 < *s2) return(LESSTHAN);
     if (*s1 > *s2) return(GREATERTHAN);
     if (*s1 == 0) return(EQUALS);
     s1++;
     s2++;
     }
  }
