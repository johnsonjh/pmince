heading{Letter from Oscar Goldman, 7-15-81:}

The first issue of the newletter mentions the problem in Crayon
which arises when the pause option is used with a printer that sends
out a line only after receiving a line feed. (My Epson MX-80 is one such.)

Clearly the solution is to supply the necessary line feed.  One
place to do this is in the function 'PagePause' in the file Crayon.C .
As it is now, that function looks like:

verbatim[ PagePause()
           {            /* pause, get fresh page from user */
      			    if (PrVPos () != 0) PrFF;
			    PrFlush();
			    puts ("Insert fresh page; type any character when ready -->");
			    getchar();
		}]

Using Mince (of course), edit the file by adding the line

verbatim[    OSend ('/012');]

just after the line 'PrFlush ();'.  Then compile and relink (the
pieces to link are in the file cc.sub).

By making this change, if the pause option is used, the last
thing sent to the printer after printing a page is the needed line
feed.  The extra line feed does no harm; it will merely help push the
paper out of the printer!  Also, if the pause option is not used, then
the function PagePause is not called, and the change has no effect.
Care should be taken not to use the pause option with continuous
forms, because then the extra line-feed will make a mess.

As far as I have been able to determine, there are no undesirable
side effects arising from this modification.  However, this fix is not
very elegant, since it modifies Page Pause even for printers that
don't have the buffer problem (?) of the Epson.  I suppose that in
some future version on Crayon, the creators of Amethyst will make the
required modification in the printer configuration routine.

display{Oscar Goldman
1221 Knox Road
Wynnewood, PA  19096}
