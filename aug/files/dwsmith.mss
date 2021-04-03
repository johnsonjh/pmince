heading(MINCE Extensions
by Dave W. Smith)
Begin(Verbatim)
address(August 7, 1981)
Enclosed is a listing of some additional commands derived from EMACS &
co.  The commands to toggle the case of a character, and to write the
file and exit are straightforward, and should require no additional
discussion. However, the command to find matching delimiters may need
a bit of explanation.

First note that the delimiter to match is extracted from the command
by masking off the meta and Control-X bits. This means that you can
add the command as either C-X-<delim> or M-<delim> with the same
effect. I chose the M-<delim> form because I was used to it from
another editor.

The second point, which may interest those who attempt to use 'MoveTo'
and 'MovePast' from Support.C, is that I rolled my own search code
after choosing not to use 'MoveTo'. One obvious approach to searching
for a delimiteris to use 'MoveTo', passing it a predicate function
that keeps track of matching delimiters on the side, such that the
predicate returns true only when it sees the delimiter that really
matches. This works in most cases, and is a fairly elegant solution.
However, 'MoveTo' checks first to see if you've moved backward to the
start of the buffer, and returns WITHOUT CALLING THE PREDICATE if you
have. Thus if you're searching  for a matching delimiter that happens
to be the first character in the buffer, the search will fail! This
isn't a problem if you're searching forward in the file.

Moral ( Beware of tricky predicates! )

I would be interested in hearing from anyone who has successfully
added an equivalent to the EMACS M-! command (move line to top of
foot(I looked in my EMACS Manual, corresponding to EMACS
version 150, and can't  find M-! or a ^R command that seems to
correspond. -Barry). If you've managed to do it, PLEASE submit the code
to AUG. I've tried a couple of times, but I can't seem to make it work
in all cases.
display(-Enjoy

Dave W. Smith 
450 N. Mathilda, #O-103
Sunnyvale, CA. 94086)
