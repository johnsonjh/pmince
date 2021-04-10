Pageheading(Odd, Left "Scribble Test Document", Right "AUG 08/81 (Odd)",
Value(Page)")
Pageheading(Even, Right "Scribble Test Document", Left "AUG 08/81 (Even)",
Value(Page)")
title(chapter)", Right "Right")
title(chapter)", Right "Right")
MajorHeading(Preface)
index(Instructions)
prefacesection(Purpose of this Manuscript)
This Manuscript is meant to exercise most of the features of Scribble
and Crayon to aid you in testing Scribbles and Crayons that you
develop. 
prefacesection(Creation Notice)Scribble and Crayon Test Manuscript by
begin(itemize)Created for Amethyst User's Group
August 13, 1981. [BAD]

Modified to remove some bugs in the formatting commands, generally
cleaned up the contents. Added some extra text so that Scribble and
Crayon problems I am aware of will be pointed up. September 5, 1981. [BAD] 

Much of the text was rearranged to more closely match the order of
certain sections of the new (1.3) Scribble manual. Several new
commands were added to the text. September 6, 1981. [BAD]
end(itemize)

Include(CON:) 
Include(CON:) at 
Include(CON:).

With this version of Scribble: 
include(CON:)

And with this version of Crayon: 
include(CON:)

include(CON:)
prefacesection(Usage)Once you get Scribble and Crayon configured to
work with your printer(s) run this manuscript thru Scribble/Crayon
(once for each printer type you have defined). Undoubtedly there will
be botches. Hopefully all of the botches will be due to improper
specifications in the CONFIG.DAT file. Once you get things working
right with this file, put a copy of each of the CORRECT outputs you
have created in a (safe) and handy place for future reference. This
will constitute the standard of comparison for the new
Scribbles/Crayons you will produce and submit to AUG. Hopefully all
future submissions for Scribble/Crayon extensions will be able to
format this manuscript as flawlessly (?) as the factory version(s). 

As you produce new environments, features and facilities, put tests of
them in this manuscript and try running it thru your new system. This
will give you a resonably effective test to see if your new features
work, and if they inadvertently gronk any other ones. 

This file is long for a reason: short files are not likely to exercise
Scribble or Crayon too hard. Weird things can often happen on the 25th
page (only if it's the second page of the index) that you would never
catch until you ran that all-important document off (and distributed
100 copies of it, before catching it). If you exercise all of the new
and old features at once (or at least most of them) you are more
likely to spot those bizarre 'interaction type' bugs while you are
still programming, and before you start trying to use the system.

This manuscript can also help you set up the CONFIG.DAT
file properly for your printers. I didn't get the paper size for my
printer just right until after I had been using Scribble/Crayon for
over four months.
Chapter(Environments)
section(Address Environments)
Address Environment
address(The Amethyst Users Group
Post Office Box 8173
index(Four Digit Zip Code)
section(Center Environments)
center(All of this line should be centered
as this one should be also,
index(Center)
index(Closing)
Closing environment.
it should appear identical
Address}
Index(Comment)
comment(this had better not
show up) This sentence follows the comment.
index(Description)
Description environment :
description{
\You may have trouble formatting this Manuscript with your
Scribble, since I use a 64K system, and I have made extensive use of
the features of Scribble that require lots of memory. It is possible
that you will not be able to use this manuscript if you have 56K, and
almost certain if you only have 48K. In Versions of Scribble 1.3 (and
later, I presume) you can specify how much space to reserve for end
stuff. This feature may well save you. This manuscript uses about 4400
bytes for the largest page and 2900 bytes for the end space when
formatted for a vanilla device.

\Maybe indented some more, maybe not.
\In the official Scribbles and Crayons this sentence is NOT indented
even further than the previous sentence.}
Index(Display)
Display:
display(
this is a test line.
this is another.)
which concludes the testing.
index(Enumerate) Note that most "failures" for
index(Personal Computers) are either shipping
damage problems (connectors shaking loose), or operator errors. Common
operator malfunctions are 
enumerate[expecting the $39 microverter to
also modulate the sound so your TV set makes noise (it doesn't, and it
index(RF Modulator)

having dead batteries in the RF modulator (since it is easy to forget
to turn off, it is easy to kill the batteries in it).]

using the sound connector instead of the D/A connector (or vice versa,
or confusing any of the RCA phono connectors)

index(Shifting)

using a trashed diskette (a diskette will nearly always get trashed if
index(Trashing Disks)

not turning one of the units (Disk, CPU, Monitor) on}
Index(Example)
Example:
example(
this is a test line.
this is another.)
which concludes the testing.
Index(FlushLeft)
FlushLeft:
FlushLeft(
this is a test line.
this is another.)
which concludes the testing.
Index(FlushRight)
FlushRight:
FlushRight(
this is a test line.
this is another.)
which concludes the testing.
index(Format)
Format environment. None of the
telephone numbers are real. Some of the people tell me that they
aren't very real either.
begin(format)
        Allison's Work          454-2086
        Jerry                   859-2921
        Linda                   555-5691
        Time & Weather          545-1411
        Andrew's Lab            541-4403
        Andrew's Home           544-5012
        Physics Laboratory      541-5659
        Physics Office          541-8386
        UNIX                    545-5115
        SPRINT                  545-4443
end(format)
index(Itemize) 
These are some of the the things you
index(Dinner)
index(Extreme Illness)

index(Leg of brown tree toad)

index(Curry Powder)

index(Rock Salt)

index(Roach poison concentrate)]

index(Flour)

index(Lard)

index(Water)

index(Naragansett Beer)]

index(Tomato)

index(Ragu)

index(Baby Gouda Cheese)

index(Fresh Zucchini)

index(Vienna Sausages)]}
Section(Level Environments)
index(Level)
begin(level)
Level environments.
begin(level)
Level environments.
begin(level)
Level environments.
begin(level)
Level environments.
end(level)

Level environments.
begin(level)
Level environments.
end(level)

Level environments.
begin(level)
Level environments.
end(level)

Level environments.
end(level)

Level environments.
begin(level)
Level environments.
begin(level)
Level environments.
begin(level)
Level environments.
end(level)

Level environments.
end(level)

Level environments.
end(level)

Level environments.
end(level)

Level environments.
end(level)

index(Quotation)
Index(TV Guide)
B[Special:] The Polish experiment in democracy and its
index(Soviet Union) are analyzed in "Why
Poland Makes Moscow Shudder." According to executive producer Bob
foot(TV Guide Vol.29
string(second=page)} which should be
of absolutely no interest to you whatsoever. Ahem. However it
demonstrates this feature effectively.
index(Undent)
index(BRL) have had DEC-10
index(DEC-20) and -20 experience, and consequently,
many of our fond memories are filled with experience with
index(EMACS) EMACS, a screen oriented code and text editing system
index(MIT). As a result of our fondness for EMACS,
index(Mark of
index(Arlington), Massachusetts
index(Massachusetts), who has developed an extensible EMACS
index(C) C, called Amethyst
index(Amethyst). My and my associates at BRL's enthusiam for Amethyst
is great, and one of the first tasks we would doubtless undertake is
index(MUMBLE-DOS). 

Mark of the Unicorn has indicated to me that they have successfully
index(Unix descendants)
index(RSX-11) RSX-11. 
They claim that
they have the porting process down to where they can come up on a new
machine in a week or less. Indeed, in light of this degree of
portability, it would be silly of us to not try to get it onto any
system we had. 
end(undent)
Section(Verbatim Environments)
index(Verbatim)

index(Two Tabs)
index(Sixteen Spaces)
                this line started with one space and two tabs.

all of the above lines should be indented the same amount if your
index(tabstops) are set at every eight spaces (heh, heh)}
index(Verse)
And this is a piece of poetry:
index[Gold in the Mountain]

Gold in the mountain,
And gold in the glen,
And greed in the heart,
Heaven having no part,
index(Herman Melville)(1819-1891)
string(third = page)]}
index(Inline Environments)
index(Italics)
foot(Boldface)
string(first=page)

