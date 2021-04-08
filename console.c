/*
** Some support modules for (buffered) console I/O and list (printer) output.
**
** Does buffering (for efficiency), and (optionally) converts H19 terminal to
** VT100 terminal sequences on the console, since some CP/M software doesn't
** know what a VT100 is, but they all seem to know what a H19 is. Optionally
** converts ASCII DEL to BS, because the CP/M world generally doesn't use DEL.
**
** Buffers are flushed upon input, buffer full, or timeout. There are two
** levels of timeout, a 'fast' one based upon the tick count returned by
** times(), and a gross catch-all based upon alarm(1). The fast timeout is
** only used by the CP/M polling routines. The routines that block don't
** bother. No timeouts are enabled unless there is buffered output. For some
** reason, on the NeXT only the gross timeout is functioning. The times() call
** there seems to always return 0. Finally, all buffering can be disabled.
*/

#include "com.h"

/*
** Redirect simulation input (temporarily) from standard input to a file.
*/

void redirectin(name) char *name;
{
  int fd;

  fd = open(name, OROF);
  if (fd >= 0)
    infile = fd;
}

/*
** Reprogram stdin for single-character raw I/O.
*/

void setupstdin(regp) struct regs *regp;
{
  signal(SIGTERM, catcher);
  signal(SIGHUP, catcher);
  signal(SIGINT, catcher);
  signal(SIGALRM, flusher);
  signal(SIGUSR1, u1catcher);
  signal(SIGUSR2, u2catcher);
#if SYSV || DNIX
#if defined(__FreeBSD__) || \
    defined(__OpenBSD__) || \
    defined(__NetBSD__)
  ioctl(0, TIOCGETA, &old);
#else
  ioctl(0, TCGETA, &old);
#endif
  new = old;
  new.c_cc[VMIN] = 1;
  new.c_cc[VTIME] = 0;
  new.c_iflag &= ~(ICRNL | INLCR | IXON);
  new.c_oflag &= ~(OCRNL | ONLCR);
  new.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL);
  if (!(regp->miscflags & NOIOCTL)) {
#if defined(__FreeBSD__) || \
	defined(__OpenBSD__) || \
	defined(__NetBSD__)
    ioctl(0, TIOCSETA, &new);
#else
    ioctl(0, TCSETA, &new);
#endif
  }
#elif BSD
  ioctl(0, TIOCGETP, &old);
  new = old;
  new.sg_flags |= RAW;
  new.sg_flags &= ~(ECHO | CRMOD);
  if (!(regp->miscflags & NOIOCTL))
    ioctl(0, TIOCSETP, &new);
#endif
}

/*
** Put stdin back to normal.
*/

void restorestdin() {
#if SYSV || DNIX
#if defined(__FreeBSD__) || \
	defined(__OpenBSD__) || \
	defined(__NetBSD__)
  ioctl(0, TIOCSETA, &old);
#else
  ioctl(0, TCSETAW, &old);
#endif
#elif BSD
  ioctl(0, TIOCSETP, &old);
#endif
}

/*
** Temporarily turn on stdin's line cooker again.
*/

void stdinlineon(regp) struct regs *regp;
{
  if (regp->miscflags & NOIOCTL)
    return;
#if SYSV || DNIX
  new.c_iflag |= (ICRNL | IXON);
  new.c_oflag |= ONLCR;
  new.c_lflag |= (ICANON | ECHO | ECHOE | ECHOK | ECHONL);
#if defined(__FreeBSD__) || \
  defined(__OpenBSD__) || \
  defined(__NetBSD__)
  ioctl(0, TIOCSETA, &new);
#else
  ioctl(0, TCSETAW, &new);
#endif
#elif BSD
  new.sg_flags |= (ECHO | CRMOD);
  new.sg_flags &= ~RAW;
  ioctl(0, TIOCSETP, &new);
#endif
}

/*
** Turn the line cooker back off.
*/

void stdinlineoff(regp) struct regs *regp;
{
  if (regp->miscflags & NOIOCTL)
    return;
#if SYSV || DNIX
  new.c_iflag &= ~(ICRNL | IXON);
  new.c_oflag &= ~ONLCR;
  new.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL);
#if defined(__FreeBSD__) || \
    defined(__OpenBSD__) || \
    defined(__NetBSD__)
  ioctl(0, TIOCSETA, &new);
#else
  ioctl(0, TCSETAW, &new);
