/*
** Some support modules for directory access.
*/

#include "com.h"

#ifdef USEDIRENT

#  ifdef NEXT
#    define _POSIX_SOURCE
#  endif

#  include <sys/types.h>
#  include <dirent.h>
#  include <sys/stat.h>

#  ifdef NEXT
#    define dirent direct
#    define CAST (struct dirent *)
#  else
#    define CAST
#  endif

/*
** Simple filename pattern matcher.
*/

static int
fmatch(name, pat)
char *name, *pat;
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
	if (!*q)                                        /* If we're also out of pattern, match. */
		return 1;
	if (*q && (*q == '*' || *q == '?') && !q[1])    /* Leftover's wildcards? */
		return 1;                               /* Then we also must match. */
	return 0;                                       /* Else non-wild pattern left, no match. */
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

struct fcache *
getflist(regp, name)
struct regs *regp;
char *name;
{
	char cmdbuf[MAXNAMLEN + 3], *cp;
	int drive, ii;
	DIR *dir;
	struct dirent *dire;
	struct stat st;

	while (dirlisthead)     /* First delete existing directory list. */
		uncachefile(dirlisthead->name, &dirlisthead);
	if (!name || !*name)
		return dirlisthead;
	drive = name[0] - 'A';
	name[1] = 0;                    /* Split off drive letter. */
	if (!(cp = cfntoufn(name)))     /* No drive path? */
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
	if (!(dir = opendir(cp)))       /* Attempt to open the directory. */
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

#else /* Don't use DIRENT libraries. */

/*
** Given a (probably ambiguous) Unix file name, make a list of the
** files that match it.  Doesn't use popen (to make the binary small
** --- we've avoided the whole buffered I/O package so far for just
** this reason).  Doesn't read the directories directly, for some
** semblance of portability.  This subroutine is fairly specific to
** Unix, unlike most of the others.  Could be converted to use
** getdents/readdir and regexp, but I doubt it would look any cleaner
** and I doubt it would be any more portable.  If this implementation
** doesn't work for you you'll just have to rewrite this routine to
** suit yourself.  Right now it forks off one or more shells to run
** find or ls, and sed.
**
** If we're in a world where subdirectories can exist, we need to
** USEFIND, as ls won't (easily) filter them out, nor is the ls code
** set up to work with drive paths.
**
** This routine is complicated somewhat by the necessity of merging
** the fake file list (if any) with any given directory.
*/

#  define USEFIND

struct fcache *
getflist(regp, name)
struct regs *regp;
char *name;
{
	int fd, child, pid, drive;
	struct stat st;
	char dirfile[20], cmdbuf[512], *fname, *fbuf, *cp, *mktemp();
#  ifdef USEFIND
	char *cmd = "cd ";
	char *cmd2 = " && find . '(' ! -name . -type d -prune ')' -o '(' -name '";
	char *cmd3 = "' ! -name '.*' -type f -print ')' | sed -e 's@^.*/@@'";
#  else
	char *cmd = "exec /bin/ls -dr ";
#  endif
	char *fcmd = "exec sed -n -e '/^";
	char *fcmd2 = "$/p'";

	while (dirlisthead)     /* First delete existing directory list. */
		uncachefile(dirlisthead->name, &dirlisthead);
	if (!name || !*name)
		return dirlisthead;
#  ifdef USEFIND
	drive = name[0] - 'A';
	name[1] = 0;                    /* Split off drive letter. */
	if (!(cp = cfntoufn(name)))     /* No drive path? */
		return dirlisthead;
	name = &name[2];
	mysprintf(cmdbuf, "%s%s%s%s%s", cmd, cp, cmd2, name, cmd3);
#  else
	strcpy(cmdbuf, cmd);
	strncat(cmdbuf, name, sizeof(cmdbuf));
#  endif
	strcpy(dirfile, "/tmp/comXXXXXX");
	fname = mktemp(dirfile);
	if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0600)) >= 0) {
		unlink(fname);          /* My private scratch file, thank you! */
		if (!(child = fork())) {
			int fds[2];     /* Child. */

			if (fd != 1) {
				dup2(fd, 1); /* Child & grandchild write stdout to file. */
				close(fd);
			}
#  ifdef TRACE
			if (!(regp->trcoptions & TRCFCB))
#  endif
			if ((fd = open("/dev/null", O_RDWR)) >= 0) {
				if (fd != 2) {
					dup2(fd, 2);
					close(fd);
				}
			}
			if (nfakefiles && !pipe(fds)) {
				if (!(child = fork())) {
					close(fds[1]);          /* Grandchild, only reads from pipe, */
					if (fds[0] != 0) {      /* and only on stdin. */
						dup2(fds[0], 0);
						close(fds[0]);
					}
					strcpy(cmdbuf, fcmd);
					cp = cmdbuf;
					while (*cp)
						cp++;
					while (*name) { /* Convert glob pattern to regexp. */
						if (*name == '*')
							*cp++ = '.';
						else if (*name == '.')
							*cp++ = '\\';
						else if (*name == '?')
							*name = '.';
						*cp++ = *name++;
					}
					*cp++ = 0;
					strncat(cmdbuf, fcmd2, sizeof(cmdbuf));
#  ifdef TRACE
					write(2, cmdbuf, strlen(cmdbuf));
					write(2, "\n", 1);
#  endif
					execl("/bin/sh", "/bin/sh", "-c", cmdbuf, 0);
					_exit(1);
				} else if (child > 0) {
					close(fds[0]);                  /* Parent, only writes to pipe. */
					for (fd = 0; fd < nfakefiles; fd++) {
						if (fakefs[fd].name[0]) /* Not deleted? */
							write(fds[1], dirfile,
							      mysprintf(dirfile, "%s\n", fakefs[fd].name));
					}
					close(fds[1]);                          /* Done sending grandchild our list. */
					while ((pid = wait(0)) != child)        /* Wait for it to */
						;                               /* finish filtering the names and die. */
				}
			}
			/* Go on to do the main file list. */
#  ifdef TRACE
			write(2, cmdbuf, strlen(cmdbuf));
			write(2, "\n", 1);
#  endif
			execl("/bin/sh", "/bin/sh", "-c", cmdbuf, 0);
			_exit(1);
		} else if (child > 0) {                         /* (Grand)parent */
			while ((pid = wait(0)) != child)        /* Wait for offspring to finish. */
				;
			lseek(fd, 0L, 0);                       /* Rewind the file. */
			if (!fstat(fd, &st) && st.st_size) {    /* How big did it end up? */
				if (fbuf = malloc(st.st_size + 2)) {
					read(fd, fbuf, st.st_size);
#  ifdef TRACE
					if (regp->trcoptions & TRCFCB)
						write(2, fbuf, st.st_size);
#  endif
					fbuf[st.st_size] = 0;
					fbuf[st.st_size + 1] = 0;
					name = fbuf;
					while (*name) {
						cp = name;
						while (*cp && *cp != '\n')
							cp++;
						*cp = 0;
						cmdbuf[0] = 'A' + drive;
						cmdbuf[1] = ':';
						strncpy(&cmdbuf[2], name, sizeof cmdbuf - 2);
						cachefile(cmdbuf, &dirlisthead);
						name = cp + 1;
					}
					free(fbuf);
				}
			}
		}
		close(fd);
	}
	return dirlisthead;
}

