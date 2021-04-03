/* DIFF.C - Source compare by lines (Differences)

	Copyright (c) 1981 by Mark of the Unicorn Inc.
	Created by Jason T. Linhart  4/29/81
	Updated to version 1.1  7/19/80  JTL

	This program is a simple source compare program that works by
lines.  Two input files, specified on the command line, are commpared
one line at a time for regions of difference.  The comparison is done
one line at a time untill a region of difference is encountered and
then an exaustive search is done for the smallest region of difference
that ends in two lines which are the same.
	The size of regions of matching is printed, the entire contents
of both versions of regions of difference is printed.  The program
also accepts "-p" on the command line to direct the output to the CP/M
list device and "-o <filename>" to direct the output to the named
file.
	The program is written in BDS-C using a special set of defines
that provide an interesting, and perhaps more readable, control
structure.  The program is included in the Scribble package as an
interesting and perhaps usefull "bonus".
*/

#include "bdscio.h"

	/* Universal defines */
#define TRUE	(-1)
#define FALSE	0
#define NULL	0

#define NUL	'\0'
#define CR	'\r'
#define LF	'\n'
#define EOF	'\32'

#define FLAG	char

#define loop		WHILE (1) {
#define endloop	}

#define if		IF (
#define then		) {
#define elseif		} ELSE IF (
#define else		} ELSE {
#define endif		}

#define while		IF (!(
#define until		IF ((
#define remain		)) BREAK

	/* Global variables */
#define BUFFSIZ	8192
#define MAXLINES	50

char iobuff[BUFSIZ], *ofd;	/* Declarations for C buffered output */

struct fbuff {			/* The file buffer structure */
	int fd;				/* C I/O package file descriptor */
	char *eptr, *mptr;		/* pointers into buff */
	char buff[BUFFSIZ];		/* Character buffering */
	struct lbuff {			/* Line buffering */
		char *lptr;			/* Location of line in memory */
		int lhash;			/* Hash of line's contents */
		} line[MAXLINES];
	} f1, f2;			/* Allow for two input files */

	/* Forward function declarations */
char *FMore();
FLAG FEOF(), FMatch();

	/* The program */
main(argc,argv)		/* Source commpare program */
	int argc;
	char *argv[];
{
	printf("DIFF Version 1.1 Copyright (c) 1981 by Mark of the Unicorn Inc.\n");
						/* argument decoding */
	if argc==3 then
		ofd=1;
	elseif argc==4 && !strcmp("-P",argv[3]) then
		ofd=2;
	elseif argc==5 && !strcmp("-O",argv[3]) then
		ofd=iobuff;
		if fcreat(argv[4],ofd)<0 then
			Fatal("Unable to open output file.");
		endif
	else
		Fatal("Usage: 'diff file_one file_two (-p | -o file)'");
	endif
						/* Initialize input files */
	FInit(argv[1],argv[2]);
	fprintf(ofd,"Comparing '%s' with '%s'\n",argv[1],argv[2]);

						/* While input: print a region of difference */
	loop while !FEOF() remain;
		DoRgn();
	endloop

						/* Close files, finish up output */
	FFini();
	fprintf(ofd,"Comparison complete.\n");
	if ofd>3 then
		putc(EOF,ofd);
		fflush(ofd);
		fclose(ofd);
	endif;
	}

DoRgn()				/* Compare a region */
{
	int depth, level, tmp;

						/* First move over matching portion */
	depth=0;
	loop
		until FEOF() remain;
		while FMatch(0,0) remain;

		FNext(1,1);
		++depth;
	endloop

	if depth then fprintf(ofd,"Matched %d lines.\n",depth); endif
	if FEOF() then return; endif

						/* For every line in each file, compare it
							and the next line with every previous
							line in the other file, looking for a
							two line match */
	depth=level=0;
	loop
		while depth<MAXLINES-1 remain;
		until FMatch(level,depth) && FMatch(level+1,depth+1) remain;
		until FMatch(depth,level) && FMatch(depth+1,level+1) remain;

		if level<depth then
			++level;
		else
			++depth;
			level=0;
		endif
	endloop
						/* Files might differ by more than we can
							buffer */
	if depth>=MAXLINES-1 then
		Fatal("Files differ by more than %d lines.",MAXLINES);
	endif
						/* Make tmp be the length in the first file */
	if FMatch(level,depth) && FMatch(level+1,depth+1) then
		tmp=level;
		level=depth;
		depth=tmp;
	endif
						/* Print the region of difference */
	fprintf(ofd,"********************\n");
	tmp=0;
	loop while tmp<=depth remain;
		FPrint(0,tmp);
		++tmp;
	endloop
	fprintf(ofd,"**********\n");
	tmp=0;
	loop while tmp<=level remain;
		FPrint(1,tmp);
		++tmp;
	endloop
	fprintf(ofd,"********************\n");
	FNext(depth,level);
	}

/* The File abstraction */
	/* The 'F' or File abstraction maintains a buffer of two input
          files with special line hash values to allow for very fast
          line comparisons.  An array of MAXLINES lines of information
          is stored, allowing for that much look ahead.  Further the
          input files are buffered BUFFSIZ characters ahead to make
          disk I/O more efficent. */