#endif
#elif BSD
  new.sg_flags |= RAW;
  new.sg_flags &= ~(ECHO | CRMOD);
  ioctl(0, TIOCSETP, &new);
#endif
}

/*
** A rdchk() function for systems that don't have it already.  The
** SYSV version is pretty inefficient (3 system calls vs 1), since many
** CP/M programs poll the console a lot.  If there is a pending input
** character get it and prevent further polling until it's consumed.
** The CYGW variant only exists because Cygwin's implementation of
** VMIN/VTIME doesn't work right, so for that poll() is used instead.
**
** Returns non-zero when there's something to read.
*/

int myrdchk(fd) int fd;
{
  int i;
  unsigned char chr;

  if (holdingchar >= 0)
    return 1;
  else {
#if DNIX
    if (!rdchk(fd))
      return 0;
#elif CYGW
    {
      struct pollfd pfd;

      ioctl(fd, TCSETAW, &new); /* Not sure why; Cygwin insists. */
      pfd.fd = fd;
      pfd.events = POLLIN;
      return poll(&pfd, 1, 0);
    }
#elif SYSV
    new.c_cc[VMIN] = 0;
    new.c_cc[VTIME] = 0;
    new.c_lflag &= ~ICANON;
#if defined(__FreeBSD__) || \
	defined(__OpenBSD__) || \
	defined(__NetBSD__)
    ioctl(fd, FIONREAD, &i);
    if (!i)
      return 0;
#else
    ioctl(fd, TCSETAW, &new);
#endif
#elif BSD
    ioctl(fd, FIONREAD, &i);
    if (!i)
      return 0;
#endif
    do
      i = read(fd, &chr, 1);
    while (i < 0 && errno == EINTR);
#if SYSV
    new.c_cc[VMIN] = 1;
#if defined(__FreeBSD__) || \
	defined(__OpenBSD__) || \
	defined(__NetBSD__)
    ioctl(fd, TIOCSETA, &new); 
#else
    ioctl(fd, TCSETAW, &new);
#endif
#endif
    if (i > 0) {
      holdingchar = chr;
      return 1;
    } else {
      if (infile) {
        close(infile);
        infile = 0; /* Back to standard input. */
      }
      return 0;
    }
  }
}

/*
** Console-in function.
*/

void conin(regp, cp) struct regs *regp;
char *cp;
{
  unsigned char chr;
  int i;

  if (holdingchar >= 0) {
    chr = holdingchar;
    holdingchar = -1;
  } else {
  again:
    chr = 0; /* In case of error. */
    do
      i = read(infile, &chr, 1);
    while (i < 0 && errno == EINTR);
    if (i <= 0 && infile) {
      close(infile);
      infile = 0; /* Back to standard input. */
      goto again;
    }
  }
  if (!(regp->miscflags & NODELTOBS) && chr == 0x7F)
    chr = 8; /* Convert DEL to backspace? */
  *cp = chr;
}

/*
** Write character(s) to console out, with optional H19 to VT100 translation.
**
** There's a couple of other oddball translations here too, for a copy of
** PILOT I had lying around.  I'm not sure exactly what terminal it wanted.
*/

