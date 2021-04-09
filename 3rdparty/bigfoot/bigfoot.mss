@COMMENT<THIS FILE MUST BE FORMATTED USING PF.COM AND THEN
PRINTED USING PP.COM>
@STYLE<leftmargin 10 chars, paperlength 11 inches, indent 6 chars>
@MESSAGE<FORMATTING FOR DEVICE TYPE 12?  >
@PAGEHEADING<left="BIGFOOT - footnoting program for PLU*PERFECT WRITER">
@pagefooting(center=@value<page>)
@style(spacing 1 line, spread 1 line)
@style(chapters no)
@begin(center)
@u<BIGFOOT 1.0 (CP/M Version)>

USER'S GUIDE

by

Michael A. Covington


Last revised 7 January 1984


Athens Microcomputer Center
2173 West Broad Street
Athens, Georgia 30604
@end(center)
@begin(text)
@section(Introduction)

BIGFOOT is a program that frees you from two of the most
important limitations of Perfect Writer.  In itself, Perfect
Writer prohibits footnotes more than 300 characters long, and the
entire set of footnotes cannot exceed the size of the Perfect
Writer endspace (typically 5000 characters).  BIGFOOT provides a
way of getting around both of these restrictions, so that the
lengths of individual footnotes and the total space occupied by
footnotes is limited only by the disk space available.

BIGFOOT works by creating a rearranged copy of your document
which you can then format and print with Perfect Writer in the
usual way.  BIGFOOT picks up all the footnotes -- marked with the
@@NOTE command, just as if they were to be processed by Perfect
Writer -- and places them in a numbered list either at the end of
your document or in a separate file; at the original location of
each footnote BIGFOOT places a command to print the appropriate
superscript number.

Since the rearranging is done in a copy of your document rather
than in the original, you can make revisions by editing the
original and running it through BIGFOOT again.  In this way you
can add or remove footnotes -- or anything else -- and have the
footnote numbering come out correct in the final version.  You
can also edit the rearranged copy in order to control the fine
details of the format in which the footnotes are printed.

@section(A Note to Beginners)

The remainder of this manual presupposes that you are already
familiar with Perfect Writer.  If not, you should probably put
BIGFOOT aside until you have had some practice using Perfect
Writer by itself.

@section(Using BIGFOOT)

Your BIGFOOT disk is not copy-protected, and we strongly
recommend that you make a copy of it to work with while storing
the original in a safe place.  In what follows, when we mention
your BIGFOOT disk, we mean the working copy, not the original.

BIGFOOT accepts Perfect Writer document files with footnotes
marked with @@NOTE (just as for Perfect Writer); the only
difference is that, in a document to be processed by BIGFOOT, the
footnotes can be of unlimited length.

To activate BIGFOOT, do the following:

(1) Use Perfect Writer to create and save the document that you
want to process.  Mark footnotes with @@NOTE (just as if they
were to be processed by Perfect Formatter).  With BIGFOOT, the
footnotes can be of any length and are always placed in a
numbered list at the end of the document or on a separate file,
not at the foot of the page.

(2) If in Perfect Writer, exit to CP/M.

(3) Remove the Perfect Writer disk from drive A and insert the
BIGFOOT disk.  (Your document disk remains in drive B.)  Type
@ux(@w(A:BIGFOOT)).

(4) BIGFOOT asks you for the name of the file containing the
original document and the name of the file onto which the
rearranged document is to be written.  (The file that is to be
written does not have to exist already; BIGFOOT will create it. 
If, however, a file with the same name exists already, it will be
overwritten.)

For safety, the name of the file that BIGFOOT is to create must
end in '.BFT'; if necessary, BIGFOOT will change the file name
that you type in order to ensure that this is so.  On the other
hand, the file name for the original document cannot end in
'.BFT' (it usually ends in '.MSS').

(4) BIGFOOT asks you whether you want the footnotes included in
the original file.  If so, you are asked to indicate a disk drive
on which BIGFOOT can use some temporary work space; Drive A
(containing the BIGFOOT disk) is usually satisfactory.  If you
want the footnotes written on a separate file, BIGFOOT does not
need work space and instead asks you to name the file on which
the footnotes are to be written.

(5) BIGFOOT displays the numbers of the footnotes that are being
processed (analogous to the page numbers displayed by Perfect
Formatter and Perfect Printer).  The run ends with a
"Reformatting complete" message.

(6) The next step is to get back into Perfect Writer (by placing
the Perfect Writer disk in drive A and typing MENU) and format
and print the newly created file.  For instance, if you had an
original document named MYFILE.MSS and you've just created a
rearranged version called MYFILE.BFT, you should ask Perfect
Writer to format MYFILE.BFT (producing MYFILE.FIN) and then print
MYFILE.FIN.  (Careful here!  If, after formatting MYFILE.BFT, you
just hit RETURN when Perfect Writer asks you what file to print,
Perfect Writer may want to print MYFILE.BFT, which is wrong; you
want to print MYFILE.FIN.)

@section(Important Notes)

(1) Unlike Perfect Writer, BIGFOOT does not assume '.MSS' at the
ends of file names for which the extension is not specified. 
With BIGFOOT, you must type the complete file name (preferably
including drive).

(2) Note that neither BIGFOOT nor Perfect Writer accepts
@@BEGIN(NOTE) or @@END(NOTE).  @@BEGIN and @@END can be used only
with environment format commands such as TEXT and FLUSHLEFT.

(3) Place the @@NOTE marker exactly where you want the footnote
number to appear.  Do not space before it unless you want a space
before the footnote number.

(4) No matter how strange the output from BIGFOOT looks, try
actually running it through the Perfect Writer formatter before
concluding that there is anything wrong with it.  Since BIGFOOT
preserves all line breaks in the original and in addition
introduces a line break at every footnote, the reformatted
document will contain many lines that are shorter than the
original. 

(5) When you ask for footnotes to be included in the reformatted
document, they are placed right at the end of it.  Therefore,
formatting commands intended to affect footnotes can be placed at
the end of the original document; the footnotes will be placed
after them. 

@section(Error Messages)

@flushleft(@u<S-TYPE FILE NOT FOUND ERROR>)
You have asked BIGFOOT to read a file that does not exist. 
Double-check the file name, including the drive specification.

@flushleft(@u<S-FILE WRITE ERROR>)
BIGFOOT is unable to write on the disk that you have asked it to
use.  The disk may be full (use the STAT command to find out), or
something else may be wrong (e.g., drive door not closed).

@flushleft(@u<READ PAST EOF ERROR>) Your original document file
probably contains a footnote that never ends; that is, you have
forgotten the closing bracket after an "@@NOTE(" or the like.  If
you are positive that this is not the problem, look for other
abnormalities of format in your document.

@flushleft(@u<BDOS ERR ON X: SELECT>)
You have asked BIGFOOT to use drive 'X', which does not exist or
is inaccessible.  Press RETURN to return to CP/M.
@end(text)ed to affect footnotes can be placed at
the end of the original document; the footnotes will be placed
after them. 

@