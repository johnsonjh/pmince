/* CPM68K.C	This is the CP/M-68K specialized routines.

	Copyright 1982 by Mark of the Unicorn, Inc.

	These imitate all the BDS C functions which the Digital Research
supplied C compiler doesn't have (those which we use, that is). */

/* Notice we DON'T include mince.gbl. */

bios(call,arg)					/* Do a BIOS call on CP/M-68K
							This is a hack because it will
							not accomodate all call forms,
							because "arg" is a char type.
							Done through *BDOS* call #50,
							recommended in DR documentation */
	int call;
	char arg;
{
	struct {					/* BPB: BIOS Parameter Block */
		int fnum;
		long p1, p2;
		} bpb;
	int retcode;

	bpb.fnum=call;
	bpb.p1=arg;
	retcode=__BDOS(50,&bpb);
	return(retcode);
}

Xread(fd,buffer,bytes)
	int	fd;
	char	*buffer;
	int	bytes;
{
	return(read(fd,buffer,bytes*128));
}

Xwrite(fd,buffer,bytes)
	int	fd;
	char	*buffer;
	int	bytes;
{
	return(write(fd,buffer,bytes*128));
}

Xopen(name,mode)
	char	*name;
	int	mode;
{
	return(openb(name,mode));
}

Xcreat(name,mode)
	char	*name;
	int	mode;
{
	return(creatb(name,mode));
}

seek(fd,offset,ptrname)
	int	fd;
	int	offset;
	int	ptrname;
{
	long	roffset;
	long lseek();

	roffset=offset;
	roffset=lseek(fd,roffset*128,ptrname);
	if (roffset<0) return(-1); else return(0);
}

int isalpha(c)
	char	c;
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int isspace(c)
	char	c;
{
	return (c == '\040' || c == '\011' || c == '\012' || c == '\015');
}

int tolower(c)
	char	c;
{
	return (isupper(c) ? c-'A'+'a' : c);
}

int toupper(c)
	char	c;
{
	return (islower(c) ? c-'a'+'A' : c);
}

int isupper(c)
	char	c;
{
	return (c >= 'A' && c <= 'Z');
}

int islower(c)
	char c;
{
	return (c >= 'a' && c <= 'z');
}

int isdigit(c)
	char c;
{
	return (c>='0' && c<='9');
}

inp(n)
	int n;
{
	return(0);
	}

outp(n)
	int n;
{
	return(0);
	}

kbhit()
{
	return(bios(2));
	}

setmem(addr,count,byte)
	char *addr, byte;
	int count;
{
	while (count--) *addr++=byte;
	}

movmem(source, dest, count)
	char *source, *dest;
	int count;
{
	if (source > dest)
		while (count--) *dest++ = *source++;
	else {
		source+=count;
		dest+=count;
		while (count--) *--dest = *--source;
		}
	}


/* END OF CPM68K.C - CP/M-68K special routines */

