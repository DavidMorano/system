/* logzones SUPPORT */
/* lang=C++20 */

/* object to manipulate a LOGZONES file */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine maintains a LOGZONES file. This file is
	used to maintain the names and offsets for time-zone names.

	Format of file records:

	- count of usage
	- the time zone name
	- the offset from GMT (in RFC822 format for readability)

	Format example:

0123456789012345678901234567890
       3      EST +0400
       3      EDT +0500
0123456789012345678901234567890

	8 + 1 + 8 + 1 + 5 + 1 = 24


	Philosophy of implementation:

	We are not indexing the file (sigh).  Actually, many of
	these little shared database files have used indexing in
	the past and it wasn't worth the effort!  The time-zone
	names and possible offsets (a name-offset pair) serve as
	the key to the database.  These (pairs) are finite in the
	world.  Ya, maybe there are way too many more than you would
	like but the number isn't worth jumping through hoops to
	make this little thing as fast as possible.  So, we just
	search the whole database file linearly when we have to
	find a name-offset match.  We are also not caching any
	entries either.  So we just search it again whenever we
	need to.  Now don't get your underwear in a bunch over this.

	Also, we do compaisons of fields in their ASCII string form.
	Other things like this always did comparisons in their
	logical binary forms, but those are actually much slower
	since the ASCII forms needs to be converted to binary form
	for all entries examined.  By doing the comparison in string
	form, only the search key entry needs to be converted to
	string for for a search!

	Note also that the 'update()' method takes a string argument
	called the 'stamp'.  This is meant to be a time stamp in
	regular LOGZ format.  However, this format is NOT enforced
	at all and isn't even examined.  It is blindly written into
	the 'stamp' field of an entry (a field trailing all other
	fields).  Only the length of the stamp passed is constrained.
	Due to possible abuse, this field may be of limited value
	if examining it from other writters.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"logzones.h"


/* local defines */

#define	LZ		logzones
#define	LZ_ENT		logzones_ent
#define	LZ_CUR		logzones_cur

#define	LZ_ENTLEN	LOGZONES_ENTLEN	

/* entry field offsets */
#define	EFO_COUNT	0
#define	EFO_NAME	9
#define	EFO_OFFSET	18
#define	EFO_STAMP	24
#define	EFO_OVERLAST	48

