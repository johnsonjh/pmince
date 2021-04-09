#include "constant.inc"
#include "sys.inc"
#include "db.inc"

			/* Insert the template starting at ifld into buffer */
instmplt(buffer,datlst,ifld)
 char *buffer,*datlst[MAXNLFLDS]; 
 int ifld;
 {char *data;
  data = datlst[ifld];
  switch(FIELD[ifld].tmpltno)
     {case DATE:
	insdate(buffer,data);
	return;
      case TIME:
	instime(buffer,data);
	return;
      case NAME:
	insname(buffer,data);
	return;
      case PHONE:
	insphone(buffer,data);
	return;
      case ADDRESS:
	insaddr(buffer,data);
	return;
      case MONEY:
	insmoney(buffer,data);
	return;
      }
  }


			/* Insert date template into buffer */
insdate(buffer,date)
 char *buffer;
 struct _date *date;
 {if (*date->month == 0) return;
  strcat(buffer,MNSTR[atoi(date->month)-1]);
  ccat(buffer,' ');
			/* Clear away leading 0's from day field */
  strcat(buffer,mknum(date->day,1));
  strcat(buffer,", 19");
  strcat(buffer,date->year);
  }

			/* Insert time template into buffer */
instime(buffer,time)
 char *buffer;
 struct _time *time;
 {if (*time->hour == 0) return;
  strcat(buffer,time->hour);
  if (time->minute[0])   
     {ccat(buffer,':');
      strcat(buffer,mknum(time->minute,2));
      }
  if (time->amst[0]) strcat(buffer," AM");
  else		     strcat(buffer," PM");
  }




				/* Insert a phone template into buffer */
insphone(buffer,phone) 
 char *buffer;
 struct _phone *phone;
 {if (*phone->phnum == 0) return;
  if (*phone->areacd)
    {ccat(buffer,'(');
     strcat(buffer,phone->areacd);
     strcat(buffer,") ");
     }
  strcat0(buffer,phone->phnum,3);
  ccat(buffer,'-');
  strcat(buffer,phone->phnum+3);
  }

				/* Insert a contact template into buffer */
insname(buffer,name)
 char *buffer;
 struct _name *name;
 {if (isdigit(*name->lastname)) return; 
  if (*name->firstname)
    strcat(buffer,name->firstname);
  if (*name->middlename)
    {ccat(buffer,' ');
     strcat(buffer,name->middlename);
     }
  ccat(buffer,' ');
  strcat(buffer,name->lastname);
  if (*name->title2)
    {ccat(buffer,' ');
     strcat(buffer,name->title2);
     }
  } 





				/* Copy address to buffer */
insaddr(buffer,address)
 char *buffer;
 struct _address *address;
 {char newlnflg;
  newlnflg = 0;
  if (*address->organization) 
    {strcat(buffer,address->organization);
     newlnflg = 1;
     }
  if (*address->address1)
    {if (newlnflg) nextline(buffer);
     strcat(buffer,address->address1);
     }
  if (*address->address2)
    {nextline(buffer);
     strcat(buffer,address->address2);
     }
  nextline(buffer);
  strcat(buffer,address->city);
  if (*address->state)
    {strcat(buffer,", ");
     strcat(buffer,address->state);
     }
  strcat(buffer,"  ");
  strcat(buffer,address->zip);
  if (*address->country)
    {nextline(buffer);
     strcat(buffer,address->country);
     }
  }





			/* Move to the position on the nextline 
			    underneath current letter position */
nextline(buffer)
 char *buffer;
 {int i;
  ccat(buffer,'\n');
  for(i=0;i<CARRIAGEPOS-MARGIN;i++) ccat(buffer,' ');
  }



			/* Insert a money template */
insmoney(buffer,money)
 char *buffer;
 struct _money *money;
 {ccat(buffer,'$');
  strcat(buffer,mknum(money->dollars,1));
  ccat(buffer,'.');
  strcat(buffer,mknum(money->cents,2));
  }


			/* Make a number n places long */
mknum(numstr,n)
 char *numstr;
 int n;
 {int i;
  char buf2[MAXDESCSZ];
  itoa(numstr,atoi(numstr));
  *buf2 = 0;
  for(i=0;i<n-strlen(numstr);i++)
    ccat(buf2,'0');
  strcat(buf2,numstr);
  strcpy(numstr,buf2);
  return(numstr);
  }





