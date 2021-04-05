/*
**  "Portable" MINCE by Jeffrey H. Johnson <trnsz@pobox.com>
**
**  Based on the CP/M 2.2 (8080/Z-80) simulator for UNIX systems
**   by Jim Cathey, Edmund Ramm, Charlie Gibbs, and Willi Kusche
**
**  Usage: mince [-<A..P> path ...] [-b] [-c] [-d] [-i infile] [-v] [-V]
**             [-l listfile] file[.com] [args ...]
**
**  Where -b turns off buffering of output, -c makes an 8080 core file
**  upon error, -d inhibits the DEL to BS conversion on keyboard input,
**  -l listfile hooks up CP/M's list output to the named file, and -v
**  turns off the H19 to VT100 translation. The -A to -P options give
**  UNIX paths for each of CP/M's 16 logical drives.  The -i option
**  takes input from the given file instead of standard in until EOF,
**  whereupon it reverts to standard in.
*/

#include "com.h"

int debugoutput; /* For making sure debugout has NL at end. */

int main(argc, argv) int argc;
char *argv[];
{
  char ch, *filename, *argp, linebuf[256];
  unsigned char *tpa;
  struct regs *regp;

  if (!(tpa = malloc(TPASIZE + TPAPAD + sizeof(struct regs))))
    exit(2);
  memset(tpa, 0x00, TPASIZE + TPAPAD + sizeof(struct regs));
  gregp = regp = (struct regs *)tpa;
  tpa += sizeof(struct regs); /* Allocate Pseudo-CPU storage. */
  regp->vtstate = idle;       /* Fill in what we must right now. */
  regp->listfd = -1;
  holdingchar = -1;
  if (!(regp->seed = time(0)))
    regp->seed = 1;
  infilen = 0;
  infile = 0; /* STDIN by default. */

  filename = 0;
  progname = *argv;
  while (--argc > 0) {
    argp = *++argv;
    if (*argp == '-') {
      argp++;
      switch (ch = *argp++) {
#ifndef EMBED
      case 'A':
      case 'B':
      case 'C':
      case 'D': /* For Drive/User */
      case 'E':
      case 'F':
      case 'G':
      case 'H': /* assignment. */
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
        if (!*argp)
          --argc, argp = *++argv;
        drives[ch - 'A'] = argp;
        break;
#endif

      case 'b': /* No buffering. */
        regp->miscflags |= NOBUFFER;
        break;

      case 'c': /* Make 8080 core file on error. */
        regp->trcoptions |= TRCCORE;
        break;

      case 'd': /* No DEL->BS conversion. */
        regp->miscflags |= NODELTOBS;
        break;

      case 'i': /* Use file instead of standard input. */
        if (!*argp)
          --argc, argp = *++argv;
        infilen = argp;
        break;

      case 'l': /* List (print) output to file. */
        if (!*argp)
          --argc, argp = *++argv;
        regp->listname = argp;
        break;

      case 'v': /* No H19->VT-100 conversion. */
        regp->vtstate = off;
        break;

      case 'V': /* Version printout. */
        linebuf[0] = '\0';
        strcat(linebuf, "\rPortable MINCE ");
        strcat(linebuf, VERSION);
        strcat(linebuf,
               " by Jeffrey H. Johnson <trnsz@pobox.com>\r\n\nMINCE 2.6: "
               "Copyright (C) 1980-1982 Mark of the Unicorn, Inc.\r\n\nCCOM80 "
               "3.01: Adapted from COM 3.0, Copyright (C) 1984-2006\r\n    Jim "
               "Cathey, Edmund Ramm, Charlie Gibbs, & Willi Kusche.\r\n");
        write(1, linebuf, strlen(linebuf));
        exit(0);
        break;

      case 'x': /* For debugging. */
        regp->miscflags |= NOIOCTL;
        break;

      default:
        goto usage;
      }
    } else {
#ifdef NOBUFF
      regp->miscflags |= NOBUFFER;
#endif
#ifdef EMBED
      if (filename = strrchr(argp, '/')) {
        *filename++ = 0;
        drives[0] = argp;
      } else
        filename = argp;
      *argv = filename; /* Leave basename in argv. */
#else
      filename = argp;
      argc--;
      argv++;
#endif
      break;
    }
  }
#ifdef EMBED
  if (filename == progname) { /* Always false!*/
#else
  if (!filename) {
#endif
    char *cp, *cp2;
  usage:
#ifdef EMBED
    cp = "";
    cp2 = " <file>";
#else
    cp = "[-<A..P> path ...] ";
    cp2 = " cpmfile[.com] [args...]";
#endif
    write(2, linebuf,
          mysprintf(linebuf,
                    "Usage: %s %s[-b] [-c] [-d] "
                    "[-i infile] [-l listfile] [-v] [-V]%s\n",
                    progname, cp, cp2));
    exit(1);
  }
  if (infilen)
    redirectin(infilen);
  if (tpa) {
    if (loadcom(filename, tpa + LOADADDR)) { /* Load program. */
      loadfdos(tpa, regp); /* Set up Pseudo-CPU registers & memory. */
      parseargs(tpa, argc, argv);
      setupstdin(regp);
      regp->running = 2;    /* Run continuously. */
      opcodesimulator(tpa); /* Run the Pseudo-CPU. */
      if (regp->pppc && regp->badflag)
        dumplast(tpa, regp, linebuf);
    } else {
      write(2, linebuf,
            mysprintf(linebuf, "%s: Can't open %s\n", progname, filename));
      exit(3);
    }
  }
  doexit(0);
}

/*
** Convert remaining Unix command-line arguments to CP/M's strange
** pre-filled FCB's and command tail.
*/

void parseargs(tpa, argc, argv) unsigned char *tpa;
int argc;
char *argv[];
{
  int i, len, arg;
  char *cp;
  struct regs *regp;

  regp = (struct regs *)(tpa - sizeof(struct regs));
  if (regp->trcoptions & TRCCOUNT) {
    write(2, argv[-1], strlen(argv[-1]));
    for (i = 0; i < argc; i++) {
      write(2, " ", 1);
      write(2, argv[i], strlen(argv[i]));
    }
    write(2, "\n", 1);
  }
  ufntocpmfn(argc ? argv[0] : 0, &tpa[FCB1]);
  ufntocpmfn((argc > 1) ? argv[1] : 0, &tpa[FCB2]);

  i = 0; /* Make up command tail. */
  cp = (char *)&tpa[DEFDMA + 1];
  arg = 0;
  while (argc-- && i < 126) {
    *cp++ = ' ';
    len = strlen(argv[arg]);
    if (len + i > 126)
      len = 126 - i;
    strncpy(cp, argv[arg++], len);
    cp += len;
    i += len;
  }
  if (i)
    tpa[DEFDMA] = i + 1;
  else
    tpa[DEFDMA] = 0;
}

/*
** Convert a Unix filename to a CP/M FCB name.
*/

void ufntocpmfn(name, fcb) char *name;
unsigned char *fcb;
{
  char *cp, chr;
  int i;

  fcb[0] = 0; /* Default drive. */
  memset(&fcb[1], ' ', 11);
  if (name) {
    if (cp = strchr(name, ':')) { /* Drive letter? */
      chr = cp[-1];
      fcb[0] = chr & 0x0F;
      name = cp + 1;
    }
    i = 8;
    if (cp = strchr(name, '.')) {
      i = cp - name;
      if (i > 8)
        i = 8;
    }
    strncpy((char *)&fcb[1], name, i);
    if (cp = strrchr(name, '.'))
      strncpy((char *)&fcb[9], cp + 1, 3);
    for (i = 1; i < 12; i++) {
      chr = fcb[i];
      if (!chr)
        chr = ' ';
      if (chr >= 'a' && chr <= 'z')
        chr -= 'a' - 'A';
      fcb[i] = chr;
    }
  }
}

/*
** Signal catcher, so we can fix up things before exiting.
*/

void catcher(signum) int signum;
{ doexit(0); }

/*
** Signal catchers, for trace flag manipulation.
*/

void u1catcher(signum) int signum;
{
  if (gregp)
    gregp->trcflag = 1;
  if (gregp2)
    gregp2->trcflag = 1;
  signal(SIGUSR1, u1catcher);
}

void u2catcher(signum) int signum;
{
  if (gregp)
    gregp->trcflag = 0;
  if (gregp2)
    gregp2->trcflag = 0;
  signal(SIGUSR2, u2catcher);
}

/*
** Bail out, cleaning up before we go.
*/

void doexit(code) int code;
{
  flusher(0);     /* Make sure all output is done. */
  restorestdin(); /* Fix the terminal. */
  if (debugoutput)
    write(2, "\n", 1);
  exit(code); /* We be gone! */
}

/*
** Make an 8080 core file, so to speak.  For debugging.
*/

void docore(tpa, regp) unsigned char *tpa;
struct regs *regp;
{
#ifndef EMBED
  int fd;

  if ((fd = creat("8080.core", 0666)) >= 0) {
    write(fd, tpa, TPASIZE);
    write(fd, regp, sizeof(*regp));
    close(fd);
  }
#endif
}

/*
** Load the .COM program into memory.
*/

int loadcom(name, where) char *name;
unsigned char *where;
{
  int fd, fsize, i;
  char *cp, *tail, fnambuf[1024];

#ifdef EMBED
  if (nfakefiles) {
    memcpy(where, fakefs[0].data, *fakefs[0].size);
    return 1;
  }
#else
  tail = ".com";
  strncpy(fnambuf, name, sizeof fnambuf);
  if (cp = strchr(name, '.')) {
    if (strcmp(cp, tail))
      strcpy(cp, tail);
  } else
    strncat(fnambuf, tail, sizeof(fnambuf) - 1);
  if ((fd = open(fnambuf, 0)) >= 0) {
    fsize = lseek(fd, 0L, 2);
    lseek(fd, 0L, 0);
    if (fsize <= (TPASIZE - LOADADDR - FDOSSIZE - 2)) { /* Too big? */
      while (fsize > 0) {                               /* No, load it. */
        i = read(fd, where, fsize);
        if (i < 0)
          exit(2);
        fsize -= i;
      }
      close(fd);
      return 1;
    }
    close(fd);
  }
  return 0;
#endif
}

/*
** The fake FDOS (BIOS & BDOS) for the 8080.  This is mostly a jump
** table stashed at the high end of memory.  It calls the simulator's
** service routine by using the HLT instruction followed by an opcode.
** Routines marked '@@' aren't implemented, partly for safety, partly
** due to laziness, and mostly due to lack of need.  The BIOS part
** is required to be a jump table, so this array is twice as big
** as would otherwise be needed.  (The original design wasn't a
** jump table, and it failed miserably.)
*/

unsigned char fdos[] = {
    0x76, 0x00, 0xC9, /* Fake BDOS for target system. */
                      /* BIOS Jump Table */
    0xC3, 0x33, 0xFF, /* Wboot */
    0xC3, 0x36, 0xFF, /* Const */
    0xC3, 0x39, 0xFF, /* Conin */
    0xC3, 0x3C, 0xFF, /* Conout */
    0xC3, 0x3F, 0xFF, /* List */
    0xC3, 0x42, 0xFF, /* Punch */
    0xC3, 0x45, 0xFF, /* Reader */
    0xC3, 0x48, 0xFF, /* Home */
    0xC3, 0x4B, 0xFF, /* Seldsk */
    0xC3, 0x4E, 0xFF, /* Settrk */
    0xC3, 0x51, 0xFF, /* Setsec */
    0xC3, 0x54, 0xFF, /* Setdma */
    0xC3, 0x57, 0xFF, /* Read */
    0xC3, 0x5A, 0xFF, /* Write */
    0xC3, 0x5D, 0xFF, /* Listst */
    0xC3, 0x60, 0xFF, /* Sectran */

    /* Fake BIOS for target system */
    0x76, 1, 0xC9,  /* Wboot */
    0x76, 2, 0xC9,  /* Const */
    0x76, 3, 0xC9,  /* Conin */
    0x76, 4, 0xC9,  /* Conout */
    0x76, 5, 0xC9,  /* List */
    0x76, 6, 0xC9,  /* Punch */
    0x76, 7, 0xC9,  /* Reader */
    0x76, 8, 0xC9,  /* Home @@ */
    0x76, 9, 0xC9,  /* Seldsk @@ */
    0x76, 10, 0xC9, /* Settrk @@ */
    0x76, 11, 0xC9, /* Setsec @@ */
    0x76, 12, 0xC9, /* Setdma @@ */
    0x76, 13, 0xC9, /* Read @@ */
    0x76, 14, 0xC9, /* Write @@ */
    0x76, 15, 0xC9, /* Listst */
    0x76, 16, 0xC9  /* Sectran @@ */
};

/*
** Build the FDOS (BIOS & BDOS) image in the 8080's space.
*/

void loadfdos(tpa, regp) unsigned char *tpa;
struct regs *regp;
{
  unsigned char *cp;
  int ii;

  memcpy(&tpa[TPASIZE - FDOSSIZE], fdos, sizeof(fdos));
  tpa[0] = tpa[5] = 0xC3; /* Build BIOS and BDOS jumps. */
  tpa[6] = (TPASIZE - FDOSSIZE) & 0xFF;
  tpa[7] = (TPASIZE - FDOSSIZE) >> 8;
  tpa[1] = tpa[6] + 3;
  tpa[2] = tpa[7];
  tpa[TPASIZE - FDOSSIZE - 1] = 0; /* Return address to exit simulation. */
  tpa[TPASIZE - FDOSSIZE - 2] = 0;
  for (ii = 0; ii < TPAPAD; ii += 2) { /* Overrun protection. */
    tpa[TPASIZE + ii] = 0x76;          /* HLT */
    tpa[TPASIZE + ii + 1] = 0x01;      /* Warm Start*/
  }
  regp->ppc = &tpa[LOADADDR];               /* Start executing here. */
  regp->psp = &tpa[TPASIZE - FDOSSIZE - 2]; /* Initial stack here. */
  regp->dma = &tpa[DEFDMA];                 /* Initial DMA buffer here. */
}

/*
** What to do when a CP/M FDOS service request (HLT #n) is detected.
**
** A static variable is used so that a second, paralleled simulation
** (if any) will get the same input values.
*/

void service(tpa) unsigned char *tpa;
{
  char *cp, linebuf[128];
  unsigned char byte;
  struct regs *regp;
  static unsigned char lastrega;

  regp = (struct regs *)(tpa - sizeof(struct regs));
  byte = *(regp->ppc + 1);
  switch (byte) {
  case 0: /* BDOS function. */
    bdos(tpa, regp, &lastrega);
    break;
  /* BIOS functions. */
  case 1: /* Warm Boot. */
    doexit(0);
    break;

  case 2: /* Console Status. */
    flushconout(regp, 1);
    if (regp->miscflags & DUMMYSVC)
      regp->rega = lastrega;
    else
      lastrega = regp->rega = myrdchk(infile) ? 0xFF : 0;
    break;

  case 3: /* Console Input. */
    if (regp->miscflags & DUMMYSVC)
      regp->rega = lastrega;
    else {
      flushconout(regp, 0);
      conin(regp, (char *)&regp->rega);
      lastrega = regp->rega;
    }
    break;

  case 4: /* Console Output. */
    if (!(regp->miscflags & DUMMYSVC))
      conout(regp, regp->regc);
    break;

  case 5: /* List Output. */
    if (!(regp->miscflags & DUMMYSVC))
      listout(regp, regp->regc);
    break;

  case 15: /* Return List Status. */
    if (!(regp->miscflags & DUMMYSVC))
      flushlistout(regp, 1);
    regp->rega = 0xFF;
    break;

  case 6: /* Auxiliary Output. */
  case 7: /* Auxiliary Input. */
  default:
    cp = linebuf;
    cp += mysprintf(cp, "\nInvalid Service instruction %02X.\n", byte);
    cp += mysprintf(cp, "Register contents:");
    write(2, linebuf, cp - linebuf);
    dumpregs(tpa);
    if (regp->trcoptions & TRCCORE)
      docore(tpa, regp);
    doexit(5);
    break;
  }
}

/*
** Perform CP/M BDOS function.
**
** Static variables are used so that a second, paralleled simulation
** (if any) will get the same input values.
*/

void bdos(tpa, regp, lastregap) unsigned char *tpa;
struct regs *regp;
unsigned char *lastregap;
{
  int len, fileio, seekval, ioval;
  char *cp, *cp2, *ufn, linebuf[512];
  static char lineinbuf[256];
  static int linelen;
  unsigned char fcode, *ucp1, *ucp2;
  struct fcb *fcbp;
  struct fcache *fp;
  struct stat st;
  static char lastdn[3];

  fileio = 0;
  fcode = regp->regc;
  if (fcode > 14) { /* File I/O operation? */
    fileio = 1;     /* Which ones use FCB's? */
    switch (fcode) {
    case 24: /* Return Log-in Vector. */
    case 25: /* Return Current Disk. */
    case 26: /* Set DMA Address. */
    case 32: /* Get/Set User Code. */
    case 37: /* Reset Drive. */
      fileio = 0;
    }
  }
  if (fileio) {
    fcbp = (struct fcb *)&tpa[(regp->regd << 8) | regp->rege];
    switch (fcode) {
    case 33: /* Read Random. */
    case 34: /* Write Random. */
    case 40: /* Write Random with Zero Fill. */
      if (fcbp->dn[0] >= 0) {
        fcbp->cr = fcbp->rr[0] & 0x7F;
        fcbp->extent = fcbp->rr[1] << 1 | fcbp->rr[0] >> 7;
      }
      break;
    }
  } else
    fcbp = 0;

  switch (fcode) {
  case 0: /* System Reset. */
    doexit(0);
    break;

  case 1: /* Console Input. */
    if (regp->miscflags & DUMMYSVC)
      regp->rega = *lastregap;
    else {
      flushconout(regp, 0);
      conin(regp, (char *)&regp->rega);
      *lastregap = regp->rega;
      if (regp->rega >= ' ' || regp->rega == '\n' || regp->rega == '\r' ||
          regp->rega == '\t' || regp->rega == 8)
        conout(regp, regp->rega);
    }
    break;

  case 2: /* Console Output. */
    if (!(regp->miscflags & DUMMYSVC))
      conout(regp, regp->rege);
    break;

  case 5: /* List (print) output. */
    if (!(regp->miscflags & DUMMYSVC))
      listout(regp, regp->rege);
    break;

  case 6: /* Direct Console I/O. */
    if (regp->rege == 0xFF) {
      if (!(regp->miscflags & DUMMYSVC)) {
        flushconout(regp, 1);
        if (myrdchk(infile)) {
          conin(regp, (char *)&regp->rega);
          *lastregap = regp->rega;
        } else
          *lastregap = regp->rega = 0;
      } else
        regp->rega = *lastregap;

    } else if (!(regp->miscflags & DUMMYSVC))
      conout(regp, regp->rege);
    break;

  case 9: /* Print String. */
    ucp1 = &tpa[(regp->regd << 8) | regp->rege];
    ucp2 = ucp1;
    while (*ucp2 != '$' && ucp2 < &tpa[TPASIZE])
      ucp2++;
    if (len = (ucp2 - ucp1))
      if (!(regp->miscflags & DUMMYSVC))
        conoutstr(regp, (char *)ucp1, len);
    break;

  case 10: /* Read Console Buffer. */
    ucp1 = &tpa[(regp->regd << 8) | regp->rege];
    if (!(regp->miscflags & DUMMYSVC)) {
      stdinlineon(regp);
      linelen = ucp1[0];
      do
        linelen = read(0, lineinbuf, linelen);
      while (linelen < 0 && errno == EINTR);
      if (linelen > 0) {
        ucp1[1] = linelen;
        memcpy(&ucp1[2], lineinbuf, linelen);
      } else
        ucp1[1] = 0;
      stdinlineoff(regp);
    } else {
      if (linelen > 0) {
        ucp1[1] = linelen;
        memcpy(&ucp1[2], lineinbuf, linelen);
      } else
        ucp1[1] = 0;
    }
    break;

  case 11: /* Get Console Status. */
    if (regp->miscflags & DUMMYSVC)
      regp->rega = *lastregap;
    else
      *lastregap = regp->rega = myrdchk(infile) ? 0xFF : 0;
    break;

  case 12:                          /* Return Version Number. */
    regp->regb = regp->regh = 0;    /* CP/M */
    regp->rega = regp->regl = 0x22; /* 2.2 */
    break;

  case 13:                    /* Reset Disk System. */
    regp->driveuser &= 0x0F;  /* Drive A, */
    regp->dma = &tpa[DEFDMA]; /* default DMA address. */
    break;

  case 14: /* Select Disk. */
    regp->driveuser = (regp->rege & 0x0F) << 4 | (regp->driveuser & 0x0F);
    break;

  case 15: /* Open File. */
    if (regp->miscflags & DUMMYSVC) {
      regp->rega = *lastregap;
      memset(&fcbp->s1, 0, 19);
      fcbp->dn[0] = lastdn[0];
      fcbp->dn[1] = lastdn[1];
      fcbp->dn[2] = lastdn[2];
    } else {
      *lastregap = regp->rega = 0xFF; /* Assume failure. */
      memset(&fcbp->s1, 0, 19);
      fcbp->dn[1] = lastdn[1] = O_RDONLY; /* Unix's file mode. */
      if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
        if (fp = cachedfile(cp2, &openfilehead)) {
          ioval = fp->fd;
          fcbp->dn[2] = lastdn[2] = fp->fakeid;
        } else {
          ioval = -1; /* Actually Unix fd. */
          if (ufn = cfntoufn(cp2)) {
            fileio = -1; /* Search fake file list first. */
            for (ioval = 0; ioval < nfakefiles; ioval++) {
              if (!strcmp(&cp2[2], fakefs[ioval].name)) {
                fileio = ioval;
                break;
              }
            }
            if (fileio < 0) { /* Fake not found, open for real. */
              ioval = open(ufn, OROF);
              fileio = 0; /* No fake ID, either. */
            } else        /* Fake found, flag it so. */
              ioval = 0;
          }
        }
        fcbp->dn[0] = lastdn[0] = ioval; /* Unix fd, if non-zero. */
        if (ioval >= 0) {
          *lastregap = regp->rega = 0; /* Open successful. */
          if (!fp) {
            fp = cachefile(cp2, &openfilehead);
            fp->fd = ioval;
            fcbp->dn[2] = lastdn[2] = fp->fakeid = fileio;
          }
        }
      }
    }
    break;

  case 16: /* Close File. */
    if (regp->miscflags & DUMMYSVC)
      regp->rega = *lastregap;
    else
      *lastregap = regp->rega = 0xFF; /* Assume Error. */
    if (fcbp->dn[0] >= 0) {
      cp2 = fncanon(fcbp, 0, regp->driveuser >> 4);
      if (!(regp->miscflags & DUMMYSVC)) {
        uncachefile(cp2, &openfilehead);
        if (fcbp->dn[0] > 2)
          *lastregap = regp->rega = close(fcbp->dn[0]) ? 0xFF : 0;
        else
          *lastregap = regp->rega = 0;
      }
    }
    fcbp->dn[0] = fcbp->dn[1] = -1;
    break;

  case 17: /* Search For First. */
    cp2 = ambigname(fncanon(fcbp, 0, regp->driveuser >> 4));
    if (!(regp->miscflags & DUMMYSVC))
      getflist(regp, cp2); /* then fall into... */
  case 18:                 /* Search For Next. */
    if (dirlisthead) {
      ufntocpmfn(dirlisthead->name, regp->dma);
      if (((regp->driveuser >> 4) + 1) == regp->dma[0])
        regp->dma[0] = 0; /* Default drive?  Make it so. */
      uncachefile(dirlisthead->name, &dirlisthead);
      *lastregap = regp->rega = 0;
    } else
      *lastregap = regp->rega = 0xFF;

    break;

  case 19: /* Delete File. */
    if (regp->miscflags & DUMMYSVC) {
      regp->rega = *lastregap;
      break;
    }
    *lastregap = regp->rega = 0xFF; /* Assume Error. */
    if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
      if (ufn = cfntoufn(cp2)) {
        fileio = -1; /* Search fake file list first. */
        for (ioval = 0; ioval < nfakefiles; ioval++) {
          if (!strcmp(&cp2[2], fakefs[ioval].name)) {
            fileio = ioval;
            break;
          }
        }
        if (fileio < 0) {
          if (!unlink(ufn))
            *lastregap = regp->rega = 0;
        } else {
          fakefs[fileio].name[0] = 0;
          *lastregap = regp->rega = 0;
        }
      }
    }
    break;

  case 33:                       /* Read Random. */
  case 20:                       /* Read Sequential. */
    *lastregap = regp->rega = 1; /* Assume Error. */
    if (fcbp->dn[0] >= 0) {
      len = 0x4000 * fcbp->extent + 128 * fcbp->cr;
      if (fcbp->dn[0] == 0) /* Fake file? */
        seekval = (len < *fakefs[fcbp->dn[2]].size) ? len : 0;
      else
        seekval = lseek(fcbp->dn[0], len, 0);
      if (seekval == len) {
        if (fcbp->dn[0] == 0) { /* Fake file? */
          memcpy(regp->dma, &fakefs[fcbp->dn[2]].data[seekval], 128);
          ioval = 128;
        } else
          ioval = read(fcbp->dn[0], regp->dma, 128);
        if (ioval > 0) {
          *lastregap = regp->rega = 0;
          if (ioval != 128) /* Padding (^Z's) required? */
            memset(&regp->dma[ioval], 26, 128 - ioval);
          *lastregap = regp->rega = 0;
        }
        if (fcode == 20) /* Only update record position on seq read. */
          if (++fcbp->cr >= 128) {
            fcbp->cr = 0;
            fcbp->extent++;
          }
      } else if (fcode == 33) /* Invalid Random Read? */
        *lastregap = regp->rega = 6;
    }
    break;

  case 34:                       /* Write Random. */
  case 40:                       /* Write Random with Zero Fill. */
  case 21:                       /* Write Sequential. */
    *lastregap = regp->rega = 1; /* Assume Error. */
    if (fcbp->dn[0] > 0) {       /* Fake files are RO. */
      if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
        if (!fcbp->dn[1]) {     /* Open for writing yet? */
          fcbp->dn[1] = O_RDWR; /* No, reopen the file RD/WR. */
          close(fcbp->dn[0]);
          fcbp->dn[0] = -1;
          if (ufn = cfntoufn(cp2)) {
            if ((fcbp->dn[0] = open(ufn, ORWF)) < 0) {
              fcbp->dn[0] = open(ufn, OROF);
              break;
            }
          }
        }
        len = 0x4000 * fcbp->extent + 128 * fcbp->cr;
        if ((seekval = lseek(fcbp->dn[0], len, 0)) == len) {
          if ((ioval = write(fcbp->dn[0], regp->dma, 128)) == 128)
            *lastregap = regp->rega = 0;
          if (fcode == 21) /* Only update record pos on seq write. */
            if (++fcbp->cr >= 128) {
              fcbp->cr = 0;
              fcbp->extent++;
            }
        } else if (fcode != 21) /* Invalid Random Write? */
          *lastregap = regp->rega = 6;
      }
    }
    break;

  case 22: /* Make File. */
    if (regp->miscflags & DUMMYSVC) {
      regp->rega = *lastregap;
      memset(&fcbp->s1, 0, 19);
      fcbp->dn[0] = lastdn[0];
      fcbp->dn[1] = lastdn[1];
    } else {
      *lastregap = regp->rega = 0xFF; /* Assume error. */
      if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
        if (ufn = cfntoufn(cp2)) {
          memset(&fcbp->s1, 0, 19);
          if ((fcbp->dn[0] = creat(ufn, 0666)) >= 0) {
            fcbp->dn[1] = O_RDWR;
            lastdn[0] = fcbp->dn[0];
            lastdn[1] = fcbp->dn[1];
            *lastregap = regp->rega = 0;
            cachefile(cp2, &openfilehead)->fd = fcbp->dn[0];
          }
        }
      }
    }
    break;

  case 23: /* Rename file. */
    if (regp->miscflags & DUMMYSVC) {
      regp->rega = *lastregap;
      break;
    }
    *lastregap = regp->rega = 0xFF; /* Assume error. */
    if (cp2 = fncanon(fcbp, 16, regp->driveuser >> 4)) {
      if (ufn = cfntoufn(cp2)) {
        strcpy(linebuf, ufn);
        if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
          if (ufn = cfntoufn(cp2)) {
            fileio = -1; /* Search fake file list first. */
            for (ioval = 0; ioval < nfakefiles; ioval++) {
              if (!strcmp(&cp2[2], fakefs[ioval].name)) {
                fileio = ioval;
                break;
              }
            }
            if (fileio < 0) {
              if (!link(ufn, linebuf))
                if (!unlink(ufn))
                  *lastregap = regp->rega = 0;
            } else {
              strncpy(fakefs[fileio].name, linebuf, sizeof fakefs[fileio].name);
              *lastregap = regp->rega = 0;
            }
          }
        }
      }
    }
    break;

  case 24: /* Return Log-in Vector. */
    seekval = 1;
    for (len = 0; len < (sizeof(drives) / sizeof(drives[0])); len++) {
      regp->regh = regp->regb |= (seekval >> 8);
      regp->regl = regp->rega |= seekval;
      seekval <<= 1;
    }
    break;

  case 25: /* Return Current Disk. */
    regp->rega = regp->driveuser >> 4;
    break;

  case 26: /* Set DMA address. */
    regp->dma = &tpa[(regp->regd << 8) | regp->rege];
    break;

  case 32: /* Get/Set User Code. */
    if (regp->rege == 0xFF)
      regp->rega = regp->driveuser & 0x0F;
    else
      regp->driveuser = (regp->rege & 0x0F) | (regp->driveuser & 0xF0);
    break;

  case 35: /* Compute File Size. */
    if (regp->miscflags & DUMMYSVC) {
      fcbp->rr[0] = lastdn[0];
      fcbp->rr[1] = lastdn[1];
      fcbp->rr[2] = lastdn[2];
    } else {
      if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
        if (ufn = cfntoufn(cp2)) {
          fileio = -1; /* Search fake file list first. */
          for (ioval = 0; ioval < nfakefiles; ioval++) {
            if (!strcmp(&cp2[2], fakefs[ioval].name)) { /*Ignr A:*/
              fileio = ioval;
              break;
            }
          }
          if (fileio < 0) {
            if (!stat(ufn, &st)) {
              len = st.st_size + 127;
              fcbp->rr[0] = lastdn[0] = len >> 7;
              fcbp->rr[1] = lastdn[1] = len >> 15;
              fcbp->rr[2] = lastdn[2] = len >> 23;
            }
          } else {
            len = *fakefs[fileio].size + 127;
            fcbp->rr[0] = lastdn[0] = len >> 7;
            fcbp->rr[1] = lastdn[1] = len >> 15;
            fcbp->rr[2] = lastdn[2] = len >> 23;
          }
        }
      }
    }
    break;

  case 36: /* Set Random Record. */
    if (fcbp->dn[0] >= 0) {
      fcbp->rr[0] = fcbp->cr;
      fcbp->rr[1] = fcbp->extent;
      fcbp->rr[2] = 0;
    }
    break;

  case 37: /* Reset Drive. */
    regp->rega = 0;
    break;

  default:
    cp = linebuf;
    cp += mysprintf(cp, "Invalid BDOS call $%02X.\n", fcode);
    cp += mysprintf(cp, "Register contents:");
    write(2, linebuf, cp - linebuf);
    dumpregs(tpa);
    if (regp->trcoptions & TRCCORE)
      docore(tpa, regp);
    doexit(4);
    break;
  }
}

