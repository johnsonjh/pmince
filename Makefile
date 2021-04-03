#########################################################################
#                         Portable MINCE Makefile                       #
#########################################################################

.SUFFIXES: .o

ifndef $(OS)
 OS=$(shell uname -s 2>/dev/null |\
	tr '[:upper:]' '[:lower:]' 2>/dev/null || \
	true)
endif

#########################################################################
#                              Configuration                            #
#########################################################################

ifeq ($(OS), darwin)
	COLS=80
	ROWS=24
	CFL=-O3 -fcommon
	CC=clang $(CFL)
	CPP=clang
	SYSTYPE=BSD
	OPTIONS=-D$(SYSTYPE)=1 -DZ80 -DRUNOPTS -DUSEDIRENT
	CFLAGS=$(OPTIONS)
	RM=rm -f
	TEST=test
	SIZE=size
	MINCE_CONFIGURED=1
endif

ifeq ($(OS), linux)
	COLS=80
	ROWS=24
	CFL=-O2 -fcommon
	CC=gcc $(CFL)
	CPP=cpp
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DZ80 -DRUNOPTS -DUSEDIRENT
	CFLAGS=$(OPTIONS)
	RM=rm -f --
	TEST=test
	SIZE=size
	MINCE_CONFIGURED=1
endif

#########################################################################

all: osconf depend mince$(OEXT)
	@printf '\r%s\n' "" || true
	@$(TEST) -x ./mince$(OEXT) 2>/dev/null && \
		$(SIZE) ./mince$(OEXT) 2>/dev/null || true
	@$(TEST) -x ./mince$(OEXT) 2>/dev/null && \
		printf '\n%s\n' \
		" **** MINCE ($(ROWS) rows, cols $(COLS)) build successful ****"

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
	@$(TEST) -x ./bintoc
	@./bintoc $? mince_com >$@
	@$(TEST) -f $@

#########################################################################

mince_swp.c: mince80/mince.swp
	@$(TEST) -x ./bintoc
	@./bintoc $? mince_swp >$@
	@$(TEST) -f $@

#########################################################################

fakefs_mince.o: fakefs.c mince_com.o mince_swp.o Makefile
	$(CC) $(CFLAGS) -DMINCE -c fakefs.c -o $@

#########################################################################

embed.o: com.c com.h Makefile
	$(CC) $(CFLAGS) -DEMBED -c com.c -o $@

#########################################################################

ccom$(OEXT): ccpu.o com.o console.o dir.o disass.o fakefs.o Makefile
	$(CC) $(CFLAGS) ccpu.o com.o console.o dir.o disass.o fakefs.o -o $@

#########################################################################

mince$(OEXT): ccpu.o console.o dir.o disass.o fakefs_mince.o \
	embed.o mince_com.o mince_swp.o Makefile
	$(CC) $(CFLAGS) ccpu.o console.o dir.o disass.o fakefs_mince.o \
		embed.o mince_com.o mince_swp.o -o $@

#########################################################################

mince80/mince.swp: ccom$(OEXT)
	@$(TEST) -x ./termset
	@$(shell printf '%s\n' "export ROWS=$(ROWS) && \
		export COLS=$(COLS) && \
		./termset" "$(OEXT)")
	@$(TEST) -f mince80/mince.swp

#########################################################################

.PHONY: all clean osconf com.h dep depend version.h

#########################################################################

clean:
	$(RM) ccom$(OEXT) *.[soL] *.s1 a.out *.bak core *~ *.map benchmark \
		mince$(OEXT) mince_com.* *.exe mince_swp.* mince80/mince.swp

osconf:
ifeq ($(MINCE_CONFIGURED), 1)
	$(info Configured for $(OS))
else
ifeq ($(OS),)
	$(error Error: OS detection failed; review Makefile)
endif
	$(error Error: No configuration for OS $(OS); review Makefile)
endif

com.h: version.h Makefile

version.h: Makefile

dep: depend

depend:

#########################################################################
