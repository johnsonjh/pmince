pageheading(left "Notes on AUG Disk # 1 by B. Dobyns")
This disk isn't nearly as full as I had intended it to be, but the
release is occuring now, so that everyone can begin to share the
benefits of the code contained herein. 

There should be little trouble getting most of these functions to work
in your Amethyst package, with the exception of Jeffrey Stone's and
Oscar Goldman's Scribble/Crayon hacks. These are strictly for
Scrobble/Crayon 1.0 only and will not work elsewhere.

For Mince extenders, most of the Mince source is very liberally
commented, and easy to use. You may have some trouble geting some of
these things going in your Mince, though, if you have not tried any
extenson yet. 

I have included SD for two reasons: One, they both work under
CP/M 2.2 (while many older extended directory listers don't). Two, I
am planning on turning one into an overlay for Mince (heh, heh).

Don't forget that if you #define LARGE in MINCE.GBL you can get two
more modes 'Save' and 'C'. Neither of these are really 'complete' yet,
but the code for them is scattered about the source files, disabled by
#ifdef LARGE ... and you can be tinkering with them now. 

Hereare some notes on how to go about extension:

If the extensions that you want on this disk are already
built into a Bindings.C file (recognizable by the presence of
finit1(), finit2(), finit3(), SetModes() and UInit() in the file) then
you merely need to pip the file over to a disk with the rest of the
Mince code on it, and edit MC.SUB or LMC.SUB to reflect the new name
of the bindings source file. Take out the old references to Bindings.C
and put in the new ones (e.g. Demo.C). Submit (L)MC and lean back. You
should get a new Mince shortly. 

Note that you can have lines in a submit file that start with a ';'
and these are taken by CP/M (all versions) to be comment lines, and
are printed on the terminal, but ignored. Comment lines in your submit
files can greatly ease the pain of figuring out what the little
nasties do weeks later (especially when you named it with a VERY
arcane name (QQX.SUB)). 

If you do not find that the code that you want is already in a
Bindings file, you must do some work. Take your current Bindings.C and
edit it thusly: Take the functions you want and place them at the end
of the file, just before the /* END OF mumble.C */. Place a
modification notice at the head of the file. If this loses you, look
at what I did (in MACBIND.C) for an example of one way to go about
this.  Decide which command names (the letters you type on the
keyboard) you want to go with which command. Look in the file for the
bindings similar to it (all the C- commands are in finit1, all the M-
in finit2, all the C-X in finit3). Near the the beginning of finitX
there will be a number of function declarations. Put the new functions
in after the ones there (e.g. int MMumble();) Put the assignments for
the function names in the body of finitX, preserving the order. Once
again, if this loses you, look at the way I did it.  

Once you have modified your bindings, you may find that there is
insufficient space for all of the code to fit before the externals.
You can comment out page mode in the Bindings, and this will give you
some space, but the functions will still be linked in by L2. To insure
that the page mode (or any other) functions you never reference will
NOT be linked in, edit them out of COMMx and into another file. Put
this file after the -l on the L2 command line. I provide a sample of
this, COMM4.C in which I have placed all the page mode functions, and
the MSearch and MRSearch (I wrote new ones). If I include it after the
-l on the L2 line (with appropriately modified COMM(1-3)), when
compiled with the 'factory standard' bindings, I get a 'factory
standard' mince. When compiled with a bindings without references to
these functions, they do not get linked in, and save me space (in
which I can do other things.)

I keep all of my .CRL files on a separate disk in B:, and keep a
mince, all the source code, the C compiler, and L2 on another in A:.
This way i have enough space to get things done in. I have provided a
few submit files that i use so that you can see how i have things
organized and how i get new minces built.

address{22 September 1981

Barry A. Dobyns, Editor, Slave &c.
Amethyst Users Group
P.O. Box 8173
Austin, Texas 78712-8173
(512)441-9466}
