#############################################################################
#                           Portable MINCE Makefile                         #
#############################################################################

#############################################################################
#                           Detect Operating System                         #
#############################################################################

ifndef $(OS)
 OS=$(shell uname -s 2>/dev/null | \
	tr '[:upper:]' '[:lower:]' 2>/dev/null || \
	true)
endif

#############################################################################
#                               Configuration                               #
#############################################################################

PENV=/usr/bin/env
COLS?=80
ROWS?=24
PREFIX=/usr/local
PBIN=bin
CFEXTRA=-pipe
#CFEXTRA+=-Ofast -fomit-frame-pointer -march=native \
#		-fstack-protector-strong -D_FORTIFY_SOURCE=2

#############################################################################
#                   Solaris/OpenIndiana/illumos Configuration               #
#############################################################################

ifeq ($(OS), sunos)
	CFL=-O2 -fcommon $(CFEXTRA)
	CC=gcc
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f
	RMDIR=rmdir
	TEST=test
	SIZE=size
	STRIP=strip
	MKDIR=mkdir -p
	CP=cp -f
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#############################################################################
#                             Haiku OS Configuration                        #
#############################################################################

ifeq ($(OS), haiku)
	UTME=$(shell date +%s)
	MVER=$(shell grep -o '\".*\"' ./version.h 2>/dev/null | \
	    tr -d '\"' 2>/dev/null)
	MARH=$(shell uname -p | sed 's/^x86$$/x86_gcc2/')
	MARU=$(shell whoami 2>/dev/null)
	PENV=/bin/env
	PREFIX=/system/non-packaged
	CFL=-O2 -fcommon $(CFEXTRA)
	CC=$(shell setarch `uname -p` sh -c "command -v cc" 2>/dev/null || \
	    sh -c "command -v cc" 2>/dev/null || \
	    sh -c "command -v gcc" 2>/dev/null || \
	    printf '%s\n' "cc")
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1 -DNOBUFF=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f --
	RMDIR=rmdir
	TEST=test
	SIZE=size --
	STRIP=strip
	MKDIR=mkdir -p --
	CP=cp -f --
	MV=mv -f --
	OBJE=.o
	MINCE_CONFIGURED=1
	EXTRA_MESSAGES="\ \*\*\*\*\ Haiku\ detected\!\ Create\ a\ Haiku\ \
		       Package\ with\ \"$(MAKE)\ hpkg\"\ \*\*\*\*"
endif

#############################################################################
#                             OpenBSD Configuration                         #
#############################################################################

ifeq ($(OS), openbsd)
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=clang
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f
	RMDIR=rmdir
	TEST=test
	SIZE=size
	STRIP=strip
	MKDIR=mkdir -p
	CP=cp -f
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#############################################################################
#                             FreeBSD Configuration                         #
#############################################################################

ifeq ($(OS), freebsd)
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=clang
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f
	RMDIR=rmdir
	TEST=test
	SIZE=size
	STRIP=strip
	MKDIR=mkdir -p
	CP=cp -f
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#############################################################################
#                        Darwin/macOS/OSX Configuration                     #
#############################################################################

ifeq ($(OS), darwin)
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=clang
	SYSTYPE=BSD
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1 -DNOBUFF=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f
	RMDIR=rmdir
	TEST=test
	SIZE=size
	STRIP=strip
	MKDIR=mkdir -p
	CP=cp -f
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#############################################################################
#                            GNU/Linux Configuration                        #
#############################################################################

ifeq ($(OS), linux)
	CFL=-O2 -fcommon $(CFEXTRA)
	CC?=gcc
	SYSTYPE=SYSV
	OPTIONS=-D$(SYSTYPE)=1 -DRUNOPTS=1 -DUSEDIRENT=1
	CFLAGS+=$(CFL) $(OPTIONS)
	RM=rm -f --
	RMDIR=rmdir
	TEST=test
	SIZE=size --
	STRIP=strip
	MKDIR=mkdir -p --
	CP=cp -f --
	OBJE=.o
	MINCE_CONFIGURED=1
endif

#############################################################################

.SUFFIXES: $(OBJE)
.PHONY: all clean osconf strip compress upx install hpkg

#############################################################################

