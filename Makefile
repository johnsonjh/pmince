#########################################################################
#                         Portable MINCE Makefile                       #
#########################################################################

#########################################################################
#                         Detect Operating System                       #
#########################################################################

ifndef $(OS)
 OS=$(shell uname -s 2>/dev/null | \
	tr '[:upper:]' '[:lower:]' 2>/dev/null || \
	true)
endif

#########################################################################
#                              Configuration                            #
#########################################################################

PREFIX=/usr/local
PBIN=bin
CFEXTRA=-pipe
CFEXTRA+=-Ofast -fomit-frame-pointer -march=native \
		-fstack-protector-strong -D_FORTIFY_SOURCE=2

#########################################################################

ifeq ($(OS), darwin)
	COLS?=80
	ROWS?=24
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=clang
	SYSTYPE=BSD
	OPTIONS=-D$(SYSTYPE)=1 -DZ80=1 -DRUNOPTS=1 -DUSEDIRENT=1 -DNOBUFF=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f
	TEST=test
	SIZE=size
	STRIP=strip
	MKDIR=mkdir -p
	CP=cp -f
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#########################################################################

ifeq ($(OS), linux)
	COLS?=80
	ROWS?=24
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=gcc
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DZ80=1 -DRUNOPTS=1 -DUSEDIRENT=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f --
	TEST=test
	SIZE=size --
	STRIP=strip --
	MKDIR=mkdir -p --
	CP=cp -f --
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#########################################################################

.SUFFIXES: $(OBJE)
.PHONY: all clean osconf dep depend strip install

#########################################################################

all: osconf depend coffwrap$(OEXT) mince$(OEXT) strip
	@printf '\r%s\n' "" || true
	@$(TEST) -x ./mince$(OEXT) 2>/dev/null && \
		$(SIZE) ./mince$(OEXT) 2>/dev/null || true
	@$(TEST) -x ./mince$(OEXT) 2>/dev/null && \
		printf '\n%s\n' \
		" **** MINCE ($(ROWS) rows, cols $(COLS)) build successful ****"

#########################################################################

coffwrap$(OEXT): coffwrap.c version.h
	$(CC) $(COFFWRAP) $? -o $@

#########################################################################

ccpu$(OBJE): ccpu.c com.h Makefile
	$(CC) $(CFLAGS) -c ccpu.c -o $@

#########################################################################

com$(OBJE): com.c com.h Makefile
	$(CC) $(CFLAGS) -c com.c -o $@

#########################################################################

console$(OBJE): com.h console.c Makefile

#########################################################################

dir$(OBJE): com.h dir.c Makefile

#########################################################################

mince_com.c: mince80/mince.com
	./coffwrap$(OEXT) $? mince_com mince.com >./$@

#########################################################################

mince_swp.c: mince80/mince.swp
	./coffwrap$(OEXT) $? mince_swp mince.swp >./$@

#########################################################################

fakefs_mince$(OBJE): fakefs.c mince_com$(OBJE) mince_swp$(OBJE) Makefile
	$(CC) $(CFLAGS) -DMINCE -c fakefs.c -o $@

#########################################################################

embed$(OBJE): com.c com.h Makefile
	$(CC) $(CFLAGS) -DEMBED -c com.c -o $@

#########################################################################

ccom$(OEXT): ccpu$(OBJE) com$(OBJE) console$(OBJE) dir$(OBJE) \
	disass$(OBJE) fakefs$(OBJE) Makefile
	$(CC) $(CFLAGS) ccpu$(OBJE) com$(OBJE) console$(OBJE) dir$(OBJE) \
		disass$(OBJE) fakefs$(OBJE) -o $@

#########################################################################

mince$(OEXT): ccpu$(OBJE) console$(OBJE) dir$(OBJE) disass$(OBJE) \
	fakefs_mince$(OBJE) embed$(OBJE) mince_com$(OBJE) \
	mince_swp$(OBJE) Makefile
	$(CC) $(CFLAGS) ccpu$(OBJE) console$(OBJE) dir$(OBJE) \
		disass$(OBJE) fakefs_mince$(OBJE) embed$(OBJE) \
		mince_com$(OBJE) mince_swp$(OBJE) -o $@

#########################################################################

mince80/mince.swp: ccom$(OEXT)
	@$(TEST) -x ./termset
	@$(shell printf '%s\n' "export RM=\"$(RM)\" && \
		export ROWS=$(ROWS) && \
		export COLS=$(COLS) && \
		./termset" "$(OEXT)")
	@$(TEST) -f mince80/mince.swp


#########################################################################

clean:
	$(RM) ccom$(OEXT) *.[soL] a.out *.bak core *~ *.map coffwrap$(OEXT) \
		mince$(OEXT) mince_com.* *.exe mince_swp.* mince80/mince.swp \
		mince68k/mince.swp mince_68k.* *.core *.cob *.obj *.ovr *.alm

#########################################################################

strip:
	$(STRIP) mince$(OEXT) || true

#########################################################################

osconf:
ifeq ($(MINCE_CONFIGURED), 1)
	$(info Configured for $(OS))
else
ifeq ($(OS),)
	$(error Error: OS detection failed; review Makefile)
endif
	$(error Error: No configuration for OS $(OS); review Makefile)
endif

#########################################################################

com.h: version.h Makefile

#########################################################################

version.h: Makefile

#########################################################################

dep: depend

depend:

#########################################################################

install: depend mince$(OEXT) strip
	$(MKDIR) $(PREFIX) || true
	$(MKDIR) $(PREFIX)/$(PBIN) || true
	$(CP) mince$(OEXT) $(PREFIX)/$(PBIN)/mince$(OEXT)

#########################################################################
