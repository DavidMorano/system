/* logzones SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a LOGZONES file */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	logzones

	Description:
	This subroutine maintains a LOGZONES file.  This file is
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
	need to.  Now do not get your underwear in a bunch over this.

	Also, we do compaisons of fields in their ASCII string form.
	Other things like this always did comparisons in their
	logical binary forms, but those are actually much slower
	since the ASCII forms needs to be converted to binary form
	for all entries examined.  By doing the comparison in string
	form, only the search key entry needs to be converted to
	string for for a search!

	Note also that the |update()| method takes a string argument
	called the 'stamp'.  This is meant to be a time stamp in
	regular LOGZ format.  However, this format is NOT enforced
	at all and isn't even examined.  It is blindly written into
	the 'stamp' field of an entry (a field trailing all other
	fields).  Only the length of the stamp passed is constrained.
	Due to possible abuse, this field may be of limited value
	if examining it from other writters.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<strw.h>		/* |strwblanks(3uc)| */
#include	<strn.h>		/* |strnblanks(3uc)| */
#include	<lockfile.h>
#include	<cfdec.h>
#include	<ctdec.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<isfiledesc.h>		/* |isfsremote(3uc)| */
#include	<ischarx.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"logzones.h"

import libutil ;

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

#ifndef	CF_CREAT
#define	CF_CREAT	0		/* always create the file? */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int logzones_magic(logzones *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LOGZONES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (logzones_magic) */

static int	logzones_opener(LZ *,cc *,int,mode_t) noex ;
static int	logzones_fileopen(LZ *,time_t) noex ;
static int	logzones_fileclose(LZ *) noex ;
static int	logzones_lockacq(LZ *,time_t,int) noex ;
static int	logzones_lockrel(LZ *) noex ;
static int	logzones_updater(LZ *,cc *,int,int,cc *) noex ;
static int	logzones_search(LZ *,char *,int,int,char **) noex ;
static int	logzones_enteropen(LZ *,time_t) noex ;

static int entry_start(LZ_ENT *,cchar *,int,int,cchar *) noex ;
static int entry_startbuf(LZ_ENT *,cchar *,int) noex ;
static int entry_update(LZ_ENT *,cchar *) noex ;
static int entry_write(LZ_ENT *,char *,int) noex ;
static int entry_finish(LZ_ENT *) noex ;

