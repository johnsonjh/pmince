		   A Small Change to the C Mode by H. Schulz
    				   11/06/82

      It has come to my attention that the changes I introduced into the C
    mode still left that mode using indentation sizes determined by the
    indentcol variable in MINCE. That made it necessary to run CONFIG on the
    .SWP file and force the indentcol variable to a non-zero value at
    startup, or you had to do that each time you wanted to use C mode. The
    unfortunate result of the non-zero default value was that it became a
    pain to use Fill mode for some quick text rather than C program editing
    since you had to go in and change the value of indentcol to zero again.

      The solution to that problem is to make the default value of the
    indentcol variable zero again (yeah, run CONFIG again) and change the
    amount indented to the tab increment value (usually defaulted to 8
    anyway), tabincr. This can easily be done by changing the statement 

    			if (Buff()=='{') 
    				wid += indentcol;
    			else if (Buff()=='}')
    				wid -= indentcol;

    found in the MCIndent() function added to COMM1.C using COMM1.DIF (see
    the file MC-CMODE.DOC or MC-CMODE.DQC) to

    			if (Buff()=='{') 
    				wid += tabincr;
    			else if (Buff()=='}') 
    				wid -= tabincr; ,

    recompiling COMM1.C and relinking the files. Make the changes to the new
    COMM1.C file, NOT the COMM1.DIF file since the checksum will then be
    incorrect and SSED will not make the changes requested.

      Now you can again easily use MINCE for text editing as well as
    programing in C. 

    					Hope you find this useful,
    						Herb Schulz
