/*
**  Portable MINCE-80 by Jeffrey H. Johnson <trnsz@pobox.com>
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

int debugoutput;                /* For making sure debugout has NL at end. */

int main(argc, argv)
int argc;
char *argv[];
{
    char ch, *filename, *argp, linebuf[256];
    unsigned char *tpa;
#ifdef CONTEST
    unsigned char *tpa2;
    struct regs *regp2;
#endif
    struct regs *regp;

    if (!(tpa = malloc(TPASIZE + TPAPAD + sizeof(struct regs))))
        exit(2);
    memset(tpa, 0x00, TPASIZE + TPAPAD + sizeof(struct regs));
    gregp = regp = (struct regs *) tpa;
    tpa += sizeof(struct regs); /* Allocate Pseudo-CPU storage. */
    regp->vtstate = idle;       /* Fill in what we must right now. */
    regp->listfd = -1;
    holdingchar = -1;
    if (!(regp->seed = time(0)))
        regp->seed = 1;
    infilen = 0;
    infile = 0;                 /* STDIN by default. */

    filename = 0;
    progname = *argv;
    while (--argc > 0) {
        argp = *++argv;
        if (*argp == '-') {
            argp++;
            switch (ch = *argp++) {
#ifndef EMBED
              case 'A': case 'B': case 'C': case 'D': /* For Drive/User */
              case 'E': case 'F': case 'G': case 'H': /* assignment. */
              case 'I': case 'J': case 'K': case 'L':
              case 'M': case 'N': case 'O': case 'P':
                if (!*argp)
                    --argc, argp = *++argv;
                drives[ch - 'A'] = argp;
                break;
#endif
#ifdef TRACE
              case 'e':         /* End trace address. */
                if (!*argp)
                    --argc, argp = *++argv;
                regp->trcend = tpa + myatoi(argp);
                break;

              case 'f':         /* Trace FCB's too. */
                regp->trcoptions |= TRCFCB;
                break;

              case 'n':         /* Count opcodes simulated. */
                regp->trcoptions |= TRCCOUNT;
                break;

              case 'p':         /* Simulate N MHz real timing. */
                if (!*argp)
                    --argc, argp = *++argv;
                if (*argp == '.') /* Accept forms like .1 (Mhz) */
                    regp->mhz = myatoi(argp+1) * 100000;
                else
                    regp->mhz = myatoi(argp) * 1000000;
                regp->trcoptions |= MHZ;
                regp->basetime = time(0);
                break;

              case 's':         /* Start trace address. */
                if (!*argp)
                    --argc, argp = *++argv;
                regp->trcbegin = tpa + myatoi(argp);
                break;

              case 't':         /* Start tracing immediately. */
                regp->trcflag = 1;
                break;

              case 'T':         /* Trace BIOS/BDOS too. */
                regp->trcoptions |= TRCSVC;
                break;

              case 'S':
                regp->trcoptions |= TRCSTEP;
                break;

              case 'z':
                regp->trcoptions |= ZILOG;
                break;
#endif
              case 'b':         /* No buffering. */
                regp->miscflags |= NOBUFFER;
                break;

              case 'c':         /* Make 8080 core file on error. */
                regp->trcoptions |= TRCCORE;
                break;

              case 'd':         /* No DEL->BS conversion. */
                regp->miscflags |= NODELTOBS;
                break;

              case 'i':         /* Use file instead of standard input. */
                if (!*argp)
                    --argc, argp = *++argv;
                infilen = argp;
                break;

              case 'l':         /* List (print) output to file. */
                if (!*argp)
                    --argc, argp = *++argv;
                regp->listname = argp;
                break;

              case 'v':         /* No H19->VT-100 conversion. */
                regp->vtstate = off;
                break;

              case 'V':         /* Version printout. */
                strcpy(linebuf, progname);
                strcat(linebuf, " Version 3.0\n");
                write(1, linebuf, strlen(linebuf));
                exit(0);
                break;

              case 'x':         /* For debugging. */
                regp->miscflags |= NOIOCTL;
                break;

              default:
                goto usage;
            }
        } else {
#ifdef EMBED
            if (filename = strrchr(argp, '/')) {
                *filename++ = 0;
                drives[0] = argp;
            } else {
                filename = argp;
            }
            *argv = filename;   /* Leave basename in argv. */
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
#ifdef TRACE
              mysprintf(linebuf, "Usage: %s %s[-b] [-c] [-d] "
                  "[-i infile] [-l listfile] [-v] [-V] [-p MHz] [-s startaddr]"
                        " [-t] [-T] [-e endaddr]%s\n",
                        progname, cp, cp2));
#else
              mysprintf(linebuf, "Usage: %s %s[-b] [-c] [-d] "
                        "[-i infile] [-l listfile] [-v] [-V]%s\n",
                        progname, cp, cp2));
#endif
        exit(1);
    }
    if (infilen)
        redirectin(infilen);
    if (tpa) {
        if (loadcom(filename, tpa + LOADADDR)) { /* Load program. */
            loadfdos(tpa, regp); /* Set up Pseudo-CPU registers & memory. */
            parseargs(tpa, argc, argv);
            setupstdin(regp);
#ifdef TRACE
#  ifdef CONTEST
            if (!(tpa2 = malloc(TPASIZE + TPAPAD + sizeof(struct regs))))
                exit(2);
            gregp2 = regp2 = (struct regs *) tpa2;
            memcpy(regp2, regp, TPASIZE + TPAPAD + sizeof(struct regs));
            tpa2 += sizeof(struct regs);
            loadfdos(tpa2, regp2); /* Set up Pseudo-CPU2 regs & memory. */
            if (regp2->trcbegin) /* Repair pointers. */
                regp2->trcbegin += (tpa2 - tpa);
            if (regp2->trcend)
                regp2->trcend += (tpa2 - tpa);
            strcpy(regp->marker, "A: ");
            strcpy(regp2->marker, "C: ");
            regp2->miscflags |= DUMMYSVC; /* No I/O for the second run. */
            regp->running = 1;  /* Only one step each before returning. */
            regp2->running = 1;
            do {
                oopcodesimulator(tpa); /* Step the old (Asm) Pseudo-CPU. */
                opcodesimulator(tpa2); /* Step the new (C) Pseudo-CPU. */
                if (regp->pchi != regp2->pchi ||
                    regp->pclo != regp2->pclo ||
                    regp->sphi != regp2->sphi ||
                    regp->splo != regp2->splo ||
                    regp->rega != regp2->rega ||
#    ifdef Z80
                    (regp->regf & 0xC5) != (regp2->regf & 0xC5) || /* SZPC! */
#    else
                    (regp->regf & 0xC1) != (regp2->regf & 0xC1) || /* SZC! */
#    endif
                    regp->regb != regp2->regb ||
                    regp->regc != regp2->regc ||
                    regp->regd != regp2->regd ||
                    regp->rege != regp2->rege ||
                    regp->regh != regp2->regh ||
                    regp->regl != regp2->regl ||
#    ifdef Z80
                    regp->regxh != regp2->regxh ||
                    regp->regxl != regp2->regxl ||
                    regp->regyh != regp2->regyh ||
                    regp->regyl != regp2->regyl ||
                    regp->rega2 != regp2->rega2 ||
                    (regp->regf2 & 0xC5) != (regp2->regf2 & 0xC5) || /* SZPC */
                    regp->regb2 != regp2->regb2 ||
                    regp->regc2 != regp2->regc2 ||
                    regp->regd2 != regp2->regd2 ||
                    regp->rege2 != regp2->rege2 ||
                    regp->regh2 != regp2->regh2 ||
                    regp->regl2 != regp2->regl2 ||
#    endif
                    tpa[regp->regb << 8 | regp->regc]
                        != tpa2[regp2->regb << 8 | regp2->regc] ||
                    tpa[regp->regd << 8 | regp->rege]
                        != tpa2[regp2->regd << 8 | regp2->rege] ||
                    tpa[regp->regh << 8 | regp->regl]
                        != tpa2[regp2->regh << 8 | regp2->regl]) {
                    flusher(0);
                    if (regp2->pppc)
                        dumplast(tpa2, regp2, linebuf);
                    write(2, linebuf,
                          mysprintf(linebuf, "\nSimulation mismatch, ASM:"));
                    dumpregs(tpa);
                    write(2, linebuf,
                          mysprintf(linebuf, "\nSimulation mismatch, C:"));
                    dumpregs(tpa2);
                    doexit(0);
                }
                /* The Half-carry bit can be different between the C and Asm */
                /* implementations, so propagate Asm's to C on PUSH P, since */
                /* the C code doesn't care about it and otherwise it might */
                /* cause a simulation mismatch later. */
                if (*regp->ppc == 0xF5) {
                    regp2->regf = (regp2->regf & 0xEF) | (regp->regf & 0x10);
                }
            } while (regp->running && regp2->running);
            if (regp2->pppc && (regp->badflag || regp2->badflag))
                dumplast(tpa2, regp2, linebuf);
#  else
            regp->running = 2;    /* Run continuously. */
            opcodesimulator(tpa); /* Run the Pseudo-CPU. */
            if (regp->pppc && regp->badflag)
                dumplast(tpa, regp, linebuf);
#  endif
#else
            regp->running = 2;    /* Run continuously. */
            opcodesimulator(tpa); /* Run the Pseudo-CPU. */
            if (regp->pppc && regp->badflag)
                dumplast(tpa, regp, linebuf);
#endif
        } else {
            write(2, linebuf, mysprintf(linebuf, "%s: Can't open %s\n",
                                        progname, filename));
            exit(3);
        }
    }
    doexit(0);
}

/*
** Convert remaining Unix command-line arguments to CP/M's strange
** pre-filled FCB's and command tail.
*/

void
parseargs(tpa, argc, argv)
unsigned char *tpa;
int argc;
char *argv[];
{
    int i, len, arg;
    char *cp;
    struct regs *regp;

    regp = (struct regs *) (tpa - sizeof(struct regs));
    if (regp->trcoptions & TRCCOUNT) {
        write(2, argv[-1], strlen(argv[-1]));
        for (i=0; i<argc; i++) {
            write(2, " ", 1);
            write(2, argv[i], strlen(argv[i]));
        }
        write(2, "\n", 1);
    }
    ufntocpmfn(argc ? argv[0] : 0, &tpa[FCB1]);
    ufntocpmfn((argc > 1) ? argv[1] : 0, &tpa[FCB2]);

    i = 0;                      /* Make up command tail. */
    cp = (char *) &tpa[DEFDMA+1];
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
    if (i) {
        tpa[DEFDMA] = i+1;
    } else {
        tpa[DEFDMA] = 0;
    }
}

/*
** Convert a Unix filename to a CP/M FCB name.
*/

void
ufntocpmfn(name, fcb)
char *name;
unsigned char *fcb;
{
    char *cp, chr;
    int i;

    fcb[0] = 0;                 /* Default drive. */
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
        strncpy((char *) &fcb[1], name, i);
        if (cp = strrchr(name, '.'))
            strncpy((char *) &fcb[9], cp+1, 3);
        for (i=1; i<12; i++) {
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

void
catcher(signum)
int signum;
{
    doexit(0);
}

/*
** Signal catchers, for trace flag manipulation.
*/

void
u1catcher(signum)
int signum;
{
    if (gregp)
        gregp->trcflag = 1;
    if (gregp2)
        gregp2->trcflag = 1;
    signal(SIGUSR1, u1catcher);
}

void
u2catcher(signum)
int signum;
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

void
doexit(code)
int code;
{
    flusher(0);                 /* Make sure all output is done. */
    restorestdin();             /* Fix the terminal. */
    if (debugoutput)
        write(2, "\n", 1);
#ifdef TRACE
    if (gregp->trcoptions & TRCCOUNT) {
        dumpcounts(gregp);
    }
#endif
    exit(code);                 /* We be gone! */
}

#ifdef TRACE
/*
** Print out the count of each opcode simulated.
*/

struct record {         /* For sorting the printout. */
    unsigned long count;
    unsigned char op0;
    unsigned char op1;
    unsigned char op2;
    unsigned char cycs;
};

int
comparison(a, b)
struct record *a, *b;
{
    if (a->count > b->count)    /* Primary key. */
        return -1;              /* Descending order. */
    else if (a->count < b->count)
        return 1;
    else if (a->op0 > b->op0)   /* Secondary key. */
        return 1;               /* Ascending order. */
    else if (a->op0 < b->op0)
        return -1;
    else if (a->op1 > b->op1)   /* Tertiary key.*/
        return 1;               /* Ascending order. */
    else if (a->op1 < b->op1)
        return -1;
    else if (a->op2 > b->op2)   /* Quaternary key.*/
        return 1;               /* Ascending order. */
    else if (a->op2 < b->op2)
        return -1;
    else
        return 0;
}

void
printrec(regp, ops, mnep, count, cycs)
struct regs *regp;
unsigned char *ops;
char *mnep;
unsigned long count;
int cycs;
{
    char linebuf[90], *cp;

    cp = linebuf;
    cp += mysprintf(cp, mnep, count, cycs, ops[0], ops[1], ops[2]);
    cp += mysprintf(cp, "%s\n", decode(ops, regp->trcoptions & ZILOG, 0));
    write(2, linebuf, cp - linebuf);
}

void
dumpcounts(regp)
struct regs *regp;
{
    struct record records[256*7];
    extern char *svcmnemonics[64];
    extern char cycletabl[256];
#  ifdef Z80
    extern char cycletabl1[256], cycletabl2[256], cycletabl3[256],
                cycletabl4[256], cycletabl5[256], cycletabl6[256];
#  endif
    char linebuf[90], *cp, *mnep, *cycp;
    unsigned char ops[4];
    int ii, jj, kk, ll, mm, nn;
    unsigned long count, total, check;

    if (regp->opcount[0]) {     /* Do opcode counts. */
        cp = linebuf;
        cp += mysprintf(cp, "Opcodes (by Op):\n"
                        "    Count  cycs   OP  Mnemonic\n"
                        "    ----- ------ ---- ------------\n");
        write(2, linebuf, cp - linebuf);
        for (ii=0, kk=0; ii<256; ii++) {
            ops[0] = records[kk].op0 = ii;
            ops[1] = records[kk].op1 = 0;
            ops[2] = records[kk].op2 = 0;
            ops[3] = records[kk].cycs = 0;
            mnep = "%9d (@ %2d)  %02X  ";
            if (count = records[kk].count = regp->opcount[0][ii]) {
#  ifdef Z80
                if (ii == 0xCB || ii == 0xDD || ii == 0xED || ii == 0xFD) {
                         if (ii == 0xCB) {ll = 1; cycp = cycletabl1;}
                    else if (ii == 0xDD) {ll = 2; cycp = cycletabl2;}
                    else if (ii == 0xED) {ll = 3; cycp = cycletabl3;}
                    else if (ii == 0xFD) {ll = 4; cycp = cycletabl4;}
                    for (jj=0; jj<256; jj++) {
                        ops[0] = records[kk].op0 = ii;
                        ops[1] = records[kk].op1 = jj;
                        mnep = "%9d (@ %2d) %02X%02X ";
                        if ((ii == 0xDD || ii == 0xFD) && jj == 0xCB) {
                                 if (ii == 0xDD) {nn = 5; cycp = cycletabl5;}
                            else if (ii == 0xFD) {nn = 6; cycp = cycletabl6;}
                            ops[2] = 0;
                            mnep = "%9d (@ %2d) %02X%02Xdd%02X ";
                            for (mm=0; mm<256; mm++) {
                                if (count = records[kk].count = regp->opcount[nn][mm]) {
                                    ops[0] = records[kk].op0 = ii;
                                    ops[1] = records[kk].op1 = jj;
                                    ops[3] = records[kk].op2 = mm;
                                    records[kk].cycs = cycp[mm];
                                    printrec(regp, ops, mnep, count, cycp[mm]);
                                    kk++;
                                }
                            }
                                 if (ii == 0xDD) {cycp = cycletabl2;}
                            else if (ii == 0xFD) {cycp = cycletabl4;}
                        } else {
                            ops[2] = ops[3] = records[kk].op2 = 0;
                            if (count = records[kk].count = regp->opcount[ll][jj]) {
                                records[kk].cycs = cycp[jj];
                                printrec(regp, ops, mnep, count, cycp[jj]);
                                kk++;
                            }
                        }
                    }
                } else
#  endif
                {
                    cycp = cycletabl;
                    records[kk].cycs = cycp[ii];
                    printrec(regp, ops, mnep, count, cycp[ii]);
                    kk++;
                }
            }
        }
        cp = linebuf;
        cp += mysprintf(cp, "\nOpcodes (by Cnt):\n"
                        "    Count  cycs   OP  Mnemonic\n"
                        "    ----- ------ ---- ------------\n");
        write(2, linebuf, cp - linebuf);
        qsort(records, kk, sizeof(struct record), comparison);
        total = 0;
        check = 0;
        for (ii=0; ii<kk; ii++) {
            if (count = records[ii].count) {
                total += count;
                check += count * records[ii].cycs;
                ops[0] = records[ii].op0;
                ops[1] = records[ii].op1;
                ops[2] = ops[3] = records[ii].op2;
                mnep = "%9d (@ %2d)  %02X  ";
                if (ops[0] == 0xCB || ops[0] == 0xDD
                    || ops[0] == 0xED || ops[0] == 0xFD)
                    mnep = "%9d (@ %2d) %02X%02X ";
                if ((ops[0] == 0xDD || ops[0] == 0xFD) && ops[1] == 0xCB) {
                    mnep = "%9d (@ %2d) %02X%02Xdd%02X ";
                    ops[2] = 0;
                } else {
                    ops[3] = 0;
                }
                printrec(regp, ops, mnep, count, records[ii].cycs);
            }
        }
        cp = linebuf;
        cp += mysprintf(cp, "    ------------------------------\n%9ld "
                        "Total Instructions\n", total);
        write(2, linebuf, cp - linebuf);
        cp = linebuf;
        cp += mysprintf(cp, "%9ld Total Cycles (%ld average/inst.)\n",
                        regp->cycles, total ? regp->cycles/total : 0);
        write(2, linebuf, cp - linebuf);
        cp = linebuf;
        if (check != regp->cycles) {
            cp += mysprintf(cp, "%9ld Total by calculation: discrepancy %ld"
                            " (CC/RC/LDIR, etc.)", check, regp->cycles-check);
        }
        write(2, linebuf, cp - linebuf);
        debugoutput = 1;
    }
    if (regp->svcount) {        /* Now do service call counts. */
        total = 0;
        cp = linebuf;
        cp += mysprintf(cp, "\nService Calls:\n    Count  Mnemonic\n"
                        "    -----  ---------------------\n");
        write(2, linebuf, cp - linebuf);
        for (ii=0; ii<64; ii++) {
            if (count = regp->svcount[ii]) {
                total += count;
                mnep = svcmnemonics[ii];
                cp = linebuf;
                cp += mysprintf(cp, "%9d  %s $%02X %s\n", count,
                                ii >= 16 ? "BDOS" : "BIOS",
                                ii >= 16 ? ii - 16 : ii + 1, mnep);
                write(2, linebuf, cp - linebuf);
            }
        }
        cp = linebuf;
        cp += mysprintf(cp,"    ----------------------------\n%9d Total\n",
                        total);
        write(2, linebuf, cp - linebuf);
        debugoutput = 1;
    }
}
#endif

/*
** Make an 8080 core file, so to speak.  For debugging.
*/

void
docore(tpa, regp)
unsigned char *tpa;
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

int loadcom(name, where)
char *name;
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
    } else {
        strncat(fnambuf, tail, sizeof(fnambuf)-1);
    }
    if ((fd = open(fnambuf, 0)) >= 0) {
        fsize = lseek(fd, 0L, 2);
        lseek(fd, 0L, 0);
        if (fsize <= (TPASIZE - LOADADDR - FDOSSIZE - 2)) { /* Too big? */
            while (fsize > 0) { /* No, load it. */
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
    0x76,0x00,0xC9,     /* Fake BDOS for target system. */
                        /* BIOS Jump Table */
    0xC3,0x33,0xFF,     /* Wboot */
    0xC3,0x36,0xFF,     /* Const */
    0xC3,0x39,0xFF,     /* Conin */
    0xC3,0x3C,0xFF,     /* Conout */
    0xC3,0x3F,0xFF,     /* List */
    0xC3,0x42,0xFF,     /* Punch */
    0xC3,0x45,0xFF,     /* Reader */
    0xC3,0x48,0xFF,     /* Home */
    0xC3,0x4B,0xFF,     /* Seldsk */
    0xC3,0x4E,0xFF,     /* Settrk */
    0xC3,0x51,0xFF,     /* Setsec */
    0xC3,0x54,0xFF,     /* Setdma */
    0xC3,0x57,0xFF,     /* Read */
    0xC3,0x5A,0xFF,     /* Write */
    0xC3,0x5D,0xFF,     /* Listst */
    0xC3,0x60,0xFF,     /* Sectran */

                        /* Fake BIOS for target system */
    0x76,1,0xC9,        /* Wboot */
    0x76,2,0xC9,        /* Const */
    0x76,3,0xC9,        /* Conin */
    0x76,4,0xC9,        /* Conout */
    0x76,5,0xC9,        /* List */
    0x76,6,0xC9,        /* Punch */
    0x76,7,0xC9,        /* Reader */
    0x76,8,0xC9,        /* Home @@ */
    0x76,9,0xC9,        /* Seldsk @@ */
    0x76,10,0xC9,       /* Settrk @@ */
    0x76,11,0xC9,       /* Setsec @@ */
    0x76,12,0xC9,       /* Setdma @@ */
    0x76,13,0xC9,       /* Read @@ */
    0x76,14,0xC9,       /* Write @@ */
    0x76,15,0xC9,       /* Listst */
    0x76,16,0xC9        /* Sectran @@ */
};

/*
** Build the FDOS (BIOS & BDOS) image in the 8080's space.
*/

void
loadfdos(tpa, regp)
unsigned char *tpa;
struct regs *regp;
{
    unsigned char *cp;
    int ii;

    memcpy(&tpa[TPASIZE - FDOSSIZE], fdos, sizeof(fdos));
    tpa[0] = tpa[5] = 0xC3;     /* Build BIOS and BDOS jumps. */
    tpa[6] = (TPASIZE - FDOSSIZE) & 0xFF;
    tpa[7] = (TPASIZE - FDOSSIZE) >> 8;
    tpa[1] = tpa[6] + 3;
    tpa[2] = tpa[7];
    tpa[TPASIZE - FDOSSIZE - 1] = 0; /* Return address to exit simulation. */
    tpa[TPASIZE - FDOSSIZE - 2] = 0;
    for (ii=0; ii<TPAPAD; ii+= 2) { /* Overrun protection. */
        tpa[TPASIZE+ii] = 0x76; /* HLT */
        tpa[TPASIZE+ii+1] = 0x01; /* Warm Start*/
    }
    regp->ppc = &tpa[LOADADDR]; /* Start executing here. */
    regp->psp = &tpa[TPASIZE - FDOSSIZE - 2]; /* Initial stack here. */
    regp->dma = &tpa[DEFDMA];   /* Initial DMA buffer here. */
#ifdef TRACE
    if (regp->opcount[0] = malloc(sizeof(long) * 256)) /* 8080 Ops*/
        memset(regp->opcount[0], 0, sizeof(long) * 256);
#  ifdef Z80
    if (regp->opcount[1] = malloc(sizeof(long) * 256)) /* Z80 CBXX Ops */
        memset(regp->opcount[1], 0, sizeof(long) * 256);
    if (regp->opcount[2] = malloc(sizeof(long) * 256)) /* Z80 DDXX Ops */
        memset(regp->opcount[2], 0, sizeof(long) * 256);
    if (regp->opcount[3] = malloc(sizeof(long) * 256)) /* Z80 EDXX Ops */
        memset(regp->opcount[3], 0, sizeof(long) * 256);
    if (regp->opcount[4] = malloc(sizeof(long) * 256)) /* Z80 FDXX Ops */
        memset(regp->opcount[4], 0, sizeof(long) * 256);
    if (regp->opcount[5] = malloc(sizeof(long) * 256)) /* Z80 DDCBXX Ops */
        memset(regp->opcount[5], 0, sizeof(long) * 256);
    if (regp->opcount[6] = malloc(sizeof(long) * 256)) /* Z80 FDCBXX Ops */
        memset(regp->opcount[6], 0, sizeof(long) * 256);
#  endif
    if (regp->svcount = malloc(sizeof(long) * 64))
        memset(regp->svcount, 0, sizeof(long) * 64);
#endif
}

/*
** What to do when a CP/M FDOS service request (HLT #n) is detected.
**
** A static variable is used so that a second, paralleled simulation
** (if any) will get the same input values.
*/

void
service(tpa)
unsigned char *tpa;
{
    char *cp, linebuf[128];
    unsigned char byte;
    struct regs *regp;
    static unsigned char lastrega;

    regp = (struct regs *) (tpa - sizeof(struct regs));
    byte = *(regp->ppc + 1);
#ifdef TRACE
    if (regp->svcount && byte && byte < 16)
        regp->svcount[byte - 1]++;
    if (regp->trcoptions & MHZ) {
        unsigned long realtime, simtime;
        realtime = time(0) - regp->basetime;
        simtime = regp->cycles / regp->mhz;
        if (realtime < simtime && !(regp->miscflags & DUMMYSVC))
            sleep(simtime - realtime);
    }
    if (regp->trcoptions & TRCSVC) {
        cp = linebuf;
        if (byte)
            cp += mysprintf(cp, "BIOS %d ", byte);
    }
#endif
    switch (byte) {
      case 0:                   /* BDOS function. */
        bdos(tpa, regp, &lastrega);
        break;
                                /* BIOS functions. */
      case 1:                   /* Warm Boot. */
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Warm Boot)\n");
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        doexit(0);
        break;

      case 2:                   /* Console Status. */
        flushconout(regp, 1);
        if (regp->miscflags & DUMMYSVC)
            regp->rega = lastrega;
        else
            lastrega = regp->rega = myrdchk(infile) ? 0xFF : 0;
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Status): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 3:                   /* Console Input. */
        if (regp->miscflags & DUMMYSVC) {
            regp->rega = lastrega;
        } else {
            flushconout(regp, 0);
            conin(regp, (char *) &regp->rega);
            lastrega = regp->rega;
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Input): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 4:                   /* Console Output. */
        if (!(regp->miscflags & DUMMYSVC))
            conout(regp, regp->regc);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Output): %02X\n", regp->regc);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 5:                   /* List Output. */
        if (!(regp->miscflags & DUMMYSVC))
            listout(regp, regp->regc);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(List Output): %02X\n", regp->regc);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 15:                  /* Return List Status. */
        if (!(regp->miscflags & DUMMYSVC))
            flushlistout(regp, 1);
        regp->rega =  0xFF;
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(List Status): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 6:                   /* Auxiliary Output. */
      case 7:                   /* Auxiliary Input. */
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

void
bdos(tpa, regp, lastregap)
unsigned char *tpa;
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
#ifdef TRACE
    if (regp->svcount && fcode < 48)
        regp->svcount[fcode + 16]++;
#endif
    if (fcode > 14) {           /* File I/O operation? */
        fileio = 1;             /* Which ones use FCB's? */
        switch (fcode) {
          case 24:              /* Return Log-in Vector. */
          case 25:              /* Return Current Disk. */
          case 26:              /* Set DMA Address. */
          case 32:              /* Get/Set User Code. */
          case 37:              /* Reset Drive. */
            fileio = 0;
        }
    }
    if (fileio) {
        fcbp = (struct fcb *) &tpa[(regp->regd << 8) | regp->rege];
#ifdef TRACE
        if (regp->trcoptions & TRCFCB) {
            cp = linebuf;
            cp += mysprintf(cp, "---- %d ($%02X): FCB %04X  %c: %.8s.%.3s "
                            "EX:%d CR:%3d DMA: %04X\n",
                            fcode, fcode, (unsigned char *) fcbp - tpa,
                            (fcbp->drive ? fcbp->drive - 1
                             : regp->driveuser >> 4) + 'A',
                            fcbp->eight, fcbp->three, fcbp->extent,
                            fcbp->cr, regp->dma - tpa);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        switch (fcode) {
          case 33:              /* Read Random. */
          case 34:              /* Write Random. */
          case 40:              /* Write Random with Zero Fill. */
            if (fcbp->dn[0] >= 0) {
                fcbp->cr = fcbp->rr[0] & 0x7F;
                fcbp->extent = fcbp->rr[1] << 1 | fcbp->rr[0] >> 7;
            }
            break;
        }
    } else {
        fcbp = 0;
    }
#ifdef TRACE
    if (regp->trcoptions & TRCSVC) {
        cp = linebuf;
        cp += mysprintf(cp, "BDOS %d ", fcode);
    }
#endif
    switch (fcode) {
      case 0:                   /* System Reset. */
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(System Reset)");
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        doexit(0);
        break;

      case 1:                   /* Console Input. */
        if (regp->miscflags & DUMMYSVC) {
            regp->rega = *lastregap;
        } else {
            flushconout(regp, 0);
            conin(regp, (char *) &regp->rega);
            *lastregap = regp->rega;
            if (regp->rega >= ' ' || regp->rega == '\n' ||
                regp->rega == '\r' || regp->rega == '\t' ||
                regp->rega == 8)
                conout(regp, regp->rega);
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Input): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 2:                   /* Console Output. */
        if (!(regp->miscflags & DUMMYSVC))
            conout(regp, regp->rege);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Output): %02X\n", regp->rege);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 5:                   /* List (print) output. */
        if (!(regp->miscflags & DUMMYSVC))
            listout(regp, regp->rege);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(List Output): %02X\n", regp->rege);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 6:                   /* Direct Console I/O. */
        if (regp->rege == 0xFF) {
            if (!(regp->miscflags & DUMMYSVC)) {
                flushconout(regp, 1);
                if (myrdchk(infile)) {
                    conin(regp, (char *) &regp->rega);
                    *lastregap = regp->rega;
                } else {
                    *lastregap = regp->rega = 0;
                }
            } else {
                regp->rega = *lastregap;
            }
#ifdef TRACE
            if (regp->trcoptions & TRCSVC) {
                cp += mysprintf(cp, "(Direct Console Input): %02X\n",
                                regp->rega);
                write(2, linebuf, cp - linebuf);
                debugoutput = 1;
            }
#endif
        } else {
            if (!(regp->miscflags & DUMMYSVC))
                conout(regp, regp->rege);
#ifdef TRACE
            if (regp->trcoptions & TRCSVC) {
                cp += mysprintf(cp, "(Direct Console Output): %02X\n",
                                regp->rege);
                write(2, linebuf, cp - linebuf);
                debugoutput = 1;
            }
#endif
        }
        break;

      case 9:                   /* Print String. */
        ucp1 = &tpa[(regp->regd << 8) | regp->rege];
        ucp2 = ucp1;
        while (*ucp2 != '$' && ucp2 < &tpa[TPASIZE])
            ucp2++;
        if (len = (ucp2 - ucp1))
            if (!(regp->miscflags & DUMMYSVC))
                conoutstr(regp, (char *) ucp1, len);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Print String): ");
            strncpy(cp, ucp1, len);
            cp += len;
            *cp++ = '\n';
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 10:                  /* Read Console Buffer. */
        ucp1 = &tpa[(regp->regd << 8) | regp->rege];
        if (!(regp->miscflags & DUMMYSVC)) {
            stdinlineon(regp);
            linelen = ucp1[0];
            do {
                linelen = read(0, lineinbuf, linelen);
            } while (linelen < 0 && errno == EINTR);
            if (linelen > 0) {
                ucp1[1] = linelen;
                memcpy(&ucp1[2], lineinbuf, linelen);
            } else {
                ucp1[1] = 0;
            }
            stdinlineoff(regp);
        } else {
            if (linelen > 0) {
                ucp1[1] = linelen;
                memcpy(&ucp1[2], lineinbuf, linelen);
            } else {
                ucp1[1] = 0;
            }
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Read Console Buffer): ");
            if (linelen > 0) {
                strncpy(cp, lineinbuf, linelen);
                cp += linelen;
            }
            *cp++ = '\n';
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 11:                  /* Get Console Status. */
        if (regp->miscflags & DUMMYSVC)
            regp->rega = *lastregap;
        else
            *lastregap = regp->rega = myrdchk(infile) ? 0xFF : 0;
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Console Status): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 12:                  /* Return Version Number. */
        regp->regb = regp->regh = 0; /* CP/M */
        regp->rega = regp->regl = 0x22; /* 2.2 */
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Version): %03X\n",
                            regp->regb << 8 | regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 13:                  /* Reset Disk System. */
        regp->driveuser &= 0x0F; /* Drive A, */
        regp->dma = &tpa[DEFDMA]; /* default DMA address. */
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Reset Disks)\n");
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 14:                  /* Select Disk. */
        regp->driveuser = (regp->rege & 0x0F) << 4 | (regp->driveuser & 0x0F);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Select Disk): %02X\n", regp->rege);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 15:                  /* Open File. */
        if (regp->miscflags & DUMMYSVC) {
            regp->rega = *lastregap;
            memset(&fcbp->s1, 0, 19);
            fcbp->dn[0] = lastdn[0];
            fcbp->dn[1] = lastdn[1];
            fcbp->dn[2] = lastdn[2];
        } else {
            *lastregap = regp->rega = 0xFF;     /* Assume failure. */
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
                        for (ioval=0; ioval<nfakefiles; ioval++) {
                            if (!strcmp(&cp2[2], fakefs[ioval].name)) {
                                fileio = ioval;
                                break;
                            }
                        }
                        if (fileio < 0) { /* Fake not found, open for real. */
                            ioval = open(ufn, OROF);
                            fileio = 0; /* No fake ID, either. */
                        } else {          /* Fake found, flag it so. */
                            ioval = 0;
                        }
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
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Open File): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 16:                  /* Close File. */
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
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Close File): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 17:                  /* Search For First. */
        cp2 = ambigname(fncanon(fcbp, 0, regp->driveuser >> 4));
        if (!(regp->miscflags & DUMMYSVC))
            getflist(regp, cp2); /* then fall into... */
      case 18:                  /* Search For Next. */
        if (dirlisthead) {
            ufntocpmfn(dirlisthead->name, regp->dma);
            if (((regp->driveuser>>4) + 1) == regp->dma[0])
                regp->dma[0] = 0; /* Default drive?  Make it so. */
#ifdef TRACE
            if (regp->trcoptions & TRCFCB) {
                cp2 = &linebuf[sizeof linebuf/2];
                cp2 += mysprintf(cp2, "\tdirent: %s\n", dirlisthead->name);
                write(2, &linebuf[sizeof linebuf/2],
                      cp2 - &linebuf[sizeof linebuf/2]);
                debugoutput = 1;
            }
#endif
#ifdef CONTEST
            if (regp->miscflags & DUMMYSVC)
#endif
                uncachefile(dirlisthead->name, &dirlisthead);
            *lastregap = regp->rega = 0;
        } else {
            *lastregap = regp->rega = 0xFF;
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Search %s): %02X\n",
                            (fcode == 17) ? "First" : "Next", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 19:                  /* Delete File. */
        if (regp->miscflags & DUMMYSVC) {
            regp->rega = *lastregap;
            break;
        }
        *lastregap = regp->rega = 0xFF; /* Assume Error. */
        if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
            if (ufn = cfntoufn(cp2)) {
                fileio = -1; /* Search fake file list first. */
                for (ioval=0; ioval<nfakefiles; ioval++) {
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
#ifdef TRACE
                if (regp->trcoptions & TRCFCB) {
                    cp2 = &linebuf[sizeof linebuf/2];
                    cp2 += mysprintf(cp2, "\tunlink %s: %d\n", ufn, regp->rega);
                    write(2, &linebuf[sizeof linebuf/2],
                          cp2 - &linebuf[sizeof linebuf/2]);
                    debugoutput = 1;
                }
#endif
            }
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Delete File): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 33:                  /* Read Random. */
      case 20:                  /* Read Sequential. */
        *lastregap = regp->rega = 1; /* Assume Error. */
        if (fcbp->dn[0] >= 0) {
            len = 0x4000 * fcbp->extent + 128 * fcbp->cr;
            if (fcbp->dn[0] == 0) { /* Fake file? */
                seekval = (len < *fakefs[fcbp->dn[2]].size) ? len : 0;
            } else {
                seekval = lseek(fcbp->dn[0], len, 0);
            }
            if (seekval == len) {
                if (fcbp->dn[0] == 0) { /* Fake file? */
                    memcpy(regp->dma, &fakefs[fcbp->dn[2]].data[seekval], 128);
                    ioval = 128;
                } else {
                    ioval = read(fcbp->dn[0], regp->dma, 128);
                }
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
            } else if (fcode == 33) { /* Invalid Random Read? */
                *lastregap = regp->rega = 6;
            }
#ifdef TRACE
            if (regp->trcoptions & TRCFCB) {
                cp2 = &linebuf[sizeof linebuf/2];
                cp2 += mysprintf(cp2,"\tfd: %d, seek: %04X, size: %d, A: %02X\n",
                                fcbp->dn[0], seekval, ioval, regp->rega);
                write(2, &linebuf[sizeof linebuf/2],
                      cp2 - &linebuf[sizeof linebuf/2]);
                debugoutput = 1;
            }
#endif
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Read %s): %02X\n",
                            (fcode == 33) ? "Random" : "Sequentiol",
                            regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 34:                  /* Write Random. */
      case 40:                  /* Write Random with Zero Fill. */
      case 21:                  /* Write Sequential. */
        *lastregap = regp->rega = 1; /* Assume Error. */
        if (fcbp->dn[0] > 0) {  /* Fake files are RO. */
            if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
                if (!fcbp->dn[1]) { /* Open for writing yet? */
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
#ifdef TRACE
            if (regp->trcoptions & TRCFCB) {
                cp2 = &linebuf[sizeof linebuf/2];
                cp2 += mysprintf(cp2,"\tfd: %d, seek: %04X, size: %d, A: %02X\n",
                                fcbp->dn[0], seekval, ioval, regp->rega);
                write(2, &linebuf[sizeof linebuf/2],
                      cp2 - &linebuf[sizeof linebuf/2]);
                debugoutput = 1;
            }
#endif
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Write %s): %02X\n",
                            (fcode == 34) ? "Random" :
                            (fcode == 30) ? "Random Zero-Fill" :
                                            "Sequentiol",
                            regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 22:                  /* Make File. */
        if (regp->miscflags & DUMMYSVC) {
            regp->rega = *lastregap;
            memset(&fcbp->s1, 0, 19);
            fcbp->dn[0] = lastdn[0];
            fcbp->dn[1] = lastdn[1];
        } else {
            *lastregap = regp->rega = 0xFF;     /* Assume error. */
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
#ifdef TRACE
                    if (regp->trcoptions & TRCFCB) {
                        cp2 = &linebuf[sizeof linebuf/2];
                        cp2 += mysprintf(cp2, "\tcreat %s: %d\n",
                                         ufn, regp->rega);
                        write(2, &linebuf[sizeof linebuf/2],
                              cp2 - &linebuf[sizeof linebuf/2]);
                        debugoutput = 1;
                    }
#endif
                }
            }
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Make File): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 23:                  /* Rename file. */
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
                        for (ioval=0; ioval<nfakefiles; ioval++) {
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
                            strncpy(fakefs[fileio].name, linebuf,
                                    sizeof fakefs[fileio].name);
                            *lastregap = regp->rega = 0;
                        }
                    }
                }
            }
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Rename File): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 24:                  /* Return Log-in Vector. */
        seekval = 1;
        for (len=0; len<(sizeof(drives)/sizeof(drives[0])); len++) {
            regp->regh = regp->regb |= (seekval >> 8);
            regp->regl = regp->rega |= seekval;
            seekval <<= 1;
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Log-in Vector): %04X\n",
                            regp->regb << 8 | regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 25:                  /* Return Current Disk. */
        regp->rega = regp->driveuser >> 4;
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Current Disk): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 26:                  /* Set DMA address. */
        regp->dma = &tpa[(regp->regd << 8) | regp->rege];
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Set DMA address): %04X\n",
                            regp->regd << 8 | regp->rege);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 32:                  /* Get/Set User Code. */
        if (regp->rege == 0xFF)
            regp->rega = regp->driveuser & 0x0F;
        else
            regp->driveuser = (regp->rege & 0x0F) | (regp->driveuser & 0xF0);
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Get/Set User): %02X\n", regp->rega);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 35:                  /* Compute File Size. */
        if (regp->miscflags & DUMMYSVC) {
            fcbp->rr[0] = lastdn[0];
            fcbp->rr[1] = lastdn[1];
            fcbp->rr[2] = lastdn[2];
        } else {
            if (cp2 = fncanon(fcbp, 0, regp->driveuser >> 4)) {
                if (ufn = cfntoufn(cp2)) {
                    fileio = -1; /* Search fake file list first. */
                    for (ioval=0; ioval<nfakefiles; ioval++) {
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
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Compute File Size)\n");
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 36:                  /* Set Random Record. */
        if (fcbp->dn[0] >= 0) {
            fcbp->rr[0] = fcbp->cr;
            fcbp->rr[1] = fcbp->extent;
            fcbp->rr[2] = 0;
        }
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Set Random Record)\n");
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
        break;

      case 37:                  /* Reset Drive. */
        regp->rega = 0;
#ifdef TRACE
        if (regp->trcoptions & TRCSVC) {
            cp += mysprintf(cp, "(Reset Drive): %04X\n",
                            regp->regd << 8 | regp->rege);
            write(2, linebuf, cp - linebuf);
            debugoutput = 1;
        }
#endif
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
#ifdef TRACE
#  if 0
    if (fcbp && regp->trcoptions & TRCFCB) { /* Hex-dump the FCB too. */
        int ii;

        cp = linebuf;
        cp += mysprintf(cp, "FCB %04X: ", (ucp1=(unsigned char *)fcbp) - tpa);
        for (ucp2=ucp1,ii=0; ii<sizeof(*fcbp); ii++) {
            cp += mysprintf(cp, "%02X ", *ucp1++);
            if ((ii % 16) == 15 || ii >= sizeof(*fcbp)-1) {
                while ((ii % 16) != 15) { /* Pad hex to ASCII field. */
                    if ((ii % 8) == 7) /* Extra space in middle of line. */
                        *cp++ = ' ';
                    *cp++ = ' '; *cp++ = ' '; *cp++ = ' ';
                    ii++;
                }
                *cp++ = ' '; *cp++ = '|'; /* Put out ASCII field. */
                while (ucp2 < ucp1) {
                    if (*ucp2 >= ' ' && *ucp2 <= '~')
                        *cp++ = *ucp2;
                    else
                        *cp++ = '.';
                    ucp2++;
                }
                *cp++ = '|';
                *cp++ = '\n';
                if (ii < sizeof(*fcbp)-1)
                    cp += mysprintf(cp, "    %04X: ", ucp1 - tpa);
            } else if ((ii % 8) == 7) /* Extra space in middle of line. */
                *cp++ = ' ';
        }
        *cp = 0;
        write(2, linebuf, cp - linebuf);
        debugoutput = 1;
    }
#  endif
#endif
}


/*
** Dump out the previous instruction.  For supplying context for
** bad opcode or simulation mismatch reports.
*/

void
dumplast(tpa, regp, linebuf)
unsigned char *tpa;
struct regs *regp;
char *linebuf;
{
    char *cp;
    unsigned char *lastpc;

    lastpc = regp->pppc;
    cp = linebuf;
    cp += mysprintf(cp, "\nPrevious instruction:"
                    "                               ");
#ifdef Z80
    if (*lastpc == 0xCB || *lastpc == 0xDD
        || *lastpc == 0xED || *lastpc == 0xFD)
        cp += mysprintf(cp, "%02X%02X %02X%02X ",
                        (lastpc-tpa)>>8,
                        (lastpc-tpa) & 0xFF,
                        lastpc[0], lastpc[1]);
    else
#endif
        cp += mysprintf(cp, "%02X%02X  %02X  ",
                        (lastpc-tpa)>>8,
                        (lastpc-tpa) & 0xFF,
                        lastpc[0]);
    cp += mysprintf(cp, "     %s",
                    decode(lastpc, regp->trcoptions & ZILOG, 1));
    write(2, linebuf, cp - linebuf);
}


/*
** What to do when an illegal 8080 instruction is detected.
*/

void
illegal(tpa)
unsigned char *tpa;
{
    char *cp, linebuf[128];
    int pc;
    unsigned char byte;
    struct regs *regp;

    flusher(0);
    regp = (struct regs *) (tpa - sizeof(struct regs));
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

void
dumpregs(tpa)
unsigned char *tpa;
{
#ifdef TRACE
    extern char cycletabl[256];
#  ifdef Z80
    extern char cycletabl1[256], cycletabl2[256], cycletabl3[256],
                cycletabl4[256], cycletabl5[256], cycletabl6[256];
#  endif
#endif
    char *cp, linebuf[128];
    int ii, sp, pc, bc, de, hl, bc2, de2, hl2;
    unsigned char byte, byte2;
    struct regs *regp;

    debugoutput = 1;
    regp = (struct regs *) (tpa - sizeof(struct regs));
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
#ifdef Z80
    if (regp->trcoptions & SHOWALT) { /* Only show 'em if they've been used. */
      showalts:
        if (!regp->trcreccnt) {
            cp = linebuf;
            cp += mysprintf(cp,
                            "\n%s-AF' -BC'    -DE'    -HL'    -IX- -IY- -IR-",
                            regp->marker);
            write(2, linebuf, cp - linebuf);
        }
        cp = linebuf;
        cp += mysprintf(cp,
                        "\n%s%02X%02X %02X%02X>%02X %02X%02X>%02X %02X%02X",
             (!regp->trcreccnt && regp->marker && (regp->trcoptions & SHOWALT))
                               ? &"   "[3-strlen(regp->marker)] : regp->marker,
                        regp->rega2, regp->regf2, regp->regb2,
                        regp->regc2, tpa[bc2], regp->regd2, regp->rege2,
                        tpa[de2], regp->regh2, regp->regl2);
        cp += mysprintf(cp, ">%02X %02X%02X %02X%02X %02X%02X",
                        tpa[hl2], regp->regxh, regp->regxl, regp->regyh,
                        regp->regyl, regp->regi, regp->seed & 0xFF);
        write(2, linebuf, cp - linebuf);
    } else if (regp->rega2 || regp->regf2 || regp->regb2 || regp->regc2 ||
               regp->regd2 || regp->rege2 || regp->regh2 || regp->regl2 ||
               regp->regxh || regp->regxl || regp->regyh || regp->regyl) {
        regp->trcoptions |= SHOWALT;
        regp->trcreccnt = 0;
        goto showalts;
    }
#endif
    if (!regp->trcreccnt) {
        cp = linebuf;
        cp += mysprintf(cp, "\n%s-AF- -BC-    -DE-    -HL-    "
                        "-SP- -S0- -S1- -S2- -PC- -op-",
                        (regp->marker && (regp->trcoptions & SHOWALT))
                              ? &"   "[3-strlen(regp->marker)] : regp->marker);
#ifdef TRACE
        cp += mysprintf(cp, " -cy-");
#endif
        write(2, linebuf, cp - linebuf);
    }
    cp = linebuf;
    cp += mysprintf(cp,
                    "\n%s%02X%02X %02X%02X>%02X %02X%02X>%02X %02X%02X>%02X "
                    "%02X%02X>",
           (regp->marker && (!regp->trcreccnt || (regp->trcoptions & SHOWALT)))
                               ? &"   "[3-strlen(regp->marker)] : regp->marker,
                    regp->rega, regp->regf, regp->regb, regp->regc, tpa[bc],
                    regp->regd, regp->rege, tpa[de], regp->regh, regp->regl,
                    tpa[hl], regp->sphi, regp->splo);
    cp += mysprintf(cp, "%02X%02X,%02X%02X,%02X%02X ",
                    tpa[sp+1], tpa[sp+0], tpa[sp+3], tpa[sp+2],
                    tpa[sp+5], tpa[sp+4]);
#ifdef Z80
    if (byte == 0xCB || byte == 0xDD || byte == 0xED || byte == 0xFD)
        cp += mysprintf(cp, "%02X%02X %02X%02X ",
                        regp->pchi, regp->pclo, byte, tpa[pc+1]);
    else
#endif
        cp += mysprintf(cp, "%02X%02X  %02X  ",
                        regp->pchi, regp->pclo, byte);
#ifdef TRACE
    ii = cycletabl[byte];
#  ifdef Z80
    if (byte == 0xCB || byte == 0xDD || byte == 0xED || byte == 0xFD) {
        byte2 = tpa[pc+1];
        if (byte == 0xCB)
            ii = cycletabl1[byte2];
        else if (byte == 0xDD) {
            ii = cycletabl2[byte2];
            if (byte2 == 0xCB)
                ii = cycletabl5[tpa[pc+3]];
        } else if (byte == 0xED)
            ii = cycletabl3[byte2];
        else if (byte == 0xFD) {
            ii = cycletabl4[byte2];
            if (byte2 == 0xCB)
                ii = cycletabl6[tpa[pc+3]];
        }
    }
#  endif
    cp += mysprintf(cp, "(%2d) ", ii);
    cp += mysprintf(cp, "%s", decode(regp->ppc, regp->trcoptions & ZILOG, 1));
#endif
    write(2, linebuf, cp - linebuf);
    regp->trcreccnt = ((regp->trcreccnt + 1) & (regp->trcoptions & SHOWALT ? 0x07 : 0x0F));
}

/*
** Single-step the target.  Actually do some command processing here?
*/

void
stepper(tpa)
unsigned char *tpa;
{
    int i;
    char linebuf[128];
    struct regs *regp;

    dumpregs(tpa);
    regp = (struct regs *) (tpa - sizeof(struct regs));
    if (regp->trcoptions & TRCSTEP) { /* Single-stepping? */
        stdinlineon(regp);
        do {
            i = read(0, linebuf, sizeof linebuf);
        } while (i < 0 && errno == EINTR);
        stdinlineoff(regp);
    }
}