static bool	hasoffset(int) noex ;
static bool	fieldmatch(cchar *,cchar *,int,int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

constexpr bool		f_creat = CF_CREAT ;


/* exported variables */


/* exported subroutines */

int logzones_open(LZ *op,cchar *fname,int of,mode_t om) noex {
    	LOGZONES	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && fname) {
	    rs = SR_INVALID ;
	    memclear(hop) ;
	    if (fname[0]) {
	        op->fd = -1 ;
	        if_constexpr (f_creat) {
	            of |= O_CREAT ;
	        } /* end if_constexpr (f_creat) */
		if ((rs = pagesize) >= 0) {
		    op->pagesize = rs ;
		    rs = logzones_opener(op,fname,of,om) ;
		} /* end if (pagesize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (logzones_open) */

int logzones_close(LZ *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = logzones_magic(op)) >= 0) {
	    if (op->buf) {
	        rs1 = uc_free(op->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->buf = nullptr ;
	    }
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logzones_close) */

/* initialize a cursor */
int logzones_curbegin(LZ *op,LZ_CUR *curp) noex {
	int		rs ;
	if ((rs = logzones_magic(op,curp)) >= 0) {
	    op->fl.cursor = true ;
	    op->fl.cursorlockbroken = false ;
	    op->fl.cursoracc = false ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logzones_curbegin) */

int logzones_curend(LZ *op,LZ_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = logzones_magic(op,curp)) >= 0) {
	    if (op->fl.cursoracc) {
	        custime		dt = getustime ;
	        op->accesstime = dt ;
	    }
	    op->fl.cursor = false ;
	    if (op->fl.lockedread || op->fl.lockedwrite) {
	        rs1 = logzones_lockrel(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logzones_curend) */

int logzones_curenum(LZ *op,LZ_CUR *curp,LZ_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = logzones_magic(op,curp)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->fl.cursor) {
	        time_t		dt = 0 ;
		rs = SR_OK ;
	        if (! op->fl.cursorlockbroken) {
	            if (op->fd < 0) {
	                if (dt == 0) dt = getustime ;
	                rs = logzones_fileopen(op,dt) ;
	            }
	            if (rs >= 0) {
	                if ((! op->fl.lockedread) && (! op->fl.lockedwrite)) {
	                    if (dt == 0) dt = getustime ;
	                    rs = logzones_lockacq(op,dt,1) ;
	                }
	                if (rs >= 0) {
	                    int		eoff ;
	                    cint	ebl = LZ_ENTLEN ;
	                    char	ebp[LZ_ENTLEN + 1] ;
	                    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	                    eoff = (ei * ebl) ;
	                    if ((eoff + ebl) <= op->fsize) {
	                        if (ep) {
				    cint	fd = op->fd ;
	                            if ((rs = u_pread(fd,ebp,ebl,eoff)) >= 0) {
					cchar	*ebuf = (op->buf + eoff) ;
	                                rs = entry_startbuf(ep,ebuf,ebl) ;
	                            }
	                        }
	                        if (rs >= 0) {
	                            curp->i = ei ;
	                            op->fl.cursoracc = true ;
	                        }
	                    } else {
	                        rs = SR_NOTFOUND ;
	                    }
	                } /* end if (ok) */
	            } /* end if (ok) */
	        } else {
	            rs = SR_LOCKLOST ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_curenum) */

int logzones_match(LZ *op,cchar *znb,int znl,int off,LZ_ENT *ep) noex {
	int		rs ;
	int		rs1 ;
	int		ei ;
	if ((rs = logzones_magic(op,znb)) >= 0) {
	    custime	dt = getustime ;
	    LZ_ENT	e ;
	    cint	ebl = LZ_ENTLEN ;
	    char	ebp[LZ_ENTLEN + 1] ;
	    char	*bp ;
	    if ((rs = logzones_enteropen(op,dt)) >= 0) {
/* capture the lock if we do not already have it */
	        if ((! op->fl.lockedread) && (! op->fl.lockedwrite)) {
	            rs = logzones_lockacq(op,dt,1) ;
	        }
/* we do comparisons in "string" representation form (much faster !) */
	        if (rs >= 0) {
	            if ((rs = entry_start(&e,znb,znl,off,nullptr)) >= 0) {
/* formulate the search string in the buffer */
	                entry_write(&e,ebp,ebl) ;
	                if ((rs = logzones_search(op,ebp,ebl,off,&bp)) >= 0) {
	                    ei = rs ;
	                    entry_startbuf(ep,bp,ebl) ;
	                }
	                rs1 = entry_finish(&e) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (entry) */
/* optionally release our lock if we didn't have a cursor outstanding */
	            if (rs >= 0) {
	                if (! op->fl.cursor) {
			    rs = logzones_lockrel(op) ;
		        }
/* update access time as appropriate */
	                if (! op->fl.cursor) {
	                    op->accesstime = dt ;
	                } else {
	                    op->fl.cursoracc = true ;
	                }
	            } /* end if (ok) */
	        } /* end if (ok) */
	    } /* end if (enter-open) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_match) */

int logzones_update(LZ *op,cchar *znb,int znl,int off,cchar *st) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = logzones_magic(op,znb)) >= 0) {
	    rs = SR_RDONLY ;
	    if (op->fl.writable) {
		rs = logzones_updater(op,znb,znl,off,st) ;
		ei = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_update) */

int logzones_check(LZ *op,time_t dt) noex {
	int		rs ;
	if ((rs = logzones_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        bool	f = false ;
	        if (dt == 0) dt = getustime ;
	        f = f || ((dt - op->accesstime) > TO_ACCESS) ;
	        f = f || ((dt - op->opentime) > TO_OPEN) ;
	        if (f) {
	            rs = logzones_fileclose(op) ;
	        }
	    } /* end if (already open) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logzones_check) */


/* private subroutines */

static int logzones_opener(LZ *op,cc *fname,int of,mode_t om) noex {
	int		rs ;
	if ((rs = u_open(fname,of,om)) >= 0) {
	    op->fd = rs ;
	    if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        custime		dt = getustime ;
	        op->fname = cp ;
	        op->oflags = (of &= (~ O_TRUNC)) ;
	        op->operms = om ;
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	            cint	am = (of & O_ACCMODE) ;
		    if ((am == O_WRONLY) || (am == O_RDWR)) {
	                op->fl.writable = true ;
		    }
	            op->opentime = dt ;
	            op->accesstime = dt ;
	            if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
			rs = SR_TOOBIG ;
			if (sb.st_size <= INT_MAX) {
	                    cint	ne = LOGZONES_NENTS ;
	                    cint	entlen = LZ_ENTLEN ;
	                    op->timod = sb.st_mtime ;
	                    op->fsize = sb.st_size ;
	                    if ((rs = isfsremote(op->fd)) >= 0) {
	                        cint	sz = (ne * entlen) ;
	                        char	*bp{} ;
	                        op->fl.remote = (rs > 0) ;
	                        if ((rs = uc_malloc(sz,&bp)) >= 0) {
	                            op->bufsize = sz ;
	                            op->buf = bp ;
	                            op->magic = LOGZONES_MAGIC ;
	                        }
	                    } /* end if (isfsremote) */
			} /* end if (filesize) */
	            } /* end if (uc_fstat) */
	        } /* end if (uc_closeonexec) */
	        if (rs < 0) {
	            uc_free(op->fname) ;
	            op->fname = nullptr ;
	        }
	    } /* end if (m-a) */
	    if (rs < 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	} /* end if (open) */
	return rs ;
}
/* end subroutine (logzones_opener) */

static int logzones_lockacq(LZ *op,time_t dt,int f_read) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->fd < 0) {
	    rs = logzones_fileopen(op,dt) ;
	}
	if (rs >= 0) {
	    off_t	fs = op->fsize ;
	    cint	to = TO_LOCK ;
	    int		cmd ;
	    if (f_read || (! op->fl.writable)) {
	        op->fl.lockedread = true ;
	        cmd = F_RLOCK ;
	    } else {
	        op->fl.lockedwrite = true ;
	        cmd = F_WLOCK ;
	    }
	    if ((rs = lockfile(op->fd,cmd,0z,fs,to)) >= 0) {
	        if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	            f = f || (sb.st_size != op->fsize) ;
	            f = f || (sb.st_mtime != op->timod) ;
	            if (f) {
	                op->fsize = sb.st_size ;
	                op->timod = sb.st_mtime ;
	            }
	        } else {
	            op->fl.lockedread = false ;
	            op->fl.lockedwrite = false ;
	        }
	    } /* end if (lockfile) */
	} /* end if (ok) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (logzones_lockacq) */

static int logzones_lockrel(LZ *op) noex {
	int		rs = SR_OK ;
	if (op->fl.lockedread || op->fl.lockedwrite) {
	    if (op->fd >= 0) {
	        cint	fsz = intconv(op->fsize) ;
		cint	cmd = F_ULOCK ;
	        rs = lockfile(op->fd,cmd,0z,fsz,TO_LOCK) ;
	    }
	    op->fl.lockedread = false ;
	    op->fl.lockedwrite = false ;
	}
	return rs ;
}
/* end subroutine (logzones_lockrel) */

static int logzones_updater(LZ *op,cc *znb,int znl,int off,cc *st) noex {
	custime		dt = getustime ;
	int		rs ;
	int		rs1 ;
	int		ei = 0 ;
	if ((rs = logzones_enteropen(op,dt)) >= 0) {
	    /* capture the lock if we do not already have it */
	    /* lose any READ lock (we need a WRITE lock) */
	    if (op->fl.lockedread) {
		if (op->fl.cursor) op->fl.cursorlockbroken = true ;
#ifdef	OPTIONAL
		logzones_lockrel(op) ;
#else
		op->fl.lockedread = false ;
#endif /* OPTIONAL */
	    } /* end if */
/* so that we can get a WRITE lock */
	    if (rs >= 0) {
		if (! op->fl.lockedwrite) {
		    rs = logzones_lockacq(op,dt,0) ;
		}
/* we do comparisons in "string" representation form (much faster !) */
		if (rs >= 0) {
	            cnullptr	np{} ;
	            LZ_ENT	e ;
	            cint	ebl = LZ_ENTLEN ;
	            char	ebp[LZ_ENTLEN + 1] ;
	            char	*bp ;
		    if ((rs = entry_start(&e,znb,znl,off,np)) >= 0) {
/* formulate the search string in the buffer */
	                entry_write(&e,ebp,ebl) ;
	                rs = logzones_search(op,ebp,ebl,off,&bp) ;
	                ei = rs ;
	                rs1 = entry_finish(&e) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (entry) */
/* update the entry that we found and write it back */
	            if (rs >= 0) {
	                uint	eoff = (ei * ebl) ;
/* found existing entry (update in memory) */
	                if ((rs = entry_startbuf(&e,bp,ebl)) >= 0) {
			    {
	                        entry_update(&e,st) ;
	                        entry_write(&e,ebp,ebl) ;
				/* write into the actual memory window */
	                        rs = u_pwrite(op->fd,ebp,ebl,eoff) ;
			    }
			    /* ok, we are done (with just this item) */
	                    rs1 = entry_finish(&e) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (entry) */
	            } else if (rs == SR_NOTFOUND) {
			coff	fsize = op->fsize ;
			cint	fsz = intconv(op->fsize) ;
			/* need a new entry (write it to the file) */
	                ei = (fsz / ebl) ;
	                if ((rs = entry_start(&e,znb,znl,off,np)) >= 0) {
	                    entry_update(&e,st) ;
	                    entry_write(&e,ebp,ebl) ;
	                    if ((rs = u_pwrite(op->fd,ebp,ebl,fsize)) >= 0) {
	                        if (rs == ebl) {
	                            op->fsize += ebl ;
			        }
	                    }
	                    rs1 = entry_finish(&e) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (entry) */
	            } /* end if (entry update) */
	            if ((rs >= 0) && op->fl.remote) {
	                rs = uc_fsync(op->fd) ;
	            }
		    /* optionally release our lock */
	            if ((rs >= 0) && (! op->fl.cursor)) {
	                logzones_lockrel(op) ;
	            }
		    /* update access time as appropriate */
		    if (rs >= 0) {
	                if (! op->fl.cursor) {
	                    op->accesstime = dt ;
	                } else {
	                    op->fl.cursoracc = true ;
	                }
		    } /* end if (ok) */
	        } /* end if (ok) */
	    } /* end if (ok) */
	} /* end if (enter-open) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (logzones_updater) */

static int logzones_search(LZ *op,char *ebp,int ebl,int soff,char **rpp) noex {
	int		rs ;
	if ((rs = u_seek(op->fd,0z,SEEK_SET)) >= 0) {
	    uint	off = 0 ;
	    uint	eoff = 0 ;
	    cint	efo = EFO_OFFSET ;
	    cint	efl = EFL_OFFSET ;
	    int		ne = 0 ;
	    int		ei = 0 ;
	    int		i = 1 ;		/* why this return value? */
	    char	*bp = nullptr ;
	    while ((rs = u_read(op->fd,op->buf,op->bufsize)) > 0) {
	        cint	len = rs ;
	        ne = (len / ebl) ;
	        for (i = 0 ; i < ne ; i += 1) {
		    bool	f = true ;
	            eoff = i * ebl ;
	            bp = op->buf + eoff ;
	            f = f && fieldmatch(bp,ebp,EFO_NAME,EFL_NAME) ;
		    f = f && fieldmatch(bp,ebp,(efo+ 1),(efl- 1)) ;
		    if (f) {
	                if ((! hasoffset(soff)) || (soff == 0)) break ;
	                if ((soff > 0) && (bp[efo] == '-')) break ;
	                if ((soff < 0) && (bp[efo] != '-')) break ;
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
	    if (rpp) {
		*rpp = (rs >= 0) ? bp : nullptr ;
	    }
	} /* end if (seek) */
	return rs ;
}
/* end subroutine (logzones_search) */

static int logzones_fileopen(LZ *op,time_t dt) noex {
	int		rs ;
	if (op->fd < 0) {
	    cint	of = op->oflags ;
	    cmode	om = op->operms ;
	    if ((rs = u_open(op->fname,of,om)) >= 0) {
	        op->fd = rs ;
	        rs = uc_closeonexec(op->fd,true) ;
	        op->opentime = dt ;
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    } /* end if (u_open) */
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
	    if (dt == 0) dt = getustime ;
	    rs = logzones_fileopen(op,dt) ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (logzones_enteropen) */

static int entry_start(LZ_ENT *ep,cc *znb,int znl,int soff,cc *st) noex {
	int		rs = SR_FAULT ;
	if (ep && znb) {
	    cint	ml = min(znl,LOGZONES_ZNAMESIZE) ;
	    rs = SR_OK ;
	    ep->count = 0 ;
	    ep->znl = short(lenstr(znb,ml)) ;
	    ep->off = short(soff) ;
	    strnwcpy(ep->znb,LOGZONES_ZNAMESIZE,znb,znl) ;
	    if (st) {
	        strncpy(ep->st,st,LOGZONES_STAMPSIZE) ;
	    } else {
	        strnblanks(ep->st,LOGZONES_STAMPSIZE) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_startbuf(LZ_ENT *ep,cchar *ebuf,int elen) noex {
	int		rs = SR_FAULT ;
	if (ep && ebuf) {
	    rs = SR_INVALID ;
	    if (elen < 0) elen = INT_MAX ;
	    if (elen >= (EFO_OVERLAST - 1)) {
	        if (uint uv ; (rs = cfdecui(ebuf,EFL_COUNT,&uv)) >= 0) {
		    int		sign ;
	            int		cl ;
	            cchar	*bp = ebuf ;
	            cchar	*cp ;
	            ep->count = uv ;
	            bp += (EFL_COUNT + 1) ;
	            cl = sfshrink(bp,LOGZONES_ZNAMESIZE,&cp) ;
	            ep->znl = short(cl) ;
	            strnwcpy(ep->znb,LOGZONES_ZNAMESIZE,cp,cl) ;
	            bp += (EFL_NAME + 1) ;
	            sign = (*bp++ == '-') ? 1 : -1 ; /* reverse of thinking */
		    /* do we have an offset? */
	            {
	                int	hours, mins ;
		        int	i = 0 ; /* used afterwards */
	                for (i = 0 ; i < 4 ; i += 1) {
	                    cint	ch = mkchar(bp[i]) ;
	                    if (! isdigitlatin(ch)) break ;
	                } /* end for */
	                if (i >= 4) {
	                    hours = ((mkchar(*bp++) - '0') * 10) ;
	                    hours += (mkchar(*bp++) - '0') ;
	                    mins = ((mkchar(*bp++) - '0') * 10) ;
	                    mins += (mkchar(*bp++) - '0') ;
	                    ep->off = short(sign * ((hours * 60) + mins)) ;
	                } else {
	                    ep->off = LOGZONES_NOZONEOFFSET ;
	                }
	            } /* end block */
		    /* grab the stamp field */
	            strncpy(ep->st,bp,LOGZONES_STAMPSIZE) ;
	            rs = LZ_ENTLEN ;
	        } /* end if (cfdecui) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_startbuf) */

static int entry_update(LZ_ENT *ep,cchar *st) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = SR_OK ;
	    ep->count += 1 ;
	    if (st) {
	        strncpy(ep->st,st,LOGZONES_STAMPSIZE) ;
	    } else {
	        strnblanks(ep->st,LOGZONES_STAMPSIZE) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_update) */

static int entry_write(LZ_ENT *ep,char *ebuf,int elen) noex {
	int		rs = SR_FAULT ;
	if (ep && ebuf) {
	    cint	clen = EFL_COUNT ;
	    char	cbuf[EFL_COUNT + 1] ;
	    rs = SR_OVERFLOW ;
	    if (elen >= 23) {
	        int	zl ;
		int	rl ;
	        int	cl = ctdecui(cbuf,clen,ep->count) ;
	        char	*bp = ebuf ;
	        char	*cp = cbuf ;
		rs = SR_OK ;
		/* the count field */
	        if (cl > clen) {
	            cp += (cl - clen) ;
	            cl -= (cl - clen) ;
	        }
		{
	            rl = (clen - cl) ;
	            strnblanks(bp,rl) ;
	            strncpy((bp + rl),cp,cl) ;
		}
	        bp += clen ;
	        *bp++ = ' ' ;
		/* write out the zone-name */
		{
#ifdef	COMMENT
	            zl = ep->znl ;
#else
	            zl = lenstr(ep->znb,EFL_NAME) ;
#endif /* COMMENT */
	            rl = (EFL_NAME - zl) ;
	            strnblanks(bp,rl) ;
	            strwcpyuc((bp + rl),ep->znb,zl) ;
	            bp += EFL_NAME ;
		}
	        *bp++ = ' ' ;
/* write out the zone offset (field len=EFL_OFFSET) */
	        if (hasoffset(ep->off)) {
	            cint	v = abs(ep->off) ;
	            int		hours ;
	            int		mins ;
	            *bp++ = (ep->off >= 0) ? '-' : '+' ;
	            hours = (v / 60) ;
	            mins = (v % 60) ;
	            *bp++ = char((hours / 10) + '0') ;
	            *bp++ = char((hours % 10) + '0') ;
	            *bp++ = char((mins / 10) + '0') ;
	            *bp++ = char((mins % 10) + '0') ;
	        } else {
	            bp = strwblanks(bp,5) ;
	        }
	        *bp++ = ' ' ;
		/* the stamp field */
	        cp = strwcpy(bp,ep->st,EFL_STAMP) ;
	        cl = intconv(cp - bp) ;
	        if ((rl = EFL_STAMP - cl) > 0) {
	            strwblanks(cp,rl) ;
	        }
	        bp += EFL_STAMP ;
		/* end it off */
	        *bp++ = '\n' ;
	        rs = intconv(bp - ebuf) ;
	    } /* end if (overflow) */
	} /* end if (non-null) */
	return rs ;
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

static bool hasoffset(int off) noex {
	return (abs(off) <= (12 * 60)) ;
}
/* end subroutine (hasoffset) */

/* return true if a field matched up */
static bool fieldmatch(cchar *ebp,cchar *mbp,int fo,int fl) noex {
	return (strncmp((ebp + fo),(mbp + fo),fl) == 0) ;
}
/* end subroutine (fieldmatch) */