/* entry field lengths */
#define	EFL_COUNT	8
#define	EFL_NAME	8
#define	EFL_OFFSET	5
#define	EFL_STAMP	23

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		10		/* seconds */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern int	sfshrink(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	ctdecui(char *,int,uint) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	lockfile(int,int,off_t,off_t,int) ;
extern int	isfsremote(int) ;
extern int	isdigitlatin(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpyuc(char *,cchar *,int) ;
extern char	*strnwcpy(char *,int,cchar *,int) ;


/* external variables */


/* local structures */


/* forward references */

static int	logzones_fileopen(LZ *,time_t) noex ;
static int	logzones_fileclose(LZ *) noex ;
static int	logzones_lockget(LZ *,time_t,int) noex ;
static int	logzones_lockrelease(LZ *) noex ;
static int	logzones_search(LZ *,char *,int,int,char **) noex ;
static int	logzones_enteropen(LZ *,time_t) noex ;

static int entry_start(LZ_ENT *,cchar *,int,int,cchar *) noex ;
static int entry_startbuf(LZ_ENT *,cchar *,int) noex ;
static int entry_update(LZ_ENT *,cchar *) noex ;
static int entry_write(LZ_ENT *,char *,int) noex ;
static int entry_finish(LZ_ENT *) noex ;

static bool	haveoffset(int) noex ;
static bool	fieldmatch(cchar *,cchar *,int,int) noex ;


/* local variables */

static cchar	blanks[] = "                       " ;


/* exported variables */


/* exported subroutines */

int logzones_open(LZ *op,cchar *fname,int oflags,mode_t operms) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	op->magic = 0 ;
	op->mtime = 0 ;
	op->fd = -1 ;
	op->fname = NULL ;
	memset(&op->f,0,sizeof(struct logzones_flags)) ;

#if	CF_CREAT
	oflags |= O_CREAT ;
#endif

	if ((rs = u_open(fname,oflags,operms)) >= 0) {
	    cchar	*cp ;
	    op->fd = rs ;
	    if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        struct ustat	sb ;
	        const time_t	dt = time(NULL) ;
	        op->fname = cp ;
	        op->oflags = (oflags &= (~ O_TRUNC)) ;
	        op->operms = operms ;
	        if ((rs = uc_closeonexec(op->fd,TRUE)) >= 0) {
	            cint	am = (oflags & O_ACCMODE) ;
	            op->f.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	            op->opentime = dt ;
	            op->accesstime = dt ;
	            if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	                cint	n = LOGZONES_NENTS ;
	                cint	entlen = LZ_ENTLEN ;
	                op->mtime = sb.st_mtime ;
	                op->filesize = sb.st_size ;
	                op->pagesize = getpagesize() ;
	                if ((rs = isfsremote(op->fd)) >= 0) {
	                    cint	size = (n * entlen) ;
	                    char	*bp ;
	                    op->f.remote = (rs > 0) ;
	                    if ((rs = uc_malloc(size,&bp)) >= 0) {
	                        op->bufsize = size ;
	                        op->buf = bp ;
	                        op->magic = LOGZONES_MAGIC ;
	                    }
	                } /* end if (isfsremote) */
	            } /* end if (stat) */
	        } /* end if (uc_closeonexec) */
	        if (rs < 0) {
	            uc_free(op->fname) ;
	            op->fname = NULL ;
	        }
	    } /* end if (m-a) */
	    if (rs < 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	} /* end if (open) */

	return rs ;
}
/* end subroutine (logzones_open) */