/*
** Dump out the previous instruction.  For supplying context for
** bad opcode or simulation mismatch reports.
*/

void dumplast(tpa, regp, linebuf) unsigned char *tpa;
struct regs *regp;
char *linebuf;
{
  char *cp;
  unsigned char *lastpc;

  lastpc = regp->pppc;
  cp = linebuf;
  cp += mysprintf(cp, "\nPrevious instruction:"
                      "                               ");
  if (*lastpc == 0xCB || *lastpc == 0xDD || *lastpc == 0xED || *lastpc == 0xFD)
    cp += mysprintf(cp, "%02X%02X %02X%02X ", (lastpc - tpa) >> 8,
                    (lastpc - tpa) & 0xFF, lastpc[0], lastpc[1]);
  else
    cp += mysprintf(cp, "%02X%02X  %02X  ", (lastpc - tpa) >> 8,
                    (lastpc - tpa) & 0xFF, lastpc[0]);
  cp += mysprintf(cp, "     %s", decode(lastpc, regp->trcoptions & ZILOG, 1));
  write(2, linebuf, cp - linebuf);
}

/*
** What to do when an illegal 8080 instruction is detected.
*/

void illegal(tpa) unsigned char *tpa;
{
  char *cp, linebuf[128];
  int pc;
  unsigned char byte;
  struct regs *regp;

  flusher(0);
  regp = (struct regs *)(tpa - sizeof(struct regs));
  regp->running = 0;
  pc = (regp->pchi << 8) | regp->pclo;
  byte = tpa[pc];
  cp = linebuf;
  cp += mysprintf(cp, "\nIllegal instruction %02X at %04X.\n", byte, pc);
  cp += mysprintf(cp, "Register contents:");
  write(2, linebuf, cp - linebuf);
  dumpregs(tpa);
  write(2, "\n", 1);
  if (regp->trcoptions & TRCCORE)
    docore(tpa, regp);
}

