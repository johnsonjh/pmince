
struct _date
  {char month[3];	/* numeric, min=1, max=12 */
   char day[3];   	/* numeric, adjacent, min=1, max=31 */
   char year[3];	/* numeric, adjacent, min=0, max=99 */
   };

/*
//
*/


struct _time
  {char hour[3];	/* numeric, min=1, max=12 */
   char minute[3];	/* numeric, adjacent, min=0, max=59 */
   char amst[2];	/* status, adjacent */
   char pmst[2];	/* status, adjacent */
   };

/*
: AM [] PM []
*/


struct _address
  {char organization[61];	/* alphanumeric */
   char address1[51];	/* alphanumeric, on new line */
   char address2[51];	/* alphanumeric, on new line */
   char city[27];	/* alphanumeric, on new line */
   char state[3];	/* alphabetic, adjacent */
   char zip[10];   	/* alphanumeric, adjacent */
   char country[31];	/* alphanumeric, on new line */
   };

/*
Organization: 
Address1: 
Address2: 
City:  State:   Zip: 
Country: 
*/


struct _phone
  {char areacd[4];	/* numeric, min=0, max=999 */
   char phnum[8];	/* numeric, adjacent */
   };

/*
() -
*/


struct _name
  {char firstname[18];	/* alphabetic */
   char middlename[16];	/* alphabetic, adjacent */
   char lastname[21];	/* alphabetic, adjacent */
   char title[15];	/* alphanumeric, on new line */
   char salutation[13];	/* alphanumeric, adjacent */
   char title2[7];	/* alphanumeric, adjacent */
   };

/*
First Name: Middle: Last: 
Title: Salutation: (Dear)  Title2: 
*/


struct _socialsecno
  {char ssnum[10];	/* numeric */
   };

/*
--
*/


struct _money
  {char dollars[5];	/* numeric, min=0, max=9999 */
   char cents[3];	/* numeric, adjacent, min=0, max=99 */
   };

/*
$.
*/
