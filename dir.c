/*
** Some support modules for directory access.
*/

#include "com.h"

#ifdef NEXT
#define _POSIX_SOURCE
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef NEXT
#define dirent direct
#define CAST (struct dirent *)
#else
#define CAST
#endif

/*
** Simple filename pattern matcher.
*/

static int fmatch(name, pat) char *name, *pat;
{
  char ch, *p, *q;

  p = name;
  q = pat;
  while (ch = *p) {
    p++;
    if (*q == '*') {
      if (ch == q[1]) /* Ran out of wildcard? */
        q += 2;
      continue;
    }
    if (ch == *q || *q == '?') {
      q++;
      continue;
    }
    return 0;
  }
  /* Ran out of filename. */
  if (!*q) /* If we're also out of pattern, match. */
    return 1;
  if (*q && (*q == '*' || *q == '?') && !q[1]) /* Leftover's wildcards? */
    return 1;                                  /* Then we also must match. */
  return 0; /* Else non-wild pattern left, no match. */
}

/*
** Given a (probably ambiguous) Unix file name, make a list of the
** files that match it.  Doesn't use popen (to make the binary small
** --- we've avoided the whole buffered I/O package so far for just
** this reason).  Reads the directories directly.  This subroutine is
** fairly specific to Unix, unlike most of the others.
**
** This routine is complicated somewhat by the necessity of merging
** the fake file list (if any) with any given directory.
*/

struct fcache *getflist(regp, name) struct regs *regp;
char *name;
{
  char cmdbuf[MAXNAMLEN + 3], *cp;
  int drive, ii;
  DIR *dir;
  struct dirent *dire;
  struct stat st;

  while (dirlisthead) /* First delete existing directory list. */
    uncachefile(dirlisthead->name, &dirlisthead);
  if (!name || !*name)
    return dirlisthead;
  drive = name[0] - 'A';
  name[1] = 0;                /* Split off drive letter. */
  if (!(cp = cfntoufn(name))) /* No drive path? */
    return dirlisthead;
  name = &name[2];

  for (ii = 0; ii < nfakefiles; ii++) {
    if (fakefs[ii].name[0]) /* Not deleted? */
      if (fmatch(fakefs[ii].name, name)) {
        cmdbuf[0] = 'A' + drive;
        cmdbuf[1] = ':';
        strncpy(&cmdbuf[2], fakefs[ii].name, sizeof cmdbuf - 2);
        cachefile(cmdbuf, &dirlisthead);
      }
  }
  if (!(dir = opendir(cp))) /* Attempt to open the directory. */
    return dirlisthead;     /* Failed! */
  while (dire = CAST readdir(dir)) {
    if (dire->d_name[0] == '.')
      continue;
    if (fmatch(dire->d_name, name)) {
      strncpy(cmdbuf, cp, sizeof(cmdbuf) - 1);
      strncat(cmdbuf, "/", sizeof(cmdbuf) - 1);
      strncat(cmdbuf, dire->d_name, sizeof(cmdbuf) - 1);
      if (!stat(cmdbuf, &st)) {
        if ((st.st_mode & S_IFMT) == S_IFREG) {
          cmdbuf[0] = 'A' + drive;
          cmdbuf[1] = ':';
          strncpy(&cmdbuf[2], dire->d_name, sizeof cmdbuf - 2);
          cachefile(cmdbuf, &dirlisthead);
        }
      }
    }
  }
  closedir(dir);
  return dirlisthead;
}

/*
** Convert a filename from CP/M's FCB format to something Unix likes better.
** Format will be A:unix_basename which needs further conversion for host OS.
*/