void conout(regp, chr) struct regs *regp;
char chr;
{
  char linebuf[20], *cp;

  if (!regp->conbuf) {
    regp->conbuf = malloc(SBUFSIZE);
    regp->conpos = 0;
  }
  switch (regp->vtstate) {
  case idle:
    if (chr == 0x1A     /* ADM-31?  For PILOT. */
        || chr == 0x0C) /* Formfeed. */
      goto cls;
    if (chr == 0x1B) {
      regp->vtstate = esc;
      break;
    } /* Fall through. */
  case off:
    bufconout(regp, &chr, 1);
    break;

  case esc:
    switch (chr) {
    case '@': /* Enter insert mode. */
      regp->vtstate = idle;
      bufconout(regp, "\033[4h", 4);
      break;

    case 'A': /* Cursor Up. */
    case 'B': /* Cursor Down. */
    case 'C': /* Cursor Forward. */
    case 'D': /* Cursor Back. */
    case 'H': /* Home. */
    case 'J': /* Erase Screen from cursor. */
    case 'K': /* Erase Line from cursor. */
    case 'L': /* Insert Line. */
    case 'M': /* Delete Line. */
    case 'N': /* Delete Character. */
      regp->vtstate = idle;
      linebuf[0] = '\033';
      linebuf[1] = '[';
      linebuf[2] = chr;
      bufconout(regp, linebuf, 3);
      break;

    case 'E': /* Home and Erase. */
    cls:
      regp->vtstate = idle;
      bufconout(regp, "\033[H\033[J\033[m", 9);
      break;

    case 'O': /* Exit insert mode. */
      regp->vtstate = idle;
      bufconout(regp, "\033[4l", 4);
      break;

    case 'Y': /* Cursor Move. */
    case '=': /* ADM-31-ish cursor move?  For PILOT. */
      regp->vtstate = waitrow;
      break;

    case 'p': /* Enter Reverse Video. */
      regp->vtstate = idle;
      bufconout(regp, "\033[7m", 4);
      break;

    case 'q': /* Exit Reverse Video. */
    case ')': /* Exit Bold.  ADM-31?  For PILOT. */
      regp->vtstate = idle;
      bufconout(regp, "\033[m", 3);
      break;

    case '(': /* Enter Bold.  ADM-31?  For PILOT. */
      regp->vtstate = idle;
      bufconout(regp, "\033[1m", 4);
      break;

    case 'v': /* Enable Autowrap. */
      regp->vtstate = idle;
      bufconout(regp, "\033[?7h", 5);
      break;

    case 'w': /* Disable Autowrap. */
      regp->vtstate = idle;
      bufconout(regp, "\033[?7l", 5);
      break;

    case 'I': /* Reverse Index. */
      regp->vtstate = idle;
      chr = 'M';
    default:
      regp->vtstate = idle;
      linebuf[0] = '\033';
      linebuf[1] = chr;
      bufconout(regp, linebuf, 2);
    }
    break;

  case waitrow:
    regp->vtstate = waitcol;
    regp->vtrow = chr + 1 - ' ';
    break;

  case waitcol:
    regp->vtstate = idle;
    cp = linebuf;
    cp += mysprintf(cp, "\033[%d;%dH", regp->vtrow, chr + 1 - ' ');
    bufconout(regp, linebuf, cp - linebuf);
    break;
  }
}

void conoutstr(regp, str, len) struct regs *regp;
char *str;
int len;
{
  if (regp->vtstate != off)
    while (len--)
      conout(regp, *str++);
  else
    bufconout(regp, str, len);
}

static void bufconout(regp, str, len) struct regs *regp;
char *str;
int len;
{
  int i, chpos;
  char *linebuf;

  if (regp->miscflags & NOBUFFER || !regp->conbuf) {
    while (len) { /* Unbuffered output. */
      i = write(1, str, len);
      if (i < 0) {
        if (errno == EINTR)
          continue;
        else
          break;
      }
      len -= i;
      str += i;
    }
    return;
  }
  regp->consemaphore = 1;
  chpos = regp->conpos;
  linebuf = regp->conbuf;
  while (len--) {
    linebuf[chpos++] = *str++;
    if (chpos >= SBUFSIZE) {
      regp->trcreccnt = 0; /* Output forces new trace label line. */
      alarm(0);
      while (chpos) {
        i = write(1, linebuf, chpos);
        if (i < 0) {
          if (errno == EINTR)
            continue;
          else
            break;
        }
        chpos -= i;
        linebuf += i;
      }
      chpos = 0;
      linebuf = regp->conbuf;
    } else
      alarm(1);
  }
  regp->conpos = chpos;
  regp->consemaphore = 0;
}

void flushconout(regp, dopoll) struct regs *regp;
int dopoll;
{
  int tick, i, lastflushtick, chpos;
  char *linebuf;
  struct tms tm;

  if (regp->miscflags & NOBUFFER || !regp->conbuf)
    return;
  regp->consemaphore = 1;
  if (chpos = regp->conpos) {
    regp->trcreccnt = 0; /* Output forces new trace label line. */
    tick = lastflushtick = regp->lastconflushtick;
    if (!(tick = times(&tm))) /* Stupid NeXT! */
      tick = tm.tms_utime;
    if (!dopoll || tick > lastflushtick + 5) {
      alarm(0);
      linebuf = regp->conbuf;
      while (chpos) {
        i = write(1, linebuf, chpos);
        if (i < 0) {
          if (errno == EINTR)
            continue;
          else
            break;
        }
        chpos -= i;
        linebuf += i;
      }
      chpos = 0;
      regp->lastconflushtick = tick;
    }
    regp->conpos = chpos;
  }
  regp->consemaphore = 0;
}