int logzones_close(LZ *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	if (op->buf != NULL) {
	    rs1 = uc_free(op->buf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->buf = NULL ;
	}

	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}

	if (op->fname != NULL) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (logzones_close) */

/* initialize a cursor */
int logzones_curbegin(LZ *op,LZ_CUR *curp) noex {
	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	op->f.cursor = TRUE ;
	op->f.cursorlockbroken = FALSE ;
	op->f.cursoracc = FALSE ;
	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (logzones_curbegin) */

int logzones_curend(LZ *op,LZ_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	if (op->f.cursoracc) {
	    time_t	dt = time(NULL) ;
	    op->accesstime = dt ;
	}

	op->f.cursor = FALSE ;

	if (op->f.lockedread || op->f.lockedwrite) {
	    rs1 = logzones_lockrelease(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (logzones_curend) */

int logzones_enum(LZ *op,LZ_CUR *curp,LZ_ENT *ep) noex {
	time_t		dt = 0 ;
	int		rs = SR_OK ;
	int		ei = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	if (! op->f.cursor) return SR_INVALID ;

/* has our lock been broken */

	if (! op->f.cursorlockbroken) {
	    if (op->fd < 0) {
	        if (dt == 0) dt = time(NULL) ;
	        rs = logzones_fileopen(op,dt) ;
	    }
	    if (rs >= 0) {
	        if ((! op->f.lockedread) && (! op->f.lockedwrite)) {
	            if (dt == 0) dt = time(NULL) ;
	            rs = logzones_lockget(op,dt,1) ;
	        }
	        if (rs >= 0) {
	            int		eoff ;
	            cint	ebl = LZ_ENTLEN ;
	            char	ebp[LZ_ENTLEN + 1] ;
	            ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	            eoff = (ei * ebl) ;
	            if ((eoff + ebl) <= op->filesize) {
	                if (ep != NULL) {
	                    if ((rs = u_pread(op->fd,ebp,ebl,eoff)) >= 0) {
	                        rs = entry_startbuf(ep,(op->buf + eoff),ebl) ;
	                    }
	                }
	                if (rs >= 0) {
	                    curp->i = ei ;
	                    op->f.cursoracc = TRUE ;
	                }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if (ok) */
	    } /* end if (ok) */
	} else {
	    rs = SR_LOCKLOST ;
	}

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_enum) */

int logzones_match(LZ *op,cchar *znb,int znl,int off,LZ_ENT *ep) noex {
	LZ_ENT	e ;
	const time_t	dt = time(NULL) ;
	cint	ebl = LZ_ENTLEN ;
	int		rs ;
	int		ei ;
	char		ebp[LZ_ENTLEN + 1] ;
	char		*bp ;

	if (op == NULL) return SR_FAULT ;
	if (znb == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

/* is the file open */

	if ((rs = logzones_enteropen(op,dt)) >= 0) {

/* capture the lock if we do not already have it */

	    if ((! op->f.lockedread) && (! op->f.lockedwrite)) {
	        rs = logzones_lockget(op,dt,1) ;
	    }

/* we do comparisons in "string" representation form (much faster !) */

	    if (rs >= 0) {

	        if ((rs = entry_start(&e,znb,znl,off,NULL)) >= 0) {

/* formulate the search string in the buffer */

	            entry_write(&e,ebp,ebl) ;

	            if ((rs = logzones_search(op,ebp,ebl,off,&bp)) >= 0) {
	                ei = rs ;
	                entry_startbuf(ep,bp,ebl) ;
	            }

	            entry_finish(&e) ;
	        } /* end if (entry) */

/* optionally release our lock if we didn't have a cursor outstanding */

	        if (rs >= 0) {

	            if (! op->f.cursor) rs = logzones_lockrelease(op) ;

/* update access time as appropriate */

	            if (! op->f.cursor) {
	                op->accesstime = dt ;
	            } else {
	                op->f.cursoracc = TRUE ;
	            }

	        } /* end if (ok) */

	    } /* end if (ok) */

	} /* end if (enter-open) */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_match) */

int logzones_update(LZ *op,cchar *znb,int znl,int off,cchar *st) noex {
	LZ_ENT		e ;
	const time_t	dt = time(NULL) ;
	cint		ebl = LZ_ENTLEN ;
	int		rs ;
	int		ei = 0 ;
	char		ebp[LZ_ENTLEN + 1] ;
	char		*bp ;

	if (op == NULL) return SR_FAULT ;
	if (znb == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	if (! op->f.writable) return SR_RDONLY ;

/* is the file open */

	if ((rs = logzones_enteropen(op,dt)) >= 0) {

/* capture the lock if we do not already have it */
/* lose any READ lock (we need a WRITE lock) */

	    if (op->f.lockedread) {
	        if (op->f.cursor) op->f.cursorlockbroken = TRUE ;

#ifdef	OPTIONAL
	        logzones_lockrelease(op) ;
#else
	        op->f.lockedread = FALSE ;
#endif /* OPTIONAL */

	    }

/* so that we can get a WRITE lock */

	    if (rs >= 0) {

	        if (! op->f.lockedwrite) {
	            rs = logzones_lockget(op,dt,0) ;
	        }

/* we do comparisons in "string" representation form (much faster !) */

	        if (rs >= 0) {
	            if ((rs = entry_start(&e,znb,znl,off,NULL)) >= 0) {

/* formulate the search string in the buffer */

	                entry_write(&e,ebp,ebl) ;

	                rs = logzones_search(op,ebp,ebl,off,&bp) ;
	                ei = rs ;
	                entry_finish(&e) ;
	            } /* end if (entry) */

/* update the entry that we found and write it back */

	            if (rs >= 0) {
	                uint	eoff = (ei * ebl) ;

/* found existing entry (update in memory) */

	                if ((rs = entry_startbuf(&e,bp,ebl)) >= 0) {

	                    entry_update(&e,st) ;

	                    entry_write(&e,ebp,ebl) ;

/* write into the actual memory window */

	                    rs = u_pwrite(op->fd,ebp,ebl,eoff) ;

/* ok, we're done */

	                    entry_finish(&e) ;
	                } /* end if (entry) */
	            } else if (rs == SR_NOTFOUND) {

/* need a new entry (write it to the file) */

	                ei = op->filesize / ebl ;

	                if ((rs = entry_start(&e,znb,znl,off,NULL)) >= 0) {

	                    entry_update(&e,st) ;

	                    entry_write(&e,ebp,ebl) ;

	                    rs = u_pwrite(op->fd,ebp,ebl,op->filesize) ;

	                    if ((rs >= 0) && (rs == ebl)) {
	                        op->filesize += ebl ;
	                    }

	                    entry_finish(&e) ;
	                } /* end if (entry) */
	            } /* end if (entry update) */

	            if ((rs >= 0) && op->f.remote) {
	                uc_fsync(op->fd) ;
	            }

/* optionally release our lock if we didn't have a cursor outstanding */

	            if (! op->f.cursor) {
	                logzones_lockrelease(op) ;
	            }

/* update access time as appropriate */

	            if (! op->f.cursor) {
	                op->accesstime = dt ;
	            } else {
	                op->f.cursoracc = TRUE ;
	            }
	        } /* end if (ok) */
	    } /* end if (ok) */
	} /* end if (enter-open) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_update) */

int logzones_check(LZ *op,time_t dt) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGZONES_MAGIC) return SR_NOTOPEN ;

	if (op->fd >= 0) {
	    int		f = FALSE ;
	    if (dt == 0) dt = time(NULL) ;
	    f = f || ((dt - op->accesstime) > TO_ACCESS) ;
	    f = f || ((dt - op->opentime) > TO_OPEN) ;
	    if (f) {
	        rs = logzones_fileclose(op) ;
	    }
	} /* end if (already open) */

	return rs ;
}
/* end subroutine (logzones_check) */


/* private subroutines */

static int logzones_lockget(LZ *op,time_t dt,int f_read) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (op->fd < 0) {
	    rs = logzones_fileopen(op,dt) ;
	}

	if (rs >= 0) {
	    off_t	fs = op->filesize ;
	    cint	to = TO_LOCK ;
	    int		cmd ;

	    if (f_read || (! op->f.writable)) {
	        op->f.lockedread = TRUE ;
	        cmd = F_RLOCK ;
	    } else {
	        op->f.lockedwrite = TRUE ;
	        cmd = F_WLOCK ;
	    }
	    if ((rs = lockfile(op->fd,cmd,0L,fs,to)) >= 0) {
	        struct ustat	sb ;
	        if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	            f = f || (sb.st_size != op->filesize) ;
	            f = f || (sb.st_mtime != op->mtime) ;
	            if (f) {
	                op->filesize = sb.st_size ;
	                op->mtime = sb.st_mtime ;
	            }
	        } else {
	            op->f.lockedread = FALSE ;
	            op->f.lockedwrite = FALSE ;
	        }
	    } /* end if (lockfile) */

	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (logzones_lockget) */

static int logzones_lockrelease(LZ *op) noex {
	int		rs = SR_OK ;
	if (op->f.lockedread || op->f.lockedwrite) {
	    if (op->fd >= 0) {
	        cint	size = op->filesize ;
	        rs = lockfile(op->fd,F_ULOCK,0L,size,TO_LOCK) ;
	    }
	    op->f.lockedread = FALSE ;
	    op->f.lockedwrite = FALSE ;
	}
	return rs ;
}
/* end subroutine (logzones_lockrelease) */


static int logzones_search(LZ *op,char *ebp,int ebl,int soff,char **rpp) noex {
	int		rs ;

	if ((rs = u_seek(op->fd,0L,SEEK_SET)) >= 0) {
	    uint	off = 0 ;
	    uint	eoff = 0 ;
	    cint	efo = EFO_OFFSET ;
	    cint	efl = EFL_OFFSET ;
	    int		ne = 0 ;
	    int		ei = 0 ;
	    int		i = 1 ;
	    char	*bp = nullptr ;
	    while ((rs = u_read(op->fd,op->buf,op->bufsize)) > 0) {
	        cint	len = rs ;
	        ne = (len / ebl) ;
	        for (i = 0 ; i < ne ; i += 1) {

	            eoff = i * ebl ;
	            bp = op->buf + eoff ;
	            if (fieldmatch(bp,ebp,EFO_NAME,EFL_NAME) &&
	                fieldmatch(bp,ebp,(efo+ 1),(efl- 1))) {

	                if ((! haveoffset(soff)) || (soff == 0))
	                    break ;

	                if ((soff > 0) && (bp[efo] == '-'))
	                    break ;

	                if ((soff < 0) && (bp[efo] != '-'))
	                    break ;

	            } /* end if */

	            ei += 1 ;

	        } /* end for (looping through entries) */

	        if (ei < ne) break ;

	        eoff = i * ebl ;
	        bp = op->buf + eoff ;

	        off += len ;

	    } /* end while */
	    if (rs >= 0) {
	        rs = (i < ne) ? ei : SR_NOTFOUND ;
	    }
	    if (rpp) *rpp = (rs >= 0) ? bp : nullptr ;
	} /* end if (seek) */

	return rs ;
}
/* end subroutine (logzones_search) */

static int logzones_fileopen(LZ *op,time_t dt) noex {
	int		rs ;
	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,op->operms)) >= 0) {
	        op->fd = rs ;
	        rs = uc_closeonexec(op->fd,TRUE) ;
	        op->opentime = dt ;
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    }
	} else {
	    rs = op->fd ;
	}
	return rs ;
}
/* end subroutine (logzones_fileopen) */

static int logzones_fileclose(LZ *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (logzones_fileclose) */

static int logzones_enteropen(LZ *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if (dt == 0) dt = time(NULL) ;
	    dt = time(NULL) ;
	    rs = logzones_fileopen(op,dt) ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (logzones_enteropen) */

static int entry_start(LZ_ENT *ep,cc *znb,int znl,int soff,cc *st) noex {
	int		rs = SR_FAULT ;
	cchar		*cp ;
	if (ep && znb) {
	    rs = SR_OK ;
	    ep->count = 0 ;
	    ep->znl = strnlen(znb,MIN(znl,LOGZONES_ZNAMESIZE)) ;
	    ep->off = soff ;
	    strnwcpy(ep->znb,LOGZONES_ZNAMESIZE,znb,znl) ;
	    cp = (st != NULL) ? st : blanks ;
	    strncpy(ep->st,cp,LOGZONES_STAMPSIZE) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_startbuf(LZ_ENT *ep,cchar *ebuf,int elen) noex {
	uint		uv ;
	int		rs ;
	int		cl ;
	cchar		*bp ;

	if (ep == NULL) return SR_FAULT ;

	if (elen < 0) elen = INT_MAX ;

	if (elen < (EFO_OVERLAST - 1)) return SR_INVALID ;

	bp = ebuf ;
	if ((rs = cfdecui(bp,EFL_COUNT,&uv)) >= 0) {
	    int		i ;
	    int		hours, mins, sign ;
	    cchar	*cp ;

	    ep->count = uv ;

	    bp += (EFL_COUNT + 1) ;
	    cl = sfshrink(bp,LOGZONES_ZNAMESIZE,&cp) ;

	    ep->znl = cl ;
	    strnwcpy(ep->znb,LOGZONES_ZNAMESIZE,cp,cl) ;

	    bp += (EFL_NAME + 1) ;
	    sign = (*bp++ == '-') ? 1 : -1 ; /* reverse of what might think! */

/* do we have an offset? */

	    for (i = 0 ; i < 4 ; i += 1) {
	        cint	ch = MKCHAR(bp[i]) ;
	        if (! isdigitlatin(ch)) break ;
	    } /* end for */

	    if (i >= 4) {
	        hours = (((int) *bp++) - '0') * 10 ;
	        hours += (((int) *bp++) - '0') ;
	        mins = (((int) *bp++) - '0') * 10 ;
	        mins += (((int) *bp++) - '0') ;
	        ep->off = sign * ((hours * 60) + mins) ;
	    } else {
	        ep->off = LOGZONES_NOZONEOFFSET ;
	    }

/* grab the stamp field */

	    strncpy(ep->st,bp,LOGZONES_STAMPSIZE) ;
	    rs = LZ_ENTLEN ;

	} /* end if (cfdecui) */

	return rs ;
}
/* end subroutine (entry_startbuf) */

static int entry_update(LZ_ENT *ep,cchar *st) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    cchar	*cp ;
	    rs = SR_OK ;
	    ep->count += 1 ;
	    cp = (st) ? st : blanks ;
	    strncpy(ep->st,cp,LOGZONES_STAMPSIZE) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_update) */

static int entry_write(LZ_ENT *ep,char *ebuf,int elen) noex {
	int		v ;
	int		hours, mins ;
	int		zl, cl, rl ;
	char		numbuf1[EFL_COUNT + 1] ;
	char		*bp = ebuf ;
	char		*cp ;

	if (ep == NULL) return SR_FAULT ;

	if (elen < 23) return SR_OVERFLOW ;

/* the count field */

	cp = numbuf1 ;
	cl = ctdecui(numbuf1,EFL_COUNT,ep->count) ;

	if (cl > EFL_COUNT) {
	    cp += (cl - EFL_COUNT) ;
	    cl -= (cl - EFL_COUNT) ;
	}

	rl = (EFL_COUNT - cl) ;
	strncpy(bp,blanks,rl) ;

	strncpy((bp + rl),cp,cl) ;

	bp += EFL_COUNT ;
	*bp++ = ' ' ;

/* write out the zone-name */

#ifdef	COMMENT
	zl = ep->znl ;
#else
	zl = strnlen(ep->znb,EFL_NAME) ;
#endif /* COMMENT */

	rl = (EFL_NAME - zl) ;
	strncpy(bp,blanks,rl) ;

	strwcpyuc((bp + rl),ep->znb,zl) ;

	bp += EFL_NAME ;
	*bp++ = ' ' ;

/* write out the zone offset (field len=EFL_OFFSET) */

	if (haveoffset(ep->off)) {

	    *bp++ = (ep->off >= 0) ? '-' : '+' ;
	    v = abs(ep->off) ;

	    hours = v / 60 ;
	    mins = v % 60 ;

	    *bp++ = ((hours / 10) + '0') ;
	    *bp++ = ((hours % 10) + '0') ;
	    *bp++ = ((mins / 10) + '0') ;
	    *bp++ = ((mins % 10) + '0') ;

	} else {
	    bp = strwcpy(bp,blanks,5) ;
	}

	*bp++ = ' ' ;

/* the stamp field */

	cp = strwcpy(bp,ep->st,EFL_STAMP) ;

	cl = cp - bp ;
	rl = EFL_STAMP - cl ;
	if (rl > 0)
	    strwcpy(cp,blanks,rl) ;

	bp += EFL_STAMP ;

/* end it off */

	*bp++ = '\n' ;

	return (bp - ebuf) ;
}
/* end subroutine (entry_write) */

static int entry_finish(LZ_ENT *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static bool haveoffset(int off) noex {
	return (abs(off) <= (12 * 60)) ;
}
/* end subroutine (haveoffset) */

/* return TRUE if a field matched up */
static bool fieldmatch(cchar *ebp,cchar *mbp,int fo,int fl) noex {
	return (strncmp((ebp + fo),(mbp + fo),fl) == 0) ;
}
/* end subroutine (fieldmatch) */


