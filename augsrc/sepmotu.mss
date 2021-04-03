heading(State of the Unicorn Address)
Again, curtesy of Mark of the Unicorn, here is the state of affairs
with Mince/Scribble.

Begin(Verbatim)
flushright(8/18/81)
Dear Barry,

Time for the next monthly update on the state of the world around
here. A little late this time but then I was on vacation.

There are only a few problems this month.

Mince 2.6 and possibly older versions as well
have a terrible problem involving page mode used in combination with
two window mode. When you are in two window mode, and there are two
different buffers, one in each windowm and at least one of them is in
page mode, and you switch windows either two or three times without
using other change buffer commands, THEN mince crashes completely.

There is a simple short term solution, don't use two window mode at
the same time as page mode. This problem  will be fixed in versions
distributed after today. People who wish to make use of this feature
may make the following patch.

In the function MSwpWind in the file COMM3.C, between the opening '{'
display[int     cnt;]this will
leave the first few lines of the function looking like:
format[
MSwpWind()               /* Switch which window we are in */
{
        int cnt;

        if (divide>=TMaxRow()-2) return;
        tmp=altstart;          /* switch alt and cur screen start */

] 
After making this change, recompile and link Mince.

Crayon Version 1.2: Version 1.2 of Crayon does not deal correctly with
devices having a special printer code of one, i.e. plain or special
printers. THe problem appears as page numbers in the wrong column,
incorrect overprinting, and various other horizontal alignment
problems.

An accurate description of the problem is that when Crayon goes to
print a character that is both below and to the right of the previous
character printed  it fails to move sufficiently to the right. This
missalignment is corrected on the next line.

This problem will be fixed in Crayon 1.3 and is in fact fixed in some
of the Crayon 1.2's. The best way around this is not to have a plain
printer. There is a code patch as follows. Change the function
VPrtMove in the file DRIVER.C so that the bottom several lines read as
follows, and then recompile and link Crayon. NOTE that this only
applies to Crayon 1.2.
begin(format)
     if (dev.formfeed)
          while (delv > (vpos % pagev)) {
               OPutChar (FF);
               delv -= pagev - (vpos -delv) % pagev;
               }
     while ((delv -= linev) >= 0){
          OPutCmd (&dev.donewline);
          delh=hpos;               /* This is the patch */
          }
     while ((delh -= spaceh) >= 0) OPutChar (' ');
     drvhpos = hpos;
     drvvpos = vpos % pagev;
     }
     
     
FLAG
VInfo (what)                    /* ask about printer */
end(format)

Those are all of the bugs for this month. There is one question that I
would like to answer that everyone keeps asking. There are in fact
several ways for people to get more global space in Mince for use with
personal extensions.

First a little background. We use the -e option in the C compiler to
save space in the resulting programs, this forces the global address
to appear at the specified address regardless of the size of the code.
L2 and CLINK both get this address, along with the size of the global
data area from the first .CRL file linked.  Mince does dynamic storage
allocation in the region between the global data and the stack, thus
the size of the global data area must be accurate. Further there is
more global data than there might appear to be, MINCE.CRL is compiled
with a different version of MINCE.GBL than is distributed, and has
extra global data for the buffer abstraction.

The simplest way to add storage is to have this storage be in the form
of string constants, the compiler allocates string constants in such a
way that you can write over them so long as you don't exceed their
allocated length. This is a fair although uncomfortable programming
practice in all of the compilers I know of (BDS-C UNIX V6 and V7,
ONYX UNIX, Whitesmiths) support it. This can be done as follows:
create a function such as:
begin(format)
Storage()
{
     return("This returns a string of exactly fifty characters     ");
     }
end(format) 
this function can now be used in another place to get a pointer to use
for storage as in:
begin(format)
Munge(arg)          /* Store Arg in Storage */
     char *arg;
{
     if (strlen(arg) > 50) Error ("String too long");
     else strcpy(Storage(),arg);
     Echo("String now is: ");
     TNLPrnt(Storage());
     }
end(format)

Another way to get extra storage is to create some room between the
end of the code and the start of he global data area by elimination of
some code or use of the L*.CRL files. The routines can simply KNOW
that this space is there and use it. This can get tricky to maintain
since the end of the code is likely to change every time you work on
the program. One good thing to do is to have your routine check the
location of the end of the code with the BDS-C function codend().

The final way to get more storage is  little known feature of L2. L2,
unlike CLINK, does nort require that the first function linked to be
the main function. Thus you can compile bindings with extra global
definitions and link bindings and then mince. The trick is that you
have to leave extra space (more than 1000 and less than 1100 bytes,
1100 works) for the globals that you can not see. The only way to know
this number exactly is to put a function in mince that prints the
address of the last visible global and also the value returned by the
BDS-C function that gives the address o the end of the global area.

The long awaited Scribble update is starting this week. The update
will include Mince 2.6 and Scribble 1.3 with a complete and in fact
brand new Scribble manual. It is going to take us close to a month to
get everything sent out due to disk and manual copying times.

closing(Take Care

Jason T. Linhart)

End(Verbatim)