#endif

/*
** Convert a filename from CP/M's FCB format to something Unix likes better.
** Format will be A:unix_basename which needs further conversion for host OS.
*/

char *
fncanon(fcbp, offset, curdrive)
struct fcb *fcbp;
int offset;
int curdrive;
{
	static char name[15];   /* A:8.3<NULL> */
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

	if (!fcbp->drive)       /* Put on drive letter. */
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

char *
cfntoufn(name)
char *name;
{
	int drive;
	static char path[512];  /* For compositing path+name. */

	drive = name[0] - 'A';
	if (!drives[drive])     /* No path specified? */
		if (!drive)     /* A:?  Assume cwd. */
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

char *
ambigname(name)
char *name;
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
		if (!strcmp(&newname[2], "*.*"))        /* All files? */
			newname[3] = 0;                 /* Make into Unix's '*' then. */
		return newname;
	} else
		return name;
}

/*
** Many CP/M programs (i.e. WordStar) are extremely sloppy about not closing
** files, and even re-opening them again and again.  Make sure we don't do
** this, because we can't afford to leak file descriptors under Unix.
*/

struct fcache *
cachedfile(name, lheadp)        /* Is the file already in the given cache? */
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

struct fcache *
cachefile(name, lheadp)         /* Make sure the file's in the given cache. */
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

void
uncachefile(name, lheadp)       /* Remove a file from the given cache. */
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