foot(Italics)

index(Boldface Italics) 

index(Roman)

P)}

T)

P)}

u)

ux)

un)
index(Subscripts)
index(Munged)
+(Superscripted 1) word in it.
B<Boldface 2 Superscripted>) word in it.
-(Subscripted 3) word in it.
b<Boldface 4 Subscripted>) word in it.
B<Boldface 5 Superscripted>) both.

+(Superscripted 6) word in it.

B<Boldface 7 Superscripted>) word in it.

-(Subscripted 8) word in it.

b<Boldface 9 Subscripted>) word in it.

B<Boldface 10 Superscripted>) both.

I have noted some bizarre problems that may occur if a line has
subscripted in it and the line below has superscripted. In some cases,
it may be possible for Scribble to overprint the things. Messy.
- environments above include text which
indicates which line they belong to, so that it easier to determine.
index(Commands for Reports)
section(General Confusion) Commands that have been used already used
index(Confused?)
index(Footnotes) I have been using the
Value(third) and
Value(page)} directive in this manuscript already.
subsection(Chapter, Section, Subsection and Paragraph)
index(Subsection)
index(Paragraph)
paragraph(Usage)
I have also been using these throughout.
Paragraph(Indications)
This section contains numbered paragraphs, and this paragraph should
have a number of the form N.N.N.N (Four numbers separated by periods).
Index(UnNumbered)
UnNumbered environment.
UnNumbered environment and should appear on a new page.}
Index(Unnumbered Headings)
index(MajorHeading)
index(Heading)
index(SubHeading)
section(Other Stuff)
Indexing, Appendicies, Prefaces, Pageheading and Pagefooting are all used
in this document. They should appear in the proper locations.
subsection(PageFooting and PageHeading)
index(PageFooting and PageHeading)
index(PageHeading Specifics)
The pageheading should be different on odd pages from the heading on
even pages. Specifically, the left and right text should change places
each page. Also the parity of the page  (odd | even) should appear 
someplace in the heading.
index(PageFooting Specifics) 
The center field
should contain the chapter title. The left and right fields should say
left and right, respectively.
chapter(Miscellaneous Commands and General Directives)
index(Style)
Style directive in this test manuscript.
index(Device)
Device directive in this test manuscript.
index(Blankspace)
Blankspace(1 inches)and there should be one inch between this and the previous line.
index(Ref)
Set directive in this test manuscript.
Ref.
index(Value)
Value directives to determine where the footnotes
occured so that references to them could be printed in the section on
footnotes. This Directive was also used in the PageHeading.
index(Title)
title(chapter).
This directive was also used in the PageFooting.
index(Case)
Case directive in this test manuscript.
index(include)
This is the text that you input:
Message(Give me some text and follow it with a <C-Z>)
index(What it is)
Message(Give me more text and follow it with a <C-Z>)
index(What it be)
index(The End)
index(AUG)
Index(Amethyst User's Group) 
appendixsection(Problems)
In Scribble Version 1.0 this Appendix was B even though it was the
first appendix. Even more strange, Scribble 1.0 referenced this
Appendix as A in the table of contents. Seems to work fine in Scribble
1.3.
appendixsection(General)
The Amethyst Users Group (AUG) is run solely to disseminate
information on the Amethyst package, and on systems and environments
in which that package may be run. The AUG is in no way affiliated with
Mark of the Unicorn, the originators of the Amethyst package, however,
Mark of the Unicorn retains a membership in the AUG so that hopefully
nobody is groping in the dark, so to speak. Any charges levied by the
AUG are accurate reflections, to the fullest extent possible, of the
costs incurred in the generation of the service provided. The AUG does
not and is not operated in order to make a profit, however the AUG is
b(not) a non-profit organization, since the term 'non-profit
organization' has specific legal meaning, and the AUG has not
qualified for this label and status. Therefore, donations to, and
index(not tax exempt) 
index(Money)
Current charges are as
follows:
description{
\ For 12 months in the USA, 
index(Iowa).
Persons who inquire about the users group will get one free copy of
the most recent newsletter. In other words, if there is no date on
your label, this is your free newsletter. Or, your initial
subscription is for thirteen months at the price of twelve. 

b(surface) mail everywhere else.

\There are a few hardcopies of Nos 1, 2 & 3 left, for $1.50
each, while they last. 

index(USA), as they become available. Send in your
extensions now!!! Disks will be available in IBM 8", OSI CP/M,
and CDOS 5.25". These are also the only formats I can accept machine
readable submissions in. If this is a problem, call me and we can work
something out.}

index(Donors) 
of viable extensions submitted on
disk get a free disk full of other neat things (hopefully other
submissions) in return. In the event that I get a flood of
submissions, I will need a few reviewers who are willing to rigorously
test submissions before release. Members may, at their discrection,
submit articles for inclusion in the newsletter. If it's a long one, I
index(Scribble)
format commands still
imbedded, since you never use anything else to format with) since I
don't touch-type, and I'll be sorely tempted to edit it to death.

Appendix(Bug Policy) While the Amethyst Users Group is an acceptable
forum for bug reports, I am certain that Mark of the Unicorn would
prefer that glaring errors in Amethyst be reported to them also,
rather than reading about them in the Amethyst Users Group newsletter
index(newsletter) first. Reporting the bug to Mark of the Unicorn
gets it fixed faster. Reporting the bug to Amethyst Users Group helps
the rest of us avoid situations that invoke the bug. So report bugs to
Mark of the Unicorn and Amethyst Users Group, or if you only have one
stamp, send it to Mark of the Unicorn, since they'll see that it gets
fixed, while I can't. I admit that I have been negligent on this count
once or twice, and I vow to abide by my own policy in the future.

index(XYZZY)
comment(this is so that we clear it for the
index and table of contents)
