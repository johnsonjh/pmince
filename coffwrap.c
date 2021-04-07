#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "version.h"

int i, cnt;
unsigned int size;

int main(
  int argc,
  char **argv
) {

  if (
    argc != 4
) {
    fputs(
      "Convert binary file to C.\n"
          /*

             The MIT License (MIT)
             Copyright (c) 2015 Ondrej Novak

             Permission is hereby granted, free of charge, to any person
             obtaining a copy of this software and associated documentation
             files (the "Software"), to deal in the Software without
             restriction, including without limitation the rights to use, copy,
             modify, merge, publish, distribute, sublicense, and/or sell copies
             of the Software, and to permit persons to whom the Software is
             furnished to do so, subject to the following conditions:

             The above copyright notice and this permission notice shall be
             included in all copies or substantial portions of the Software.

             THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
             EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
             MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
             NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
             BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
             ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
             CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
             SOFTWARE.

           */
          "\nUsage:\n"
          "        coffwrap <file> <identifier> <name>\n\n",
          stderr
    );
    return 1;
  } else {
    FILE *f = strcmp(
      argv[1], "-"
    ) == 0 ? stdin : fopen(
          argv[1], "rb"
    );

    if (
          f == NULL
    ) {
      fprintf(
            stderr, "Can't open input file '%s'\n",
                argv[1]
      );
      return errno;
    }

    if (
      strlen(
        argv[2]
      ) > 13
    ) {
    fprintf(
      stderr, "Error: filename too long.\n"
    );
      return 1;
    }
	printf(
	  "/* AUTOMATICALLY GENERATED - DO NOT EDIT */\n"
	);
    printf(
      "/* coffwrap %s: %s (%s) */\n",
      VERSION,
      argv[3],
      argv[2]
    );
    printf(
      "char %s_name[13] = {\"%s\"};\n",
      argv[2],
      argv[3]
    );
    printf(
      "unsigned char %s[] = {\"",
      argv[2]
    );

    i = fgetc(
      f
    );
    cnt = 0;
    size = 0;

    while (
      i != -1
    ) {
        size++;

        if (
          i >= 32 && \
		    i != '"' && \
            i != '\\' && \
            i < 128
        ) {
          putchar(
            i
          );
          cnt++;
        } else if (
          i == '"'
        ) {
          putchar(
            '\\'
          );
          putchar(
            '"'
          );
          cnt += 2;
        } else if (
          i == '\\'
        ) {
          putchar(
            '\\'
          );
          putchar(
            '\\'
          );
          cnt += 2;
        } else if (
          i == '\n'
        ) {
        putchar(
          '\\'
        );
        putchar(
          'n'
        );
        cnt += 2;
        } else if (
          i == '\r'
        ) {
          putchar(
            '\\'
          );
          putchar(
            'r'
          );
          cnt += 2;
      } else if (
        i == '\t'
      ) {
        putchar(
          '\\'
        );
        putchar(
          't'
        );
        cnt += 2;
      } else {
        printf(
          "\\%03o",
          i
        );
        cnt += 4;
      }

      if (
        cnt >= 80
      ) {
        printf(
          "\"\n\""
        );
      cnt = 0;
      }
      i = fgetc(
        f
      );
    }
    fclose(
      f
    );
    printf(
      "\\032\\000\"};\n"
    );
    printf(
      "int %s_size = %u;\n",
      argv[2],
      size+2
    );
  }
  return 0;
}
