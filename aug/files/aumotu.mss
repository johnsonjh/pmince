Courtesy of Mark of the Unicorn, here is a run-through of some of
the things that are happening.  First is a list of some of the known
bugs and their fixes:

@list{Mince:  It used to be possible to get a buffer name in
uppercase if in a find file the buffer already existed and if you
specified a new name in uppercase.  Subsequent attempts to name that
buffer would fail since buffer names are lowercased before use.  The
way around this is to use the find file to get to the buffer through
its file name.  Fixed in version 2.6.

Mince:  When reading files that have no end-of-file mark (^Z), there is
a possibility that Mince will hang by trying to read until the swap
file is full.  Note that some earlier CP/M utilities will not put in
an end-of-file mark if the file fits exactly in an even number of
sectors.  The simplest way around this is to patch a control-Z into
the file with DDT.  Recent versions of Mince will stop reading within
1K of the physical end of a file without an end-of-file mark.

Mince:  A couple of people have found Mince intolerably slow,
particularly on search and replace operations.  This is most
noticeable on systems with 2 Meg clocks.  Mince version 2.6 has
several routines coded in assemblly language for a speed-up of between
two to ten times, depending on the operation.  

Scribble:  Scribble may print the pagefooting and pageheading
underscored or boldface if text with either of those attributes wraps
a page break.  This is fixed in Scribble version 1.1.

Scribble:  Tabs ina verbatim environment do not work correctly at the
left margin.  What happened is that the first tab stop failed to get
converted from characters to micas,and was a small fraction of a
character from the left margin.  Fixed in Scribble 1.1.

Scribble:  Tab stops are relative to the left edge of the page,
whereas they should be relative to the left edge of an environment.
Fixed in Scribble 1.1.

Scribble:  In Scribble 1.0 virtually none of style commands were
functional.  In Scribble 1.1 some were fixed and some were not.  The
problems appear as either a total lack of results or as inconsistent
results.  Fixed in Scribble 1.2.

Scribble:  There is a little-known environment called "address" in all
versions of Scribble that will do a flushleft o the middle of the
page.  It is handy for use in letters.

Scribble:  Scribble 1.1 will fail to notice a leading tab character on
the second line of text on a page (not counting headings) if the tab
is within a verbatim or example environment.  Fixed in Scribble 1.2.

Scribble:  Scribble is an intensive memory user.  All current versions
require 56K to run at all, and version 1.0 may not even run in 56K.  A
special version of Scribble that runs in 48K will be available on
request in the near future. It lacks the style, note, and index
directives and cannot competently handle many sectioning commands. The
main way to save memory is by making the page smaller or by using
fewer commands that accumulate data to be printed at the end of the
document (i.e. note, sectioning, and index commands).  Scribble 1.2
provides direct control over memory sectioning and reports exact
memory usage.

Crayon:  The NEC Spinwriter proportionally-spaced print thimble does
not have the standard arrangement of characters and so requires a
translation table.  Furthermore, some characters can only be accessed
with shiftout/shiftin sequences.  Proper use of the print thimble
currently requires recoding portions of Crayon.  We will be supporting
this in the near future.  

Crayon:  Crayon 1.1 has a bug concerning the opening of the CONFIG.DAT
file.  The program can only work properly if the selected drive is not
drive A and does not contain a CONFIG.DAT file at the same time as
drive A.  This is simple to fix if you know C; just look at the code
that opens the database file in DRIVER.C and it should be obvious.
Fixed in Crayon 1.2.

Crayon:  When printing on a printer that cannot back up (overprint),
Crayon will complain and refuse to do anyting in most circumstances.
When there is any boldface or underscoring, such as that caused by
chapter or section directives, Crayon will refuse to print on a device
without backup.  Crayon version 1.2 will now warn you that it cannot
back up and continue printing without boldface or underscoring.

Crayon:  Crayon will occasionally leave completely blank pages in the
output.  This happens when the bottom margin is zero; Crayon prints
text on the last line, automatically moves to the next page and sees a
form feed, and thus skips a page.  A short-term fix is to always leave
a bottom margin of at least one line.  Fixed in Crayon 1.2.

Crayon:  The page pause option (for manual sheet feeding) does not
work correctly.  It either fails to finish the page before prompting
for the next, or on occasion funs off the bottom before asking for
another.  This is a complex problem involving the buffering strategy.
Fixed in Crayon 1.2.

Crayon:  It is not possible to use the proportionally-spaced font on
the IDS460 printer.  This is a combination of a bug in Crayon and
extremely poor design on the part of IDS.  There are many revisions of
the IDS460, and only the most recent work at all correctly.  Even the
ones that work as documented waive the property that the print head
must make a separate pass at every word, necessary for Crayon to do
correct microspacing.  This is a noisy and slow process.  Crayon 1.2
allows microspacing on newer IDS460's but the noise persists.  Thanks
go to the IDS staff for suggesting some undocumented features that
made it possible at all.

Crayon:  Crayon 1.2 now supports the Centronics 737 printer correctly.}

Further note from Mark of the Unicorn:

There has been a lot happening around here.  We are just about
to get Mince version 2.6 with partial source code to distributors so
that it will be available on various disk formats.  It is hoped that
Scribble 1.2 will follow quickly, although we are currently having
some trouble with the manual (which has been completely rewritten from
the one everyone already has).

Mince is now available on ONYX UNIX systems for $500.00,
including partial source code on cartridge tapes.  Mince is also
available under RSX-11 and UNIX V7 on PDP-11's for $750.00 on
nine-track magtape.  We are looking for some way to set up better
distribution so that we don't have to deal with all the associated
problems.  

There will be a general mailing of Mince 2.6 and Scribble 1.2 to
all Amethyst owners some day (soon I hope).  All further updates will
be only to people who have returned their license agreement.

No one has found a real bug in Mince in over four months.  With
luck, version 2.6 will be as clean as version 2.5 has been.  We are
currently in the process of designign Mince version three.  This is a
long range project, with completion in a year or so.  Current plans
call for code oerlays, directory access, direct output to a printer,
macros in some form, and so on.  Suggestions are always welcome, but
I'll warn you that we have probably already thought of it.  

Another proposed project is a new kind of word processing
program, in which the commands will have the power of Scribble
commands but everything will happen on the screen in real time as in
Mince.  At the moment is sounds impossible, but then so did Mince when
we started!

