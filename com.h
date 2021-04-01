/*
** Global, function, and structure definitions for the CP/M 2.2 Simulator.
*/

#if CYGW
#  define SYSV 1
#endif
#if NEXT
#  define BSD 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#if CYGW
#  include <sys/poll.h>
#endif
#if SYSV || DNIX
#  include <termio.h>
#  define ORWF O_RDWR|O_NOCTTY
#  define OROF O_RDONLY|O_NOCTTY
#  define OWOF O_WRONLY|O_CREAT|O_TRUNC|O_NOCTTY
#elif BSD
#  include <sgtty.h>
#  define ORWF O_RDWR
#  define OROF O_RDONLY
#  define OWOF O_WRONLY|O_CREAT|O_TRUNC
#else
#error Must select system type that resolves to DNIX, SYSV, or BSD.
#endif
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>

/*
** Some interesting CP/M constants.
*/

#define TPASIZE 65536           /* Size of Transient Program Area (TPA). */
#define TPAPAD 128              /* Pad for run-off/wild index protection. */
#define FDOSSIZE 256            /* Size of FDOS area of memory. */
#define LOADADDR 0x100          /* Code load address within TPA. */
#define FCB1 0x5C               /* First FCB address. */
#define FCB2 0x6C               /* Second FCB address. */
#define DEFDMA 0x80             /* Default DMA address. */

/* 8080 register set and misc storage.  The pseudo-machine context. */
/* Note that this structure should be extended at the _front_, as it is */
/* referenced via negative offsets from the TPA pointer within the */
/* assembly-language module.  (This saves a register.)  Beware of */
/* padding issues when changing this structure! */

struct regs {
    char marker[4];             /* Prefix in dump display. */
    char *listname;             /* Name of list output file. */
    char *conbuf;               /* Console output buffer. */
    char *listbuf;              /* List output buffer. */
    long miscflags;
    short conpos;
    unsigned char listpos;
    char listfd;
    long lastconflushtick;      /* For automatic flush timing. */
    long lastlistflushtick;
    volatile short consemaphore; /* Console arbitration semaphore. */
    volatile short listsemaphore; /* List arbitration semaphore. */
    unsigned long *svcount;
    unsigned long basetime;
    unsigned long mhz;
                                /* Beyond here there are assembly language */
                                /* offsets that must match!  Change w/care! */
/*-100*/   unsigned long cycles;    /* Cycle counter. */
/*-96*/    unsigned long *opcount[7]; /* Opcode counters (optional). */
/*-68*/    char vtstate;
/*-67*/    char vtrow;
/*-66*/    char trcflag;            /* Tracing enabled flag. */
/*-65*/    char trcoptions;
/*-64*/    unsigned char *trcbegin; /* Beginning trace address. */
/*-60*/    unsigned char *trcend;   /* Beginning trace address. */
/*-56*/    unsigned char *dma;      /* DMA (disk buffer) address. */
/*-52*/    unsigned char *ppctmp;
/*-48*/    unsigned char *pppc;     /* 68K's version of the 8080's prior PC. */
/*-44*/    unsigned char *ppc;      /* 68K's version of the 8080's PC. */
/*-40*/    unsigned char *psp;      /* 68K's version of the 8080's SP. */
/*-36*/    unsigned char badflag;   /* TRUE when bad opcode detected. */
/*-35*/    unsigned char running;   /* 0==stopping, 1==step, 2==run. */
/*-34*/    unsigned char pchi;      /* (These regs are kept in 68K registers */
/*-33*/    unsigned char pclo;      /* most of the time, but are copied here */
/*-32*/    unsigned char sphi;      /* for service calls, in case of need.) */
/*-31*/    unsigned char splo;
/*-30*/    unsigned char rega;
/*-29*/    unsigned char regf;
/*-28*/    unsigned char regr;
/*-27*/    unsigned char regiff;
/*-26*/    unsigned char driveuser; /* Drive and user code. */
/*-25*/    unsigned char trcreccnt; /* For emitting periodic trace headers. */
/*-24*/    unsigned short seed;     /* Pseudo-Random seed for LD A,R. */

