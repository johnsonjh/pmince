program defin;
{a program to help recover a Perfect Writer .FIN file.}
{It converts all non-printable characters into "~"s}
{11/2/85 by Steve Willett}

const
   CR = 13;
   LF = 10;

type
   string15 = string[15];
   string3 = string[3];
   filemode = (RD, WRT);
   rawfile = file;
   bytebuffer = array[0..127] of byte;


const
   Overwrite : boolean = false;
   Abort : boolean = false;

var
   InFile, OutFile : rawfile;
   Count, ch : byte;
   Buffer : bytebuffer;

function Yes : boolean;
{to get a response to a yes/no question from the keyboard}
   const Prompted : boolean = false;
   var   Answer : char;
   begin
      repeat
         read(Kbd, Answer);
         Answer := upcase(Answer);
         if not (Answer in ['Y','N']) then begin
            write(chr(7));
            if not Prompted then begin
               Prompted := true;
               write(' -enter "Y" or "N", please-');
            end;  {if not Prompted}
         end;  {if not Y/N}
      until (Answer in ['Y','N']);
      writeln(Answer);
      Yes := (Answer='Y');
   end;  {Yes}


function FileOpen(var FL : rawfile; FN : string15; MD : filemode; OW : boolean) : boolean;
{to assign, open and test a file}
   var   Failed : boolean;
         ErrorNumber : byte;
   begin
      assign(FL, FN);
{$I-} reset(FL);    {$I+}
      case MD of
         WRT : begin
                  ErrorNumber := IOResult;
                  Failed := (ErrorNumber=0);
                  if OW then Failed := false;
                  if Failed then begin
                     writeln(FN,' already exists - overwrite?');
                     Failed := not Yes;
                  end;  {if Failed}
                  if not Failed then rewrite(FL);
               end;  {WRT case}
         RD : begin
                 ErrorNumber := IOResult;
                 Failed := (ErrorNumber<>0);
                 if Failed then writeln('Cannot open input file ',FN,' I/O error #',ErrorNumber);
              end;  {RD case - if IOresult<>0}
      end;  {case MD}
      FileOpen := not Failed;
   end;  {FileOpen}


procedure Initialize;
{to initialize variables and query for defaults}

   var   InFileName, Name, OutFileName : string15;
         Ext : string3;
         ch : char;
         Count, Count1 : integer;



   function Control(ch : char) : string3;
   {to convert a char for writing to the screen}
      begin
         if ch<' ' then Control := '^'+chr(ord(ch)+64) else Control := ch;
      end;



   function GetName(var NM : string15; EX : string3) : string15;
   {to return a CPM filename with or without default extension EX}
      var
         Extent : boolean;
         Count, Dot : integer;
         Temp : string15;

      begin
         writeln('Please enter a CP/M standard filename. "',NM,'" is the default');
         writeln('Type ".',EX,'" is assumed if not given explicitly');
         readln(Temp);
         Dot := 0;
         Count := 0;
         while Count<length(Temp) do begin
            Count := Count+1;
            Temp[Count] := upcase(Temp[Count]);
            if Temp[Count]='.' then Dot := Count;
         end;  {while Count<length}
         if Dot<>0 then begin
            NM := copy(Temp, 1, Dot-1);
            EX := copy(Temp, Dot+1, length(Temp)-Dot);
         end  {if Dot<>0}
         else if Count<>0 then NM := Temp;
         GetName := NM + '.' + EX;
      end;  {GetName}

   begin  {Initialize}

      for Count := 1 to 20 do writeln(' ');
      writeln('DEFIN - the file to help remove non-printable characters from');
      writeln('a Perfect Write .FIN file.  Copyright 11/3/85 by Steve Willett');
      writeln('written in Turbo Pascal ver. 2.0 - copyright Borland International');
      writeln('');

      Name := 'STRIP';
      writeln('File name of input file?');
      InFileName := GetName(Name, 'FIN');
      writeln('File name of output file?');
      OutFileName := GetName(Name, 'MSS');

      Abort := not FileOpen(InFile, InFileName, RD, Overwrite);
      if not Abort then
         Abort := not FileOpen(OutFile, OutFileName, WRT, Overwrite);

   end;  {Initialize}



begin   {defin}
   Initialize;
   if Abort then writeln('Aborting') else begin
      while not eof(InFile) do begin
         blockread(InFile, Buffer, 1);
         for Count := 0 to 127 do begin
            ch := Buffer[Count] and 127;
            if not (ch in [CR, LF, 32..126]) then ch := 126;
            write(chr(ch));
            Buffer[Count] := ch;
         end;  {forCount}
         if eof(InFile) then Buffer[127] := 26;
         blockwrite(OutFile, Buffer, 1);
      end;  {while not eof}
      close(InFile);
      close(OutFile);
   end;
end.