void listout(regp, chr) struct regs *regp;
char chr;
{
  char linebuf[20], *cp;

  regp->listsemaphore = 1;
  if (!regp->listbuf && regp->listname) {
    regp->listbuf = malloc(SBUFSIZE);
    regp->listpos = 0;
    regp->listfd = open(regp->listname, OWOF, 0666);
  }
  buflistout(regp, &chr, 1);
  regp->listsemaphore = 0;
}

static void buflistout(regp, str, len) struct regs *regp;
char *str;
int len;
{
  int i, chpos;
  char *linebuf;

  if (regp->listfd < 0)
    return;
  if (regp->miscflags & NOBUFFER || !regp->listbuf) {
    while (len) {
      i = write(regp->listfd, str, len);
      if (i < 0) {
        if (errno == EINTR)
          continue;
        else
          break;
      }
      len -= i;
      str += i;
    }
    return;
  }
  chpos = regp->listpos;
  linebuf = regp->listbuf;
  while (len--) {
    linebuf[chpos++] = *str++;
    if (chpos >= SBUFSIZE) {
      while (chpos) {
        i = write(regp->listfd, linebuf, chpos);
        if (i < 0) {
          if (errno == EINTR)
            continue;
          else
            break;
        }
        chpos -= i;
        linebuf += i;
      }
      chpos = 0;
      linebuf = regp->listbuf;
    }
  }
  regp->listpos = chpos;
}

void flushlistout(regp, dopoll) struct regs *regp;
int dopoll;
{
  int tick, i, lastflushtick, chpos;
  char *linebuf;
  struct tms tm;

  if (regp->listfd < 0)
    return;
  if (regp->miscflags & NOBUFFER || !regp->listbuf)
    return;
  regp->listsemaphore = 1;
  if (chpos = regp->listpos) {
    tick = lastflushtick = regp->lastlistflushtick;
    if (!dopoll || (tick = times(&tm)) > lastflushtick + 5) {
      linebuf = regp->listbuf;
      while (chpos) {
        i = write(regp->listfd, linebuf, chpos);
        if (i < 0) {
          if (errno == EINTR)
            continue;
          else
            break;
        }
        chpos -= i;
        linebuf += i;
      }
      chpos = 0;
      regp->lastlistflushtick = tick;
    }
    regp->listpos = chpos;
  }
  regp->listsemaphore = 0;
}

void flusher(signum) int signum;
{
  struct regs *regp;

  regp = gregp;
  signal(SIGALRM, flusher);
  if (!regp->consemaphore)
    flushconout(gregp, 0);
  if (!regp->listsemaphore)
    flushlistout(gregp, 0);
}

#ifndef CYGW
static char *itoa(val, radix, leadfill, ndigits) int val, radix, leadfill,
    ndigits;
{
  short i;
  char *p, *p2, out;
  static char buf[20];
  unsigned long test, remainder;

  p = buf;
  remainder = val;
  i = ndigits;       /* Make sure there're at least the number */
  while (i--)        /* of leading whatevers that we've asked for */
    *p++ = leadfill; /* as the field width. */
  p2 = p;
  if (radix == 16) /* Hex output? */
    test = 0x10000000L;
  else {
    test = 1000000000L; /* Decimal output? */
    if (val < 0) {
      remainder = -val;
      *p++ = '-';
    }
  }
  while (test > remainder) /* Scale down to match number. */
    test /= radix;
  if (remainder)
    while (test) {
      out = remainder / test;
      *p++ = out + ((out >= 10) ? '7' : '0');
      remainder %= test;
      test /= radix;
    }
  else
    *p++ = '0';
  *p = 0;
  if (ndigits) {             /* A field width set? */
    if ((p2 + ndigits) >= p) /* Make sure we never truncate. */
      return p - ndigits;
    else
      return p2;
  }
  return buf; /* No field width, return minimum number. */
}
#endif

/*
** Provide our own atoi() function because the standard one uses ctype
** and is overly large (for some uses).  Handles hexadecimal input too.
*/

int myatoi(p) /* Parse out a number. */
    register char *p;
{
  register int val, radix;
  register char chr;

  radix = 10;
  val = 0;
  while (1) {
    if (*p == '$' || *p == 'x' || *p == 'X') {
      radix = 16;
      p++;
      val = 0;
      continue;
    }
    chr = *p++ - '0';
    if (chr > 0x16)
      chr -= 0x20;
    if (chr > 9)
      chr -= 7;
    if (chr < 0 || chr >= radix)
      break;
    val = (val * radix) + chr;
  }
  return val;
}