                        /* These following are actually kept only in memory. */
/*-22*/    unsigned char regb;
/*-21*/    unsigned char regc;
/*-20*/    unsigned char regd;
/*-19*/    unsigned char rege;
/*-18*/    unsigned char regh;
/*-17*/    unsigned char regl;
/*-16*/    unsigned char regxh; /* Z-80 index registers. */
/*-15*/    unsigned char regxl;
/*-14*/    unsigned char regyh;
/*-13*/    unsigned char regyl;
/*-12*/    unsigned char regb2; /* Alternate BCDEHL register set (Z-80). */
/*-11*/    unsigned char regc2;
/*-10*/    unsigned char regd2;
/* -9*/    unsigned char rege2;
/* -8*/    unsigned char regh2;
/* -7*/    unsigned char regl2;
/* -6*/    unsigned char rega2; /* Alternate AF register set (Z-80). */
/* -5*/    unsigned char regf2;
/* -4*/    unsigned char regi;          /* Interrupt page register (Z-80). */
/* -3*/    unsigned char operand1;      /* Operands for DAA storage. */
/* -2*/    unsigned char operand2;
/* -1*/    unsigned char operand3;      /* Operand 3 for DAA fakery. */
};

#if SVR4 || SYSV || DNIX
struct termio old, new;         /* Terminal control structures. */
#else
struct sgttyb old, new;
#endif

char *infilen;
int infile;
int holdingchar;                /* For simulating rdchk() function. */

struct fcb {                    /* CP/M File Control Block */
    unsigned char drive;        /* 0 == current, 1 == A: 2 == B:...16 == P: */
    char eight[8];              /* File Name. */
    char three[3];
    unsigned char extent;
    unsigned char s1, s2;       /* Reserved. */
    unsigned char rc;           /* Record count. */
    char dn[16];                /* Reserved for CP/M, used by Unix (ha!) */
                                /* [0]=real fd, [1]=RO/RW flag, [2]=embed fd */
    unsigned char cr;           /* Current Record. */
    unsigned char rr[3];        /* Random Record (optional). */
};

struct fcache {                 /* Cache of Unix file names, etc. */
    struct fcache *next;
    char *name;
    int fd;                     /* Zero for fake file index. */
    int fakeid;
} *openfilehead, *dirlisthead; /* Open file and file directory lists. */

struct fakefile {       /* Structure for accessing embedded file data. */
    char *name;
    char *data;
    int *size;
};

extern struct fakefile fakefs[];
extern int nfakefiles;

char *drives[16];               /* Drive A-P path prefixes. */

/* Some forward declarations and prototypes.  DNIX's C compiler lets us */
/* mix prototypes with old-style function definitions.  There is no */
/* ambiguity --- the prototype when present governs all.  Would that more */
/* compilers were this way...  I don't like new-style function definitions. */
/* Old-style definitions make for a slightly higher level of abstraction, */
/* you don't have to wade through typing mishmash to find the names of your */
/* arguments.  Most of the time I don't care about the types.  Only when */
/* debugging do I usually give them a close look.  */

#ifdef DNIX
int opcodesimulator(unsigned char *tpa);
int mysprintf();
void catcher(int signum), flusher(int signum);
void u1catcher(int signum), u2catcher(int signum);
void parseargs(unsigned char *tpa, int argc, char **argv);
void loadfdos(unsigned char *tpa, struct regs *regp);
void doexit(int exitcode);
void dumpcounts(struct regs *regp);
void docore(unsigned char *tpa, struct regs *regp);
void loadcore(unsigned char *tpa, unsigned char *where);
void ufntocpmfn(char *name, unsigned char *fcb);
char *cfntoufn(char *name);
void service(unsigned char *tpa);
void bdos(unsigned char *tpa, struct regs *regp, unsigned char *lastrega);
char *fncanon(struct fcb *fcbp, int offset, int drive);
char *ambigname(char *name);
struct fcache *cachedfile(char *name, struct fcache **headp);
struct fcache *cachefile(char *name, struct fcache **headp);
void uncachefile(char *name, struct fcache **headp);
static int fmatch(register char *name, register char *pat);
struct fcache *getflist(struct regs *regp, char *name);
void service(unsigned char *tpa);
int comparison(struct record *a, struct record *b);
void printrec(struct regs *regp, unsigned char *ops, char *mnep,
              unsigned long count, int cycles);