all: osconf mince$(OEXT) strip
	@printf '\r%s\n' "" || true
	@$(TEST) -f ./mince$(OEXT) 2>/dev/null && \
	    $(SIZE) ./mince$(OEXT) 2>/dev/null || true
	@$(TEST) -f ./mince$(OEXT) 2>/dev/null && \
	    $(TEST) -f ./fallback.L 2>/dev/null && \
	    printf '\n%s\n' \
	    " **** Expect failed! 80 rows, 24 cols defaults used! ****" \
		" **** Expect is required for non-default configurations ****" \
	    " **** MINCE (Default: 80 rows, cols 24) build successful ****" || \
	    printf '\n%s\n' \
	    " **** MINCE ($(ROWS) rows, cols $(COLS)) build successful ****"
	@$(TEST) -f ./mince$(OEXT) 2>/dev/null && \
	    printf '\n%s\n' \
	    " **** Run \"$(MAKE) compress\" or \"$(MAKE) install\" now ****" \
	    "$(EXTRA_MESSAGES)"

#############################################################################

cflags.L: osconf

#############################################################################

coffwrap$(OEXT): coffwrap.c version.h cflags.L
	$(CC) $(CFLAGS) \
	    coffwrap.c \
	    -o $@

#############################################################################

ccpu$(OBJE): ccpu.c com.h
	$(CC) $(CFLAGS) \
	    -c ccpu.c \
	    -o $@

#############################################################################

com$(OBJE): com.c com.h
	$(CC) $(CFLAGS) \
	    -c com.c \
	    -o $@

#############################################################################

console$(OBJE): com.h console.c

#############################################################################

dir$(OBJE): com.h dir.c

#############################################################################

mince_com.c: coffwrap$(OEXT) mince80/mince.com
	./coffwrap$(OEXT) \
	    mince80/mince.com \
	    mince_com \
	    mince.com \
	    >./$@

#############################################################################

mince_swp.c: coffwrap$(OEXT) mince80/mince.swp
	./coffwrap$(OEXT) \
	    mince80/mince.swp \
	    mince_swp \
	    mince.swp \
	    >./$@

#############################################################################

fakefs_mince$(OBJE): fakefs.c mince_com$(OBJE) mince_swp$(OBJE)
	$(CC) $(CFLAGS) -DMINCE \
	    -c fakefs.c \
	    -o $@

#############################################################################

embed$(OBJE): com.c com.h
	$(CC) $(CFLAGS) -DEMBED \
	    -c com.c \
	    -o $@

#############################################################################

ccom$(OEXT): ccpu$(OBJE) com$(OBJE) console$(OBJE) dir$(OBJE) \
	    disass$(OBJE) fakefs$(OBJE)
	$(CC) $(CFLAGS) \
	    ccpu$(OBJE) \
	    com$(OBJE) \
	    console$(OBJE) \
	    dir$(OBJE) \
	    disass$(OBJE) \
	    fakefs$(OBJE) \
	    -o $@

#############################################################################

mince$(OEXT): ccpu$(OBJE) console$(OBJE) dir$(OBJE) \
	disass$(OBJE) fakefs_mince$(OBJE) embed$(OBJE) mince_com$(OBJE) \
	mince_swp$(OBJE)
	$(CC) $(CFLAGS) \
	    ccpu$(OBJE) \
	    console$(OBJE) \
	    dir$(OBJE) \
	    disass$(OBJE) \
	    fakefs_mince$(OBJE) \
	    embed$(OBJE) \
	    mince_com$(OBJE) \
	    mince_swp$(OBJE) \
	    -o $@

#############################################################################

mince80/mince.swp: cflags.L termset ccom$(OEXT)
	@$(shell printf '%s\n' "export RM=\"$(RM)\" && \
	    export ROWS=\"$(ROWS)\" && \
	    export COLS=\"$(COLS)\" && \
	    export PENV=\"$(PENV)\" && \
	    export CP=\"$(CP)\" && \
	    $(PENV) sh ./termset" "$(OEXT)")
	@$(TEST) -s mince80/mince.swp

#############################################################################