/*
** Dump out the 8080/Z80 registers.
*/

void dumpregs(tpa) unsigned char *tpa;
{
  char *cp, linebuf[128];
  int ii, sp, pc, bc, de, hl, bc2, de2, hl2;
  unsigned char byte, byte2;
  struct regs *regp;

  debugoutput = 1;
  regp = (struct regs *)(tpa - sizeof(struct regs));
  flushconout(regp, 0);
  sp = (regp->sphi << 8) | regp->splo;
  pc = (regp->pchi << 8) | regp->pclo;
  bc = (regp->regb << 8) | regp->regc;
  de = (regp->regd << 8) | regp->rege;
  hl = (regp->regh << 8) | regp->regl;
  bc2 = (regp->regb2 << 8) | regp->regc2;
  de2 = (regp->regd2 << 8) | regp->rege2;
  hl2 = (regp->regh2 << 8) | regp->regl2;
  byte = tpa[pc];
  if (regp->trcoptions & SHOWALT) { /* Only show 'em if they've been used. */
  showalts:
    if (!regp->trcreccnt) {
      cp = linebuf;
      cp += mysprintf(cp, "\n%s-AF' -BC'    -DE'    -HL'    -IX- -IY- -IR-",
                      regp->marker);
      write(2, linebuf, cp - linebuf);
    }
    cp = linebuf;
    cp += mysprintf(
        cp, "\n%s%02X%02X %02X%02X>%02X %02X%02X>%02X %02X%02X",
        (!regp->trcreccnt && regp->marker && (regp->trcoptions & SHOWALT))
            ? &"   "[3 - strlen(regp->marker)]
            : regp->marker,
        regp->rega2, regp->regf2, regp->regb2, regp->regc2, tpa[bc2],
        regp->regd2, regp->rege2, tpa[de2], regp->regh2, regp->regl2);
    cp += mysprintf(cp, ">%02X %02X%02X %02X%02X %02X%02X", tpa[hl2],
                    regp->regxh, regp->regxl, regp->regyh, regp->regyl,
                    regp->regi, regp->seed & 0xFF);
    write(2, linebuf, cp - linebuf);
  } else if (regp->rega2 || regp->regf2 || regp->regb2 || regp->regc2 ||
             regp->regd2 || regp->rege2 || regp->regh2 || regp->regl2 ||
             regp->regxh || regp->regxl || regp->regyh || regp->regyl) {
    regp->trcoptions |= SHOWALT;
    regp->trcreccnt = 0;
    goto showalts;
  }
  if (!regp->trcreccnt) {
    cp = linebuf;
    cp += mysprintf(cp,
                    "\n%s-AF- -BC-    -DE-    -HL-    "
                    "-SP- -S0- -S1- -S2- -PC- -op-",
                    (regp->marker && (regp->trcoptions & SHOWALT))
                        ? &"   "[3 - strlen(regp->marker)]
                        : regp->marker);
    write(2, linebuf, cp - linebuf);
  }
  cp = linebuf;
  cp += mysprintf(
      cp,
      "\n%s%02X%02X %02X%02X>%02X %02X%02X>%02X %02X%02X>%02X "
      "%02X%02X>",
      (regp->marker && (!regp->trcreccnt || (regp->trcoptions & SHOWALT)))
          ? &"   "[3 - strlen(regp->marker)]
          : regp->marker,
      regp->rega, regp->regf, regp->regb, regp->regc, tpa[bc], regp->regd,
      regp->rege, tpa[de], regp->regh, regp->regl, tpa[hl], regp->sphi,
      regp->splo);
  cp += mysprintf(cp, "%02X%02X,%02X%02X,%02X%02X ", tpa[sp + 1], tpa[sp + 0],
                  tpa[sp + 3], tpa[sp + 2], tpa[sp + 5], tpa[sp + 4]);
  if (byte == 0xCB || byte == 0xDD || byte == 0xED || byte == 0xFD)
    cp += mysprintf(cp, "%02X%02X %02X%02X ", regp->pchi, regp->pclo, byte,
                    tpa[pc + 1]);
  else
    cp += mysprintf(cp, "%02X%02X  %02X  ", regp->pchi, regp->pclo, byte);
  write(2, linebuf, cp - linebuf);
  regp->trcreccnt =
      ((regp->trcreccnt + 1) & (regp->trcoptions & SHOWALT ? 0x07 : 0x0F));
}

/*
** Single-step the target.  Actually do some command processing here?
*/

void stepper(tpa) unsigned char *tpa;
{
  int i;
  char linebuf[128];
  struct regs *regp;

  dumpregs(tpa);
  regp = (struct regs *)(tpa - sizeof(struct regs));
  if (regp->trcoptions & TRCSTEP) { /* Single-stepping? */
    stdinlineon(regp);
    do
      i = read(0, linebuf, sizeof linebuf);
    while (i < 0 && errno == EINTR);
    stdinlineoff(regp);
  }
}
