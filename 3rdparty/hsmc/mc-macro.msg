  I just discovered that you must quote a <CR> to enter it into a string
inside a macro. This happens because the standard GetArg() function
translates a <CR> into a <NL> (a ^J with the high bit set) for use in the
working buffer. The <NL> character in the macro is put into the keyboard
buffer rather than the working buffer and when it is read in the program
just gets confused (nothing terrible happens but then what you wanted to
happen doesn't happen either).

  There are two ways to get around this: 1)make a special GetArg() function
just for the macro input (that's what Barry Dobyns did!) or 2)throw in
the towel and admit that <CR> must be quoted. Since I've got no room to
do the former I'm going to take the latter way out: You've got to quote
(^Q) a <CR> into a macro.

  Now that I've admitted defeat I'll tell you what you can do to remember
to quote <CR> and at the same time make it easier on the fingers to
finish the macro. Instead of making ^] the macro terminator make <CR> the
terminator. This involves changing the function MMakMacro() from 

MMakMacro()
{
	GetArg("Macro <C-]>: ", '\035', spare, NSPARE);
}

to

MMakMacro()
{
	GetArg("Macro <CR>: ", CR, spare, NSPARE);
} .

  Then you do the usual stuff to put it all together again. Now you won't
forget to quote the <CR> you want to enter because otherwise you'll
terminate the macro. If you decide NOT to make this change just remember
to ^Q your <CR>s. See you, Herb.