char *fncanon(fcbp, offset, curdrive) struct fcb *fcbp;
int offset;
int curdrive;
{
  static char name[15]; /* A:8.3<NULL> */
  char *src, *dst, chr;
  int drive, i;

  dst = &name[2];
  src = fcbp->eight + offset;
  i = sizeof(fcbp->eight);
  while (i-- && (chr = *src++ & 0x7F) != ' ') {
    if (chr >= 'A' && chr <= 'Z')
      *dst++ = chr + ('a' - 'A');
    else
      *dst++ = chr;
    if (chr == '/')
      return 0; /* No Unix pathnames! */
  }
  src = fcbp->three + offset;
  i = sizeof(fcbp->three);
  if ((*src & 0x7F) != ' ')
    *dst++ = '.';
  while (i-- && (chr = *src++ & 0x7F) != ' ') {
    if (chr >= 'A' && chr <= 'Z')
      *dst++ = chr + ('a' - 'A');
    else
      *dst++ = chr;
    if (chr == '/')
      return 0; /* No Unix pathnames! */
  }
  *dst++ = 0;
  if (!name[2] || name[2] == '.') /* Stay away from invalid/hidden files. */
    return 0;

  if (!fcbp->drive) /* Put on drive letter. */
    drive = curdrive;
  else if (fcbp->drive < 17)
    drive = fcbp->drive - 1;
  name[0] = 'A' + drive;
  name[1] = ':';
  return name;
}

/*
** Convert canonical (A:unix_basename) file name to full pathed Unix filename.
** Lone drive letters return only the associated path.
*/

char *cfntoufn(name) char *name;
{
  int drive;
  static char path[512]; /* For compositing path+name. */

  drive = name[0] - 'A';
  if (!drives[drive]) /* No path specified? */
    if (!drive)       /* A:?  Assume cwd. */
      return name[1] ? &name[2] : ".";
    else
      return 0; /* No path, drive invalid. */
  strncpy(path, drives[drive], sizeof path);
  strncat(path, "/", sizeof(path) - 1);
  if (name[1])
    strncat(path, &name[2], sizeof(path) - 1);
  return path;
}

/*
** Convert a CP/M ambiguous name to a canonical (Unix-ey) ambiguous name.
*/

char *ambigname(name) char *name;
{
  static char newname[15];
  char *cp, chr;

  if (name && strchr(name, '?')) {
    cp = newname;
  again:
    while ((chr = *name) && chr != '?')
      *cp++ = *name++;
    if (chr == '?') {
      *cp++ = '*';
      while (*name == '?')
        name++;
      goto again; /* Pick up extension the same way. */
    }
    *cp++ = 0;
    if (!strcmp(&newname[2], "*.*")) /* All files? */
      newname[3] = 0;                /* Make into Unix's '*' then. */
    return newname;
  } else
    return name;
}

/*
** Many CP/M programs (i.e. WordStar) are extremely sloppy about not closing
** files, and even re-opening them again and again.  Make sure we don't do
** this, because we can't afford to leak file descriptors under Unix.
*/

struct fcache *cachedfile(name,
                          lheadp) /* Is the file already in the given cache? */
    char *name;
struct fcache **lheadp;
{
  struct fcache *fp;

  fp = *lheadp;
  while (fp) {
    if (!strcmp(name, fp->name))
      return fp;
    fp = fp->next;
  }
  return 0;
}

struct fcache *cachefile(name,
                         lheadp) /* Make sure the file's in the given cache. */
    char *name;
struct fcache **lheadp;
{
  struct fcache *fp;

  if (!cachedfile(name, lheadp)) {
    fp = malloc(sizeof(*fp));
    fp->name = malloc(strlen(name) + 1);
    strcpy(fp->name, name);
    fp->next = *lheadp;
    fp->fd = -1;
    fp->fakeid = -1;
    *lheadp = fp;
    return fp;
  }
  return 0;
}

void uncachefile(name, lheadp) /* Remove a file from the given cache. */
    char *name;
struct fcache **lheadp;
{
  struct fcache *fp, *pfp;

  pfp = 0;
  fp = *lheadp;
  while (fp) {
    if (!strcmp(name, fp->name)) {
      if (pfp)
        pfp->next = fp->next;
      else
        *lheadp = fp->next;
      free(fp->name);
      free(fp);
      return;
    }
    pfp = fp;
    fp = fp->next;
  }
}