void dumplast(unsigned char *tpa, struct regs *regp, char *linebuf);
void dumpregs(unsigned char *tpa);
void stepper(unsigned char *tpa);
void fnuncanon(char *name, struct fcb *fcbp);
void redirectin(char *name);
void setupstdin(struct regs *regp);
void restorestdin(void);
void stdinlineon(struct regs *regp);
void stdinlineoff(struct regs *regp);
int myrdchk(int fd);
void conin(struct regs *regp, char *cp);
void conout(struct regs *regp, char chr);
void conoutstr(struct regs *regp, char *str, int len);
static void bufconout(struct regs *regp, char *str, int len);
void flushconout(struct regs *regp, int dopoll);
void listout(struct regs *regp, char chr);
static void buflistout(struct regs *regp, char *str, int len);
void flushlistout(struct regs *regp, int dopoll);
static char *itoa(int val, int radix, int leadfill, int ndigits);
int myatoi(char *str);
char *decode(unsigned char *ppc, int zilog, int operands);
void tdecode(char *table, char *outbuf, unsigned char prefix,
             unsigned char opcode, unsigned char *ppc, int zilog, int opernds);
int CBop(register unsigned char uc, register struct regs *regp, register unsigned char *tpa);
int DDCBop(register unsigned char op, register unsigned char uc, register struct regs *regp, register unsigned char *tpa);
int FDCBop(register unsigned char op, register unsigned char uc, register struct regs *regp, register unsigned char *tpa);
#else /* Not DNIX */
#define mysprintf sprintf
int opcodesimulator();
void catcher(), flusher();
void u1catcher(), u2catcher();
void parseargs();
void loadfdos();
void doexit();
void dumpcounts();
void docore();
void loadcore();
void ufntocpmfn();
char *cfntoufn();
void service();
void bdos();
char *fncanon();
char *ambigname();
struct fcache *cachedfile();
struct fcache *cachefile();
void uncachefile();
static int fmatch();
struct fcache *getflist();
void service();
int comparison();
void printrec();
void dumplast();
void dumpregs();
void stepper();
void fnuncanon();
void redirectin();
void setupstdin();
void restorestdin();
void stdinlineon();
void stdinlineoff();
int myrdchk();
void conin();
void conout();
void conoutstr();
static void bufconout();
void flushconout();
void listout();
static void buflistout();
void flushlistout();
static char *itoa();
int myatoi();
char *decode();
void tdecode();
static int CBop();
static int DDCBop();
static int FDCBop();
#endif /* DNIX */

char *progname;
struct regs *gregp, *gregp2;    /* Globals for signal catchers. */
int debugoutput;

                                /* Tracing flags. */
#define TRCSTEP 1               /* Single-step during trace. */
#define TRCFCB 2                /* Print out FCB's on BDOS calls. */
#define TRCSVC 4                /* Print out BIOS/BDOS calls. */
#define TRCCORE 8               /* Make an 8080 core file on error. */
#define TRCCOUNT 16             /* Count opcodes. */
#define SHOWALT 32              /* TRUE when alternate registers are used. */
#define ZILOG 64                /* Use Zilog mnemonics for disassembly. */
#define MHZ 128                 /* Simulate real N MHz timing. */

                                /* Miscellaneous flags. */
#define NOIOCTL 1               /* Don't do console IOCTL's, for adb debug. */
#define NODELTOBS 2             /* Don't map DEL to BS. */
#define NOBUFFER 4              /* Don't buffer character output. */
#define DUMMYSVC 8              /* Dummy servicing, for ASM vs C contest. */

/*
** States for terminal conversion.
*/

enum STATE {
    off,
    idle,
    esc,
    waitrow,
    waitcol
};

#define SBUFSIZE 160            /* Size of character output buffers. */
int loadcom();