clean:
	$(RM) ccom$(OEXT) \
	    *.[soL] \
	    a.out \
	    *.bak \
	    core \
	    *~ \
	    *.map \
	    coffwrap$(OEXT) \
	    mince$(OEXT) \
	    mince_com.* \
	    *.exe \
	    mince_swp.* \
	    mince80/mince.swp \
	    mince68k/mince.swp \
	    mince_68k.* \
	    *.core \
	    *.cob \
	    *.obj \
	    *.ovr \
	    *.alm \
	    mince.hpkg \
	    ./hpkg/mince.hpkg \
	    ./hpkg/bin/*
	$(RMDIR) -p ./hpkg/bin 2>/dev/null || \
	    true
	$(RM) ./hpkg/.PackageInfo \
	    ./hpkg/*
	$(RMDIR) -p hpkg 2>/dev/null || \
	    true

#############################################################################

strip: osconf mince$(OEXT)
	$(STRIP) \
	    mince$(OEXT) \
	    2>/dev/null \
	    || true
	$(STRIP) \
	    mince$(OEXT) \
	    --remove-section=.note.gnu.build-id \
	    --remove-section=.gnu.hash \
	    --remove-section=.comment \
	    --remove-section=.gnu.build.attributes \
	    2>/dev/null \
	    || true

#############################################################################

upx: compress

compress: strip mince$(OEXT)
	upx -q -q --exact --strip-relocs=0 --overlay=copy --ultra-brute \
	    mince$(OEXT) || true
	@printf '\n%s\n'  \
	    " **** You may now proceed with \"$(MAKE) install\" ****"

#############################################################################

osconf:
ifeq ($(MAKE),)
	$(error Error: MAKE variable is not set)
endif
	@printf '%s\n' "$(ROWS) $(COLS) $(CC) $(CFLAGS)" | \
	    $(PENV) cmp -s - cflags.L || { \
	    printf '%s\n' "$(ROWS) $(COLS) $(CC) $(CFLAGS)" \
	    > cflags.L; }; printf '%s\n' ""
ifeq ($(MINCE_CONFIGURED), 1)
	$(info Configured for $(OS))
else
ifeq ($(OS),)
	    $(error Error: OS detection failed; review GNUmakefile)
endif
	$(error Error: No configuration for OS $(OS); review GNUmakefile)
endif

#############################################################################

com.h: version.h

#############################################################################

install: osconf mince$(OEXT) strip
	$(TEST) -d $(PREFIX) || \
	    $(MKDIR) $(PREFIX)
	$(TEST) -d $(PREFIX)/$(PBIN) || \
	    $(MKDIR) $(PREFIX)/$(PBIN)
	$(TEST) -f $(PREFIX)/$(PBIN)/mince$(OEXT) && \
	    $(RM) $(PREFIX)/$(PBIN)/mince$(OEXT) || \
	    true
	$(CP) mince$(OEXT) $(PREFIX)/$(PBIN)/mince$(OEXT)
	@$(TEST) -x $(PREFIX)/$(PBIN)/mince$(OEXT) && \
	    printf '\n%s\n' " **** Installation successful! ****" || true
	@$(TEST) -x $(PREFIX)/$(PBIN)/mince$(OEXT) || { \
	    printf '\n%s\n' " **** Installation failed, try manually! ****"; \
	    false; };

hpkg: osconf version.h mince$(OEXT) strip
	@if [ "$(OS)" != "haiku" ]; then \
		printf '\n\n%s\n\n' \
		" **** Error: hpkg target is only for Haiku OS ****"; \
		exit 1; \
	fi
	$(TEST) -d ./hpkg || \
	    $(MKDIR) ./hpkg
	printf '%s\n' \
	    "name \"mince\"" \
	    "version $(MVER)-$(UTME)" \
	    "architecture \"$(MARH)\"" \
	    "summary \"Portable MINCE\"" \
	    "description \"MINCE Is Not Completely EMACS\"" \
	    "packager \"$(MARU)\"" \
	    "vendor \"https://github.com/johnsonjh/pmince\"" \
	    "copyrights \"Various Authors\"" \
	    "licenses \"ISC\"" \
	    "provides { mince = $(MVER)-$(UTME) }" \
	    > ./hpkg/.PackageInfo
	$(TEST) -d ./hpkg/bin || \
	    $(MKDIR) ./hpkg/bin
	$(TEST) -x ./mince$(OEXT) && \
	    $(MV) ./mince$(OEXT) ./hpkg/bin/mince$(OEXT)
	@$(TEST) -d ./hpkg/bin && \
	    cd ./hpkg && \
	    package create -v -b mince.hpkg
	@$(TEST) -d ./hpkg && \
	    $(TEST) -f ./hpkg/mince.hpkg && \
	    cd ./hpkg && \
	    package add -v mince.hpkg bin && \
	    $(MV) mince.hpkg ..
	@$(TEST) -f ./mince.hpkg && \
	    package list mince.hpkg && \
	    ls -la mince.hpkg && \
	    printf '%s\n' \
	    " **** Package \"mince.hpkg\" created successfully! ****"

#############################################################################
