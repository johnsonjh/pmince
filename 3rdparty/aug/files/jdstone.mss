style(indent=0 char)
pageheading
pagefooting
flushleft[
Dear AUG,
]
blankspace(1 line)
I've made a few changes to Scribble and Crayon.


To Scribble, I've added an "INDENT" environment which indents the
left margin by a half inch.  I also modified Scribble so that
documents can be sectioned without being "chaptered".  This
involved changing the section heading print logic (to omit the
chapter number) and changing the table-of-contents code to not
print chapter numbers.  All of the preceding changes were made as
source changes to AUXIL.C and COMMTOO.C .

I hacked CRAYON up so that it understands the new parameter of
"-page n"  where "n" is an integer page number where CRAYON
should first start printing.  This "n" has nothing to do with the
page numbering scheme, but is simply the ordinal page number as
CRAYON prints pages. I don't understand how CRAYON works, so this
change is a true genuine hack; it is not elegant (in fact, its
hardly decent). The changes occur in files CRAYON.H, CRAYON.C,
and DRIVER.C .  When you specify the "-page" option to CRAYON, it
still passes all the text of the file but doesn't begin printing
until the beginning of the designated page. This means that
printing may not start as quickly as you are used to. Don't
despair, its still faster than printing all those unwanted pages.
Before printing anything, a formfeed is output (I tried to
alleviate this, but was completely unsuccessful).

All changes which I've made to the source files are marked by a
comment of the form: /* jds */   .

Good luck to all who try to use this stuff.

address[
Jeffrey D. Stone
331 Barton Place
Menlo Park,
California 94025
]
newpage
subheading(Editor's Note) 
I have chosen to include only the functions that Jeffrey changed in
Scribble and Crayon 1.0. This should be more than sufficent for anyone
to implement the same changes as Jeffrey has. If this seems
unacceptable to anyone else, find me a better way and we will try it.

display{Barry A. Dobyns
Amethyst User's Group
P.O. Box 8173
Austin, Texas 78712
(512)441-9466}