FInit(name1,name2)		/* Open the input files */
	char *name1, *name2;
{
						/* Open the input files, checking for possible
							errors, initialize the buffer pointers and
							fill the initial line table. */
	if (f1.fd=open(name1,0))<0 then
		Fatal("Unable to open '%s'.",name1);
	endif
	if (f2.fd=open(name2,0))<0 then
		Fatal("Unable to open '%s'.",name2);
	endif
	f1.mptr=f1.eptr=f1.line[0].lptr=f1.buff;
	f2.mptr=f2.eptr=f2.line[0].lptr=f2.buff;
	FFill(0,MAXLINES);
	FFill(1,MAXLINES);
	}

FFill(file,amt)		/* read in amount lines of text */
	int file, amt;
{
	struct fbuff *fdesc;
	int thash;
	char *cptr;

						/* This function fills in amt more lines in
							the line table, lines are read from the
							file buffer and their hash values stored
							in the line table. */
	fdesc=(file==0)? &f1 : &f2;
	loop while amt>0 remain;
		fdesc->line[MAXLINES-amt].lptr=cptr=fdesc->eptr;
						/* Check for end of file */
		if cptr==NULL then
			fdesc->line[MAXLINES-amt].lhash=0;
		else
						/* Calculate hash value */
			thash=0;
			loop
						/* Watch out for using up the buffer */
				if cptr >= fdesc->mptr then
					cptr=FMore(fdesc,MAXLINES-amt);
				endif
				while *cptr!=EOF remain;
				while *cptr!=CR || *(cptr+1)!=LF remain;

				thash += *cptr++;
			endloop
						/* Put nulls at the end of the line so they
							will print correctly */
			if *cptr==EOF then
				*cptr=NUL;
				cptr=NULL;
			else
				*cptr++ = NUL;
				++cptr;
			endif
						/* Save the hash value */
			fdesc->line[MAXLINES-amt].lhash=thash;
		endif
		fdesc->eptr=cptr;
		--amt;
	endloop
	}

char *
FMore(fdesc,last)		/* Read in more of a file */
	struct fbuff *fdesc;
	int last;
{
	struct lbuff *slin, *elin;
	int size, dist, ramt;

						/* This routine reads more text into the data
							buffer, preserving the text pointed to by
							lines that are in use. */
	size=fdesc->mptr - fdesc->line[0].lptr;
						/* First move text to be saved to the begining
							of the buffer */
	movmem(fdesc->line[0].lptr,fdesc->buff,size);
	slin = fdesc->line;
	elin = &fdesc->line[last];
	dist = fdesc->line[0].lptr - fdesc->buff;
						/* Now fix up the existing line pointers */
	loop while slin<=elin remain;
		slin->lptr-=dist;
		++slin;
	endloop
						/* Calculate amount more to be read */
	size = (BUFFSIZ-size)/128;
	ramt=read(fdesc->fd,fdesc->mptr-dist,size);
	if ramt<0 then Fatal("File read error"); endif
	fdesc->mptr+=ramt*128-dist;
						/* Check for end of file condition */
	if ramt<size then *fdesc->mptr=EOF; endif
	return(fdesc->mptr-128*ramt);
	}

FNext(amt1,amt2)		/* Move down amt line in each file */
	int amt1, amt2;
{
						/* The routine moves the files up amt1 and amt2
							lines, moves up the existing lines to
							replace old lines and then read in new
							lines to replace them. */
	movmem(&f1.line[amt1],&f1.line[0],sizeof(f1.line[0])*(MAXLINES-amt1));
	FFill(0,amt1);
	movmem(&f2.line[amt2],&f2.line[0],sizeof(f2.line[0])*(MAXLINES-amt2));
	FFill(1,amt2);
	}

FPrint(file,lev)		/* print a line */
	int file, lev;
{
	struct fbuff *fdesc;

						/* Prints a specified line, prints 'EOF' on
							end of file */
	fdesc=(file==0)? &f1 : &f2;
	if fdesc->line[lev].lptr==NULL then
		fprintf(ofd,"%d> EOF\n",file+1);
	else
		fprintf(ofd,"%d> %s\n",file+1,fdesc->line[lev].lptr);
	endif
	}

FLAG
FMatch(lev1,lev2)		/* compare two lines for a match */
	int lev1, lev2;
{
						/* Returns TRUE if the two lines are the same
							Uses the line hash to speed comarison */
	return(f1.line[lev1].lhash==f2.line[lev2].lhash &&
		!strcmp(f1.line[lev1].lptr,f2.line[lev2].lptr));
	}

FLAG
FEOF()				/* Check for double end of file */
{
						/* Returns TRUE if you are at the end of file
							on both input files */
	return(f1.line[0].lptr==NULL && f2.line[0].lptr==NULL);
	}

FFini()				/* Close the files */
{
	close(f1.fd);
	close(f2.fd);
	}

Fatal(mesg,arg)		/* Print a fatal error message */
	char *mesg, *arg;
{
	printf("Fatal: ");
	printf(mesg,arg);
	printf("\n");
	exit(1);
	}

/* END OF DIFF.C - source commpare program */

