#########################################################################
#                       Portable MINCE-80 Makefile                      #
#########################################################################

.SUFFIXES: .o

#########################################################################
#                                Settings                               #
#########################################################################

COLS=80
ROWS=24
CFL=-O2 -fcommon -fno-strength-reduce -s -static
CC=gcc $(CFL)
CPP=cpp
SYSTYPE=SYSV
OPTIONS=-D$(SYSTYPE)=1 -DZ80 -DRUNOPTS -DUSEDIRENT
CFLAGS=$(OPTIONS)

#########################################################################

all: mince
	@printf '\r%s\n' "" || true
	@test -x mince 2>/dev/null && size mince 2>/dev/null || true
	@test -x mince 2>/dev/null && \
		printf '\n%s\n\n' \
		" **** MINCE ($(ROWS) rows, cols $(COLS)) build successful. ****"

#########################################################################

ccpu.o: ccpu.c com.h Makefile
	$(CC) $(CFLAGS) -c ccpu.c -o $@

#########################################################################

com.o: com.c com.h Makefile
	$(CC) $(CFLAGS) -c com.c -o $@

#########################################################################

console.o: com.h console.c Makefile

#########################################################################

dir.o: com.h dir.c Makefile

#########################################################################

mince_com.c: mince80/mince.com
	@test -x ./bintoc
	@./bintoc $? mince_com >$@
	@test -f $@

#########################################################################

mince_swp.c: mince80/mince.swp
	@test -x ./bintoc
	@./bintoc $? mince_swp >$@
	@test -f $@

#########################################################################

fakefs_mince.o: fakefs.c mince_com.o mince_swp.o Makefile
	$(CC) $(CFLAGS) -DMINCE -c fakefs.c -o $@

#########################################################################

embed.o: com.c com.h Makefile
	$(CC) $(CFLAGS) -DEMBED -c com.c -o $@

#########################################################################

ccom: ccpu.o com.o console.o dir.o disass.o fakefs.o Makefile
	$(CC) $(CFLAGS) ccpu.o com.o console.o dir.o disass.o fakefs.o -o $@

#########################################################################

mince: ccpu.o console.o dir.o disass.o fakefs_mince.o \
	embed.o mince_com.o mince_swp.o Makefile
	$(CC) ccpu.o console.o dir.o disass.o fakefs_mince.o \
		embed.o mince_com.o mince_swp.o -o $@

#########################################################################

mince80/mince.swp: ccom
	@test -x ./termset
	@$(shell printf '%s\n' "export ROWS=$(ROWS) && \
		export COLS=$(COLS) && ./termset")
	@test -f mince80/mince.swp

#########################################################################

.PHONY: all clean prereq

#########################################################################

clean:
	rm -f -- ccom *.[soL] *.s1 a.out core *~ *.map benchmark \
		mince mince_com.* mince_swp.* mince80/mince.swp

#########################################################################
