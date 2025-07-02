/* msfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a MSFILE file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUGTEST	0		/* special debugging for test */
#define	CF_SAFE		1		/* safer? */
#define	CF_CREAT	0		/* always create the file? */
#define	CF_LOCKF	0		/* use 'lockf(3c)' */
#define	CF_SOLARISBUG	1		/* work around Solaris® MMAP bug */
#define	CF_NIENUM	0		/* perform NI updates on ENUM */
#define	CF_NISEARCH	0		/* perform NI updates on SEARCH */

/* revision history:

	= 1999-06-01, David A­D­ Morano
	This code was originally written.

	= 2003-06-26, David A­D­ Morano
	Although this object works, it was only a miracle that it
	did.  There is a feature-bug in Solaris® that does not allow
	a file to be both mapped and locked at the same time (in
	either order).  But there seems to be a crack in the stupid
	Solaris® implementation because it does not enforce its stupid
	bug carefully enough and this object here fell through the
	cracks and continued working by accident.  We were locking
	the whole file beyond its end and that appears to get by
	the Solaris® police-state bug-patrol and was accidentally
	being allowed.  I reworked a good bit of this code to eliminate
	any file mapping (so that we can continue to use file-record
	locks).  This whole Solaris® crap (this is being done on
	Solaris® 8 right now) is really a pain and Sun should face
	punitive charges for inhumanity to the programmer community.
	Solaris® has some nice things since it was derived from the
	older (and better) System V UNIX®, but has really messed
	it up by not allowing what used to be allowed in the old
	days with things like the old RFS facility.  Oh, while we are
	on the subject: NFS sucks cock meat!

*/

/* Copyright © 1999,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	msfile

	Description:
	This subroutine maintains a MSFILE file.  This file is used
	to maintain machine status for nodes in the local machine
	cluster.

	Format of file records:

	- (see the 'entry' structure in the header)

	Design note: 

	In summary, Solaris® sucks cock meat!  Solaris® does not allow
	a file to be memory-mapped from an NFS remote server AND
	also be file-locked at the same time.  A lot of stupid Solaris®
	documentation notes say something to the effect that the
	Solaris® VM system cannot handle a remote file that is both
	mapped and subject to file-locking at the same time.  They
	use some sort of stupid circular reasoning that if any file
	is being file-locked, then obviously it cannot be memory-mapped
	since the file locking indicates that file-locking is taking
	place, and that obviously any file that is being file-locked
	cannot therefore also be memory mapped.  That is pretty much
	their reasoning -- I kid you not!

	Unfortunately, code, like this code here, that was first
	designed under System V UNIX® that used file-locking AND
	memory mapping together really needs to be changed to
	eliminate either the file locking or the memory mapping.
	Remote files were cross mounted in the late 80s and very
	early 90s using RFS (not stupid NFS).  The use of RFS provided
	many advantages not the least of them being full UFS
	file-system semantics, but it is not clear why Solaris® took
	a step backward from simply allowing remote files to be
	both memory-mapped and file-locked at the same time.  Some
	bright light-bulb of a software developer must have gotten
	his underwear in a bunch at some point and decided to
	disallow both of these from ever occurring at the same time
	in Solaris®.  We all have suffered from these dumb-butt Solaris®
	developers since we have to take time out to re-debug-write
	old code (like this code here) to handle the case of stupid
	Solaris® not allowing memory mapping for a file that is also
	file-locked.

	Implementation note:

	The code was actually running when files were being locked
	in their entirety and beyond their ends.  There was some
	sort of loop-hole in the stupid Solaris® code that allowed
	a file to be both file-locked and memory mapped at the same
	time under certain circumstances.  However, there seemed to
	be problems with this code when other parties on other
	(remote) systems tried to do the same thing.  They sometimes
	failed with dead-lock types of errors (I forget the details).
	As a result, I decided to change the code to fully comply
	with the stupid Solaris® requirements that no remote file
	be both memory mapped and file locked at the same time.  Any
	code that is here now that has to be with mapping of files
	is really just code that now allocates local private memory.
	This is done instead of using the process heap but was
	really done because it seemed to offer the minimal changes
	to the code to get a private memory access to the file while
	still retaining the ability to file-lock the file.  Finally,
	let me finish with the comment that Solaris® sucks cock meat.

	Final note:

	Solaris® sucks cock meat!  Give me back simultaneous memory
	mapping and file locking.  And while you are at it, give me
	back RFS also!  And to you stupid Solaris® VM developers, get
	out of Solaris® development.  Either get a new job somewhere
	else or think about committing suicide.  Either way, we can
	all be happier with one (or more) of those alternatives.

	Anecdotal note:

	Hey, you cognatively challenged Solaris® developers: give
	me back the ability to push SOCKMOD on a TPI endpoint also!
	Since you are so challenged, I know that you forgot that this
	was possible at one time.  You hosed that ability away when
	you botched up making Solaris® 2.6.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<inttypes.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>			/* |time_t| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<mapstrint.h>
#include	<stdorder.h>
#include	<mkx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<entbuf.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<isfiledesc.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"msfile.h"
#include	"msfilee.h"
#include	"msflag.h"


/* local defines */

#define	MSFILE_IDOFF		0
#define	MSFILE_HEADTABOFF	(MSFILE_IDOFF + (16 + szof(uint)))
#define	MSFILE_TABOFF		(MSFILE_HEADTABOFF + (3 * szof(uint)))

#define	MSFILE_ENTSIZE		MSFILEE_SIZE
#define	MSFILE_MAXFILESIZE	(4 * 1024 * 1024)
#define	MSFILE_NWAYS		4
#define	MSFILE_NEPW		(((8*1024) / MSFILE_ENTSIZE) - 1)
#define	MSFILE_NIDXENT		100

#if	CF_DEBUGTEST
#define	TO_OPEN		(5 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		30		/* seconds */
#else
#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		30		/* seconds */
#endif /* CF_DEBUGTEST */

#define	MS		msfile
#define	MS_ENT		msfile_ent


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
extern int	stroflags(char *,int) noex ;
#endif


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int msfile_ctor(msfile *op,Args ... args) noex {
    	MSFILE		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msfile_ctor) */

static int msfile_dtor(msfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msfile_dtor) */

template<typename ... Args>
static inline int msfile_magic(msfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MSFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (msfile_magic) */

static int	msfile_fileopen(MS *,time_t) noex ;
static int	msfile_fileclose(MS *) noex ;
static int	msfile_filesetinfo(MS *) noex ;
static int	msfile_lockget(MS *,time_t,int) noex ;
static int	msfile_lockrelease(MS *) noex ;
static int	msfile_filebegin(MS *,time_t) noex ;
static int	msfile_acquire(MS *,time_t,int) noex ;
static int	msfile_filecheck(MS *) noex ;
static int	msfile_entbufstart(MS *) noex ;
static int	msfile_entbuffinish(MS *) noex ;

static int	msfile_filetopwrite(MS *) noex ;
static int	msfile_filetopread(MS *) noex ;
static int	msfile_fileverify(MS *) noex ;
static int	msfile_headtab(MS *,int) noex ;

static int	msfile_findname(MS *,cchar *,int,char **) noex ;
static int	msfile_search(MS *,cchar *,int,char **) noex ;
static int	msfile_readentry(MS *,int,char **) noex ;

#if	CF_NISEARCH
static int	msfile_index(MS *,cchar *,int,int) noex ;
#endif

static int	msfile_headwrite(MS *) noex ;

static int	namematch(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int msfile_open(MS *op,cchar *fname,int oflags,mode_t operm) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	int		f_created = false ;
	cchar		*cp ;

#if	CF_DEBUGS
	debugprintf("msfile_open: ent\n") ;
#endif

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
#endif /* CF_SAFE */

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	{
	    char	timentbuf[TIMEBUFLEN + 1] ;
	    debugprintf("msfile_open: fname=%s\n", fname) ;
	    stroflags(timentbuf,oflags) ;
	    debugprintf("msfile_open: oflags=%s\n",timentbuf) ;
	    if (oflags & O_CREAT)
	        debugprintf("msfile_open: creating as needed\n") ;
	}
#endif /* CF_DEBUGS */

#if	CF_CREAT
	oflags |= O_CREAT ;
#endif
	oflags &= (~ O_TRUNC) ;

	memclear(op) ;
	op->pagesize = getpagesize() ;
	op->fd = -1 ;
	op->oflags = oflags ;
	op->operm = operm ;

	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    op->fname = cp ;
	    op->ti_mod = 0 ;
	    if ((rs = msfile_fileopen(op,dt)) >= 0) {
	        if ((rs = msfile_filebegin(op,dt)) >= 0) {
	            cint	n = MSFILE_NIDXENT ;
	            if ((rs = mapstrint_start(&op->ni,n)) >= 0) {
	                op->magic = MSFILE_MAGIC ;
	            }
	        }
	        if (rs < 0) {
	            msfile_fileclose(op) ;
	        }
	    }
	    if (rs < 0) {
	        uc_free(op->fname) ;
	        op->fname = nullptr ;
	    }
	} /* end if (m-a) */

#if	CF_DEBUGS
	debugprintf("msfile_open: ret rs=%d f_created=%u\n",rs,f_created) ;
#endif

	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (msfile_open) */

int msfile_close(MS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

#if	CF_DEBUGS
	debugprintf("msfile_close: mapstrint_finish() \n") ;
#endif

	rs1 = mapstrint_finish(&op->ni) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("msfile_close: _fileclose() \n") ;
#endif

	rs1 = msfile_fileclose(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("msfile_close: uc_free() \n") ;
#endif

	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

#if	CF_DEBUGS
	debugprintf("msfile_close: ret rs=%d\n") ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (msfile_close) */

int msfile_count(MS *op) noex {
	int		rs = SR_OK ;
	int		c ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	c = ((op->filesize - MSFILE_TABOFF) / MSFILE_ENTSIZE) ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msfile_count) */

int msfile_curbegin(MS *op,MSFILE_CUR *curp) noex {

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (curp == nullptr) return SR_FAULT ;

	op->ncursors += 1 ;
	opop->fl.cursorlockbroken = false ;
	opop->fl.cursoracc = false ;

	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (msfile_curbegin) */

int msfile_curend(MS *op,MSFILE_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (curp == nullptr) return SR_FAULT ;

	if (opop->fl.cursoracc) {
	    op->ti_access = time(nullptr) ;
	} /* end if */

	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

	bool	f = (op->ncursors == 0) ;
	f = f && (opop->fl.lockedread || opop->fl.lockedwrite) ;
	if (f) {
	    rs1 = msfile_lockrelease(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (msfile_curend) */

int msfile_enum(MS *op,MSFILE_CUR *curp,MS_ENT *ep) noex {
	time_t		dt = 0 ;
	int		ebs = MSFILE_ENTSIZE ;
	int		rs = SR_OK ;
	int		ei = 0 ;

#if	CF_DEBUGS
	debugprintf("msfile_enum: ent\n") ;
#endif

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (curp == nullptr) return SR_FAULT ;

	if (dt == 0) dt = time(nullptr) ;

	if ((rs = msfile_acquire(op,dt,1)) >= 0) {
	    char	*bp ;
	    ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	    if ((rs = msfile_readentry(op,ei,&bp)) >= 0) {
	        if ((ep != nullptr) && (bp != nullptr)) {
	            rs = msfilee_all(ep,1,bp,ebs) ;
	        }
	        if (rs >= 0) {
	            curp->i = ei ;
	            opop->fl.cursoracc = true ;
	        }
	    }
	} /* end if (msfile_acquire) */

#if	CF_DEBUGS
	debugprintf("msfile_enum: ret rs=%d ei=%u\n",rs,ei) ;
#endif

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_enum) */

int msfile_match(MS *op,time_t dt,cchar *nnp,int nnl,MS_ENT *ep) noex {
	cint		ebs = MSFILE_ENTSIZE ;
	int		rs = SR_OK ;
	int		i = MSFILE_NODENAMELEN ;
	int		ei = 0 ;
	char		*bp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (nnp == nullptr) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("msfile_match: ent nnl=%d nnp=%r\n",
	    nnl,nnp,lenstr(nnp,nnl)) ;
#endif

	if (nnl >= 0) {
	    i = MIN(nnl,MSFILE_NODENAMELEN) ;
	}

	nnl = lenstr(nnp,i) ;

#if	CF_DEBUGS
	debugprintf("msfile_match: nnl=%d nnp=%r\n",nnl,nnp,nnl) ;
#endif

	if (dt == 0) dt = time(nullptr) ;

	if ((rs = msfile_acquire(op,dt,1)) >= 0) {

	    rs = msfile_findname(op,nnp,nnl,&bp) ;
	    ei = rs ;

	    if ((rs >= 0) && (ep != nullptr)) {

#if	CF_DEBUGS 
	        debugprintf("msfile_match: found it rs=%d\n",rs) ;
#endif

	        rs = msfilee_all(ep,1,bp,ebs) ;

#if	CF_DEBUGS 
	        debugprintf("msfile_match: rs1=%d ep->nodename=%s\n",
	            rs,ep->nodename) ;
#endif

	    } /* end if */

/* if we are a writer (open for write), update the access time also */

	    if ((rs >= 0) && opop->fl.writable) {
	        MSFILEE_ATIME	a ;
	        if (dt == 0) dt = time(nullptr) ;
	        a.atime = dt ;
	        msfilee_atime(&a,0,bp,ebs) ;
	        if ((rs = entbuf_write(&op->ebm,ei,nullptr)) >= 0) {
	            rs = entbuf_sync(&op->ebm) ;
		}
	    } /* end if (updating access time) */

/* optionally release our lock if we didn't have a cursor outstanding */

	    if (op->ncursors == 0) {
	        msfile_lockrelease(op) ;
	        if (dt == 0) dt = time(nullptr) ;
	        op->ti_access = dt ;
	    } else {
	        opop->fl.cursoracc = true ;
	    }

	} /* end if (msfile_acquire) */

#if	CF_DEBUGS
	debugprintf("msfile_match: ret rs=%d ei=%u\n",rs,ei) ;
#endif

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_match) */

int msfile_write(MS *op,time_t dt,cchar *nnp,int nnl,MS_ENT *ep) noex {
	cint		ebs = MSFILE_ENTSIZE ;
	int		rs = SR_OK ;
	int		i = MSFILE_NODENAMELEN ;
	int		ei ;
	char		*bp ;

#if	CF_DEBUGS
	debugprintf("msfile_write: ent nnl=%d nodename=%r\n",
	    nnl,nnp,lenstr(nnp,nnl)) ;
#endif /* CF_DEBUGS */

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (nnp == nullptr) return SR_FAULT ;

	if (nnl >= 0)
	    i = MIN(nnl,MSFILE_NODENAMELEN) ;

	nnl = lenstr(nnp,i) ;

#if	CF_DEBUGS && 0
	debugprintf("msfile_write: nodename=%r\n",nnp,nnl) ;
#endif

	if ((rs = msfile_acquire(op,dt,1)) >= 0) {
	    if ((rs = msfile_findname(op,nnp,nnl,&bp)) >= 0) {
	        ei = rs ;

/* write the entry */

	        if (ep != nullptr) {
	            MSFILEE_ALL	ew ;
		    int	f = false ;
	            f = f || (ep->dtime == 0) ;
		    f = f || (ep->atime == 0) ;
		    f = f || (ep->stime == 0) ;
		    if (f) {
	                MSFILEE_DTIME	ed ;
	                MSFILEE_ATIME	ea ;
	                MSFILEE_STIME	es ;
	                int		f_swap = false ;

	                if (ep->dtime == 0) {
	                    msfilee_dtime(&ed,1,bp,ebs) ;
	                    if (ed.dtime != 0) {
	                        if (! f_swap) {
	                            f_swap = true ;
	                            ew = *ep ;
	                        }
	                        if (dt >= ed.dtime) {
	                            ew.flags &= (~ MSFLAG_MDISABLED) ;
	                            ew.dtime = 0 ;
	                        } else {
	                            ew.dtime = ed.dtime ;
	                        }
	                    } /* end if */
	                } /* end if (dtime) */

	                if (ep->atime == 0) {
	                    msfilee_atime(&ea,1,bp,ebs) ;
	                    if (ea.atime != 0) {
	                        if (! f_swap) {
	                            f_swap = true ;
	                            ew = *ep ;
	                        }
	                        ew.atime = ea.atime ;
	                    } /* end if */
	                } /* end if (atime) */

	                if (ep->stime == 0) {
	                    msfilee_stime(&es,1,bp,ebs) ;
	                    if (es.stime != 0) {
	                        if (! f_swap) {
	                            f_swap = true ;
	                            ew = *ep ;
	                        }
	                        ew.stime = es.stime ;
	                    } /* end if */
	                } /* end if (stime) */

	                if (f_swap)
	                    ep = &ew ;

	            } /* end if (some special handling) */

	            msfilee_all(ep,0,bp,ebs) ;

	            rs = entbuf_write(&op->ebm,ei,nullptr) ; /* sync */

	        } /* end if (writing entry) */

/* update the file header-table (for a write) */

	        if ((rs >= 0) && opop->fl.writable) {
	            if (dt == 0) dt = time(nullptr) ;
	            op->h.wcount += 1 ;
	            op->h.wtime = dt ;
	            if ((rs = msfile_headwrite(op)) >= 0) {
	                rs = entbuf_sync(&op->ebm) ;
	            }
	        } /* end if (updating header-table) */

/* optionally release our lock if we didn't have a cursor outstanding */

	        if (op->ncursors == 0) {
	            msfile_lockrelease(op) ;
	        }

/* update access time as appropriate */

	        if (op->ncursors == 0) {
	            if (dt == 0) dt = time(nullptr) ;
	            op->ti_access = dt ;
	        } else {
	            opop->fl.cursoracc = true ;
	        }

	    } /* end if (msfile_findname) */
	} /* end if (msfile_acquire) */

#if	CF_DEBUGS
	debugprintf("msfile_write: ret rs=%d ei=%u\n",rs,ei) ;
#endif

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_write) */

int msfile_update(MS *op,time_t dt,MS_ENT *ep) noex {
	int		rs = SR_OK ;
	int		ei = 0 ;

#if	CF_DEBUGS
	debugprintf("msfile_update: ent\n") ;
#endif

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (ep == nullptr) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("msfile_update: writable=%u\n",opop->fl.writable) ;
#endif

	if ((rs = msfile_acquire(op,dt,1)) >= 0) {
	    int		nnl ;
	    int		f_newentry = false ;
	    cchar	*nnp = ep->nodename ;
	    cint	ebs = MSFILE_ENTSIZE ;
	    char	entbuf[MSFILE_ENTSIZE + 2] ;
	    char	*bp ;

/* do the search */

	    nnl = lenstr(nnp,MSFILE_NODENAMELEN) ;
	    if (nnl > 0) {

	        if ((rs = msfile_findname(op,nnp,nnl,&bp)) >= 0) {
	            MSFILEE_ALL	ew ;
	            int		f ;
	            ei = rs ;

/* found existing entry */

#if	CF_DEBUGS
	            debugprintf("msfile_update: existing entry\n") ;
#endif

	            f = false ;
	            f = f || (ep->dtime == 0) ;
	            f = f || (ep->atime == 0) ;
	            f = f || (ep->stime == 0) ;
	            f = f || (ep->pid == 0) ;
	            if (f) { /* read-modify-write */

#ifdef	COMMENT
	                {
	                    MSFILEE_DTIME	ed ;
	                    MSFILEE_ATIME	ea ;
	                    MSFILEE_STIME	es ;
	                    int			f_swap = false ;

	                    if (ep->dtime == 0) {
	                        msfilee_dtime(&ed,1,bp,ebs) ;
	                        if (ed.dtime != 0) {
	                            if (! f_swap) {
	                                f_swap = true ;
	                                ew = *ep ;
	                            }
	                            if (dt >= ed.dtime) {
	                                ew.flags &= (~ MSFLAG_MDISABLED) ;
	                                ew.dtime = 0 ;
	                            } else {
	                                ew.dtime = ed.dtime ;
	                            }
	                        }
	                    } /* end if (dtime) */

	                    if (ep->atime == 0) {
	                        msfilee_atime(&ea,1,bp,ebs) ;
	                        if (ea.atime != 0) {
	                            if (! f_swap) {
	                                f_swap = true ;
	                                ew = *ep ;
	                            }
	                            ew.atime = ea.atime ;
	                        }
	                    } /* end if (atime) */

	                    if (ep->stime == 0) {
	                        msfilee_stime(&es,1,bp,ebs) ;
	                        if (es.stime != 0) {
	                            if (! f_swap) {
	                                f_swap = true ;
	                                ew = *ep ;
	                            }
	                            ew.stime = es.stime ;
	                        }
	                    } /* end if (stime) */

	                    if (f_swap)
	                        ep = &ew ;

	                }
#else /* COMMENT */
	                {
	                    MSFILEE_ALL	et ;

	                    ew = *ep ;
	                    msfilee_all(&et,1,bp,ebs) ;

	                    if (ep->dtime == 0) {
	                        if (et.dtime != 0) {
	                            if (dt >= et.dtime) {
	                                ew.flags &= (~ MSFLAG_MDISABLED) ;
	                                ew.dtime = 0 ;
	                            } else {
	                                ew.dtime = et.dtime ;
	                            }
	                        }
	                    } /* end if (dtime) */

	                    if (ep->atime == 0) {
	                        ew.atime = et.atime ;
	                    } /* end if (atime) */

	                    if (ep->stime == 0) {
	                        ew.stime = et.stime ;
	                    } /* end if (stime) */

	                    if (ep->pid == 0) {
	                        ew.pid = et.pid ;
	                    }

	                    ep = &ew ;
	                }
#endif /* COMMENT */

	            } /* end if (some special handling) */

	            msfilee_all(ep,0,bp,ebs) ;

	            rs = entbuf_write(&op->ebm,ei,nullptr) ; /* sync */

#if	CF_DEBUGS
	            debugprintf("msfile_update: 0 entbuf_write() rs=%d\n",rs) ;
#endif

	        } else if (rs == SR_NOTFOUND) {
	            MSFILEE_ALL		ew ;

	            f_newentry = true ;
	            if (ep->atime == 0) {
	                ew = *ep ;
	                ew.atime = ep->utime ;
	                ep = &ew ;
	            } /* end if (atime) */

	            msfilee_all(ep,0,entbuf,ebs) ;

	            ei = op->h.nentries ;
	            rs = entbuf_write(&op->ebm,ei,entbuf) ;

#if	CF_DEBUGS
	            debugprintf("msfile_update: 1 entbuf_pwrite() rs=%d\n",
			    rs) ;
#endif

	        } /* end if (entry update) */

/* update the file header */

#if	CF_DEBUGS
	        debugprintf("msfile_update: header rs=%d\n",rs) ;
#endif

	        if (rs >= 0) {
	            if (dt == 0) dt = time(nullptr) ;
	            op->h.wcount += 1 ;
	            op->h.wtime = dt ;
	            if (f_newentry) {
	                op->h.nentries += 1 ;
	                op->filesize += ebs ;
	            }
	            if ((rs = msfile_headwrite(op)) >= 0) {
	                if ((rs = entbuf_sync(&op->ebm)) >= 0) {
	                    if (op->ncursors == 0) {
	                        rs = msfile_lockrelease(op) ;
	                        if (dt == 0) dt = time(nullptr) ;
	                        op->ti_access = dt ;
	                    } else {
	                        opop->fl.cursoracc = true ;
	                    }
	                }
	            } /* end if (ok) */

	        } /* end if (updating header-table) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (msfile_acquire) */

#if	CF_DEBUGS
	debugprintf("msfile_update: ret rs=%d ei=%u\n",rs,ei) ;
#endif

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_update) */

int msfile_check(MS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSFILE_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (op->fd >= 0) {
	    if ((! opop->fl.lockedread) && (! opop->fl.lockedwrite)) {
	        if (dt == 0) dt = time(nullptr) ;
	        f = ((dt - op->ti_access) >= TO_ACCESS) ;
	        f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	        if (f) {
	            rs = msfile_fileclose(op) ;
	        }
	    } /* end if (not locked) */
	} /* end if (open) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msfile_check) */


/* private subroutines */

static int msfile_findname(MS *op,cchar *nnp,int nnl,char **rpp) noex {
	int		rs ;
	int		ei = 0 ;
	char		*bp = nullptr ;
	char		*np ;

	if ((rs = mapstrint_fetch(&op->ni,nnp,nnl,nullptr,&ei)) >= 0) {

#if	CF_DEBUGS
	    debugprintf("msfile_update: mapstrint_fetch() rs=%d\n",rs) ;
	    if (rs >= 0)
	        debugprintf("msfile_update: ei=%d\n",ei) ;
#endif

	    if ((rs = entbuf_read(&op->ebm,ei,&bp)) > 0) {

#if	CF_DEBUGS
	        debugprintf("msfile_update: entbuf_read() rs=%d\n",rs) ;
#endif

	        np = bp + MSFILEE_ONODENAME ;
	        if (! namematch(np,nnp,nnl)) {
	            rs = SR_NOTFOUND ;
	            mapstrint_delkey(&op->ni,nnp,nnl) ;
	        } /* end if */

	    } else {
	        rs = SR_NOTFOUND ;
	    }

	} /* end if (was in the index) */

#if	CF_DEBUGS
	debugprintf("msfile_update: mid rs=%d\n",rs) ;
#endif

	if (rs == SR_NOTFOUND) {
	    if ((rs = msfile_search(op,nnp,nnl,&bp)) >= 0) {
	        ei = rs ;
	        rs = mapstrint_add(&op->ni,nnp,nnl,ei) ;
	    }
	} /* end if */

	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? bp : nullptr ;
	}

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_findname) */

static int msfile_search(MS *op,cchar *nnp,int nnl,char **rpp) noex {
	cint		ebs = MSFILE_ENTSIZE ;
	int		rs = SR_OK ;
	int		i ;
	int		ne ;
	int		ei = 0 ;
	int		f_found ;
	char		*bp ;
	char		*np = nullptr ;

#if	CF_DEBUGS
	debugprintf("msfile_search: ent nnl=%u nodename=%r\n",
	    nnl,nnp,lenstr(nnp,nnl)) ;
#endif

	if (nnl < 0)
	    nnl = lenstr(nnp) ;

#if	CF_DEBUGS 
	debugprintf("msfile_search: nnl=%u pagesize=%u ebs=%u\n",
	    nnl,op->pagesize,ebs) ;
#endif

	ei = 0 ;
	ne = 0 ;
	f_found = false ;
	while ((rs >= 0) && (! f_found)) {

	    rs = entbuf_read(&op->ebm,ei,&bp) ;
	    ne = rs ;

#if	CF_DEBUGS
	    debugprintf("msfile_search: entbuf_read() rs=%d\n",rs) ;
#endif

	    if (rs <= 0)
	        break ;

	    for (i = 0 ; (rs >= 0) && (i < ne) ; i += 1) {

	        np = (bp + MSFILEE_ONODENAME) ;

/* is this a match for what we want? */

#if	CF_DEBUGS
	        debugprintf("msfile_search: i=%u db_node=%r\n",i,
	            np,lenstr(np,MSFILEE_LNODENAME)) ;
#endif

	        if (namematch(np,nnp,nnl)) {
	            f_found = true ;
	            break ;
	        }

	        bp += ebs ;

	    } /* end for (looping through entries) */

#if	CF_DEBUGS
	    debugprintf("msfile_search: for-out i=%d f_found=%u\n",
	        i,f_found) ;
#endif

	    ei += i ;

#if	CF_DEBUGS
	    debugprintf("msfile_search: ei=%d f_found=%u\n",ei,f_found) ;
#endif

	    if (f_found) break ;
	} /* end while */

#if	CF_DEBUGS
	debugprintf("msfile_search: fin rs=%d f_found=%u\n",rs,f_found) ;
	debugprintf("msfile_search: ne=%d \n",ne) ;
#endif

	if (rs >= 0) {
	    if ((ne != 0) && f_found) {
	        if (rpp != nullptr) *rpp = bp ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if */

#if	CF_DEBUGS
	debugprintf("msfile_search: ret rs=%d ei=%u\n",rs,ei) ;
#endif

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msfile_search) */

static int msfile_acquire(MS *op,time_t dt,int f_read) noex {
	cint		ebs = MSFILE_ENTSIZE ;
	int		rs = SR_OK ;
	int		f_changed = false ;
	int		f ;

#if	CF_DEBUGS
	debugprintf("msfile_acquire: ent\n") ;
#endif

	if (dt == 0)
	    dt = time(nullptr) ;

/* is the file open? */

	if (op->fd < 0) {
	    rs = msfile_fileopen(op,dt) ;
	}

	if ((rs >= 0) && (! f_read) && opop->fl.lockedread) {
	    rs = msfile_lockrelease(op) ;
	}

/* capture the lock if we do not already have it */

	f = (opop->fl.lockedread || opop->fl.lockedwrite) ;
	if ((rs >= 0) && (! f)) {
	    if ((rs = msfile_lockget(op,dt,f_read)) >= 0) {
	        f_changed = (rs > 0) ;
	        if ((rs = msfile_filecheck(op)) >= 0) {
	            f_changed = f_changed || (rs > 0) ;
	            if (f_changed) {
	                int n = ((op->filesize - MSFILE_TOPLEN) / ebs) ;
	                rs = entbuf_invalidate(&op->ebm,n) ;
	            }
	        }
	    }
	} /* end if (need lock) */

#if	CF_DEBUGS
	debugprintf("msfile_acquire: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msfile_acquire) */

static int msfile_filebegin(MS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_locked = false ;

#if	CF_DEBUGS
	debugprintf("msfile_filebegin: ent fd=%d filesize=%u\n",
	    op->fd,op->filesize) ;
#endif

	if (op->filesize == 0) {

	    opop->fl.fileinit = false ;
	    if (opop->fl.writable) {

	        if (opop->fl.lockedread) {
	            rs = msfile_lockrelease(op) ;
		}

	        if ((rs >= 0) && (! opop->fl.lockedwrite)) {
	            f_locked = true ;
	            rs = msfile_lockget(op,dt,0) ;
	        }

	        if (rs >= 0) {
	            rs = msfile_filetopwrite(op) ;
	            f_locked = (rs > 0) ;
	        }

	    } /* end if (writable) */

	} else if (op->filesize >= MSFILE_TABOFF) {
	    int		f ;

/* read the file header */

	    f = (opop->fl.lockedread || opop->fl.lockedwrite) ;
	    if (! f) {
	        rs = msfile_lockget(op,dt,1) ;
	        f_locked = (rs >= 0) ;
	    }

#if	CF_DEBUGS
	    debugprintf("msfile_filebegin: msfile_fileverify() \n") ;
#endif

	    if (rs >= 0) {
	        if ((rs = msfile_filetopread(op)) >= 0) {
	            if ((rs = msfile_fileverify(op)) >= 0) {
	        	rs = msfile_headtab(op,1) ;
	        	opop->fl.fileinit = (rs >= 0) ;
		    }
		}
	    }

	} /* end if */

/* if we locked, we unlock it, otherwise leave it! */

	if (f_locked) {
	    msfile_lockrelease(op) ;
	}

#if	CF_DEBUGS
	debugprintf("msfile_filebegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (msfile_filebegin) */

static int msfile_filecheck(MS *op) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;

	if (op->filesize < MSFILE_TABOFF) {
	    f_changed = true ;
	    if (opop->fl.writable) {
	        rs = msfile_filetopwrite(op) ;
	    }
	} else {
	    if ((rs = msfile_filetopread(op)) >= 0) {
	        if ((rs = msfile_fileverify(op)) >= 0) {
	            rs = msfile_headtab(op,1) ;
	            f_changed = (rs > 0) ;
	            opop->fl.fileinit = (rs >= 0) ;
		}
	    }
	} /* end if */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msfile_filecheck) */

static int msfile_filetopwrite(MS *op) noex {
	off_t	poff = 0L ;
	int		ml ;
	int		rs = SR_OK ;
	int		bl ;
	char		*bp = op->topbuf ;

/* write the file header stuff */

	ml = mkmagic(bp,MSFILE_FILEMAGICSIZE,MSFILE_FILEMAGIC) ;
	bp += ml ;

	*bp++ = MSFILE_FILEVERSION ;
	*bp++ = MSFILE_ENDIAN ;
	*bp++ = 0 ;		/* file type */
	*bp++ = 0 ;		/* unused */

/* next is the header (we just write zeros here) */

	memset(bp,0,MSFILE_HEADTABLEN) ;
	bp += MSFILE_HEADTABLEN ;

	bl = bp - op->topbuf ;
	op->fileversion = MSFILE_FILEVERSION ;
	op->filetype = 0 ;
	op->h = {} ;
	if ((rs = u_pwrite(op->fd,op->topbuf,bl,poff)) >= 0) {
	    op->filesize = rs ;
	    op->topsize = rs ;
	    opop->fl.fileinit = true ;
	}

	return rs ;
}
/* end subroutine (msfile_filetopwrite) */

static int msfile_filetopread(MS *op) noex {
	off_t	poff = 0L ;
	int		rs ;

	rs = u_pread(op->fd,op->topbuf,MSFILE_TOPLEN,poff) ;
	op->topsize = rs ;

	return rs ;
}
/* end subroutine (msfile_filetopread) */

static int msfile_fileverify(MS *op) noex {
	int		rs = SR_OK ;

#if	CF_DEBUGS
	debugprintf("msfile_fileverify: ent\n") ;
	debugprintf("msfile_fileverify: filemagic=%s\n",
		MSFILE_FILEMAGIC) ;
	debugprintf("msfile_fileverify: filemagicsize=%u\n",
		MSFILE_FILEMAGICSIZE) ;
#endif

	if (op->topsize >= MSFILE_TOPLEN) {
	    cint	msize = MSFILE_FILEMAGICSIZE ;
	    cchar	*cp = op->topbuf ;
#if	CF_DEBUGS
	    debugprintf("msfile_fileverify: ms=%r\n",cp,strlinelen(cp,-1,40)) ;
#endif
	    if (hasValidMagic(cp,msize,MSFILE_FILEMAGIC)) {
	        cp += msize ;
	        if (cp[0] <= MSFILE_FILEVERSION) {
	            op->fileversion = cp[0] ;
	            if (cp[1] == MSFILE_ENDIAN) {
	                op->filetype = cp[2] ;
	            } else {
	                rs = SR_NOTSUP ;
	            }
	        } else {
	            rs = SR_NOTSUP ;
	        }
	    } else {
	        rs = SR_BADFMT ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

#if	CF_DEBUGS
	debugprintf("msfile_fileverify: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (msfile_fileverify) */

static int msfile_headtab(MS *op,int f_read) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	char		*bp = (op->topbuf + MSFILE_HEADTABOFF) ;

	if (op->topbuf == nullptr) return SR_BADFMT ;

	if (f_read) {
	    struct msfile_h	h ;

	    int	hsize = szof(struct msfile_h) ;

	    stdorder_rui(bp,&h.nentries) ;
	    bp += szof(uint) ;

	    stdorder_rui(bp,&h.wtime) ;
	    bp += szof(uint) ;

	    stdorder_rui(bp,&h.wcount) ;
	    bp += szof(uint) ;

	    f_changed = (memcmp(&h,&op->h,hsize) != 0) ;
	    op->h = h ;

	} else {

	    stdorder_wui(bp,op->h.nentries) ;
	    bp += szof(uint) ;

	    stdorder_wui(bp,op->h.wtime) ;
	    bp += szof(uint) ;

	    stdorder_wui(bp,op->h.wcount) ;
	    bp += szof(uint) ;

	} /* end if */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msfile_headtab) */

static int msfile_lockget(MS *op,time_t dt,int f_read) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		lockcmd ;
	int		f_already = false ;
	int		f_changed = false ;

#if	CF_DEBUGS && 0
	debugprintf("msfile_lockget: ent f_read=%d\n",f_read) ;
#endif

	if (op->fd < 0) {
	    rs = msfile_fileopen(op,dt) ;
	} /* end if (needed to open the file) */

/* acquire a file record lock */

	if (rs >= 0) {

	    if (f_read || (! opop->fl.writable)) {
	        f_already = opop->fl.lockedread ;
	        opop->fl.lockedread = true ;
	        opop->fl.lockedwrite = false ;
	        lockcmd = F_RLOCK ;
	    } else {
	        f_already = opop->fl.lockedwrite ;
	        opop->fl.lockedread = false ;
	        opop->fl.lockedwrite = true ;
	        lockcmd = F_WLOCK ;
	    } /* end if */

/* get out if we have the lock that we want already */

	    if (! f_already) {

/* we need to actually do the lock */

#if	CF_LOCKF
	        rs = uc_lockf(op->fd,F_LOCK,0z) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	        rs = lockfile(op->fd,lockcmd,0z,0z,TO_LOCK) ;
#else
	        {
	            off_t	fs = op->filesize ;
	            rs = lockfile(op->fd,lockcmd,0z,fs,TO_LOCK) ;
	        }
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

#if	CF_DEBUGS && 0
	        debugprintf("msfile_lockget: LOCK lockfile() rs=%d\n",rs) ;
#endif

	        if (rs >= 0) {
	            if ((rs = u_fstat(op->fd,&sb)) >= 0) {

	                f_changed = 
	                    (sb.st_size != op->filesize) ||
	                    (sb.st_mtime != op->ti_mod) ;

	                if (f_changed) {
	                    if (opop->fl.bufvalid) opop->fl.bufvalid = false ;
	                    op->filesize = sb.st_size ;
	                    op->ti_mod = sb.st_mtime ;
	                } /* end if */

	                if (op->filesize < MSFILE_TABOFF) {
	                    opop->fl.fileinit = false ;
	                }

	            } else {
	                msfile_lockrelease(op) ;
	            }
	        } else {
	            msfile_lockrelease(op) ;
	        }

	    } /* end if (not already) */

	} /* end if (ok) */

#if	CF_DEBUGS && 0
	debugprintf("msfile_lockget: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msfile_lockget) */

static int msfile_lockrelease(MS *op) noex {
	int		rs = SR_OK ;

#if	CF_DEBUGS && 0
	debugprintf("msfile_lockrelease: ent\n") ;
#endif

	if ((opop->fl.lockedread || opop->fl.lockedwrite)) {

	    if (op->fd >= 0) {

#if	CF_LOCKF
	        rs = uc_lockf(op->fd,F_ULOCK,0z) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	        rs = lockfile(op->fd,F_ULOCK,0z,0z,TO_LOCK) ;
#else
	        {
	            off_t	fs = op->filesize ;
	            rs = lockfile(op->fd,F_ULOCK,0z,fs,TO_LOCK) ;
	        }
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

#if	CF_DEBUGS && 0
	        debugprintf("msfile_lockrelease: lockfile() rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS
	        debugprintf("msfile_lockrelease: UNLOCK lockfile() rs=%d\n",
	            rs) ;
#endif

	    } /* end if (file was open) */

	    opop->fl.lockedread = false ;
	    opop->fl.lockedwrite = false ;

	} /* end if (there was a possible lock set) */

#if	CF_DEBUGS
	debugprintf("msfile_lockrelease: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (msfile_lockrelease) */

static int msfile_fileopen(MS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_created = false ;

#if	CF_DEBUGS
	debugprintf("msfile_fileopen: fname=%s\n",op->fname) ;
#endif

	if (op->fd < 0) {
	    int		oflags = (op->oflags & (~ O_CREAT)) ;
	    opop->fl.fileinit = false ;
	    opop->fl.lockedread = false ;
	    opop->fl.lockedwrite = false ;
	    rs = u_open(op->fname,oflags,op->operm) ;
	    op->fd = rs ;
	    if (isNotPresent(rs) && (op->oflags & O_CREAT)) {
	        f_created = true ;
	        oflags = op->oflags ;
	        rs = u_open(op->fname,oflags,op->operm) ;
	        op->fd = rs ;
	    } /* end if (creating file) */
	    if (rs >= 0) {
	        if (dt == 0) dt = time(nullptr) ;
	        op->ti_open = dt ;
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	            if ((rs = msfile_filesetinfo(op)) >= 0) {
	                rs = msfile_entbufstart(op) ;
	            }
	        }
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	            op->ti_open = 0 ;
	        }
	    } /* end if (ok) */
	} /* end if (open-needed) */

#if	CF_DEBUGS
	debugprintf("msfile_fileopen: ret rs=%d f_create=%u\n",rs,f_created) ;
#endif

	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (msfile_fileopen) */

int msfile_fileclose(MS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (opop->fl.entbuf) {
	    rs1 = msfile_entbuffinish(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->fd >= 0) {
	    opop->fl.lockedread = false ;
	    opop->fl.lockedwrite = false ;
	    rs1 = u_close(op->fd) ;
	    op->fd = -1 ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (msfile_fileclose) */

static int msfile_filesetinfo(MS *op) noex {
	USTAT		sb ;
	int		rs ;
	int		amode ;
	amode = (op->oflags & O_ACCMODE) ;
	opop->fl.writable = ((amode == O_WRONLY) || (amode == O_RDWR)) ;
	if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	    op->ti_mod = sb.st_mtime ;
	    op->filesize = sb.st_size ;
	    if ((rs = isfsremote(op->fd)) > 0) {
	        opop->fl.remote = true ;
	    }
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (msfile_filesetinfo) */

static int msfile_entbufstart(MS *op) noex {
	int		rs = SR_OK ;

	if (! opop->fl.entbuf) {
	    if (op->fd >= 0) {
	        uint	soff = MSFILE_TABOFF ;
	        int	esize = MSFILE_ENTSIZE ;
	        int	nways = MSFILE_NWAYS ;
	        int	n = MSFILE_NEPW ;
	        rs = entbuf_start(&op->ebm,op->fd,soff,esize,nways,n) ;
	        opop->fl.entbuf = (rs >= 0) ;
	    } else {
	        rs = SR_NOANODE ;
	    }
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (msfile_entbufstart) */

static int msfile_entbuffinish(MS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (opop->fl.entbuf) {
	    opop->fl.entbuf = false ;
	    rs1 = entbuf_finish(&op->ebm) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (msfile_entbuffinish) */

static int msfile_readentry(MS *op,int ei,char **rpp) noex {
	int		rs ;
	char		*bp ;

	if ((rs = entbuf_read(&op->ebm,ei,&bp)) >= 0) {
	    if (rs == 0) rs = SR_NOTFOUND ;
	    if (rpp != nullptr) {
	        *rpp = (rs >= 0) ? bp : nullptr ;
	    }

/* enter into the index */

#if	CF_NIENUM
	    if (rs >= 0) {
	        int	nl ;
	        char	*np ;
	        np = bp + MSFILEE_ONODENAME ;
	        nl = lenstr(np,MSFILEE_LNODENAME) ;
	        rs = msfile_index(op,np,nl,ei) ;
	    } /* end if (got an entry) */
#endif /* CF_NIENUM */

	} /* end if */

#if	CF_DEBUGS
	debugprintf("msfile_readentry: ret rs=%d \n",rs) ;
#endif

	return rs ;
}
/* end subroutine (msfile_readentry) */


#if	CF_NISEARCH

static int msfile_index(MS *op,cchar *np,int nl,int ei) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		ei2 ;

	if (nl < 0)
	    nl = lenstr(np) ;

	rs1 = mapstrint_fetch(&op->ni,np,nl,nullptr,&ei2) ;

	if ((rs1 >= 0) && (ei != ei2)) {
	    rs1 = SR_NOTFOUND ;
	    mapstrint_delkey(&op->ni,np,nl) ;
	}

	if (rs1 == SR_NOTFOUND) {
	    int	nl2 = lenstr(np,MSFILEE_LNODENAME) ;
	    mapstrint_add(&op->ni,np,nl2,ei) ;
	} /* end if (not found) */

	return rs ;
}
/* end subroutine (msfile_index) */

#endif /* CF_NISEARCH */

static int msfile_headwrite(MS *op) noex {
	int		rs ;

	if ((rs = msfile_headtab(op,0)) >= 0) {
	    off_t	poff ;
	    cint	toff = MSFILE_TABOFF ;
	    cint	htoff = MSFILE_HEADTABOFF ;
	    int		bl ;
	    cchar	*bp ;
	    bp = (op->topbuf + htoff) ;
	    bl = (toff - htoff) ;
	    poff = htoff ;
	    rs = u_pwrite(op->fd,bp,bl,poff) ;
	}

	return rs ;
}
/* end subroutine (msfile_headwrite) */


static int namematch(cchar *np,cchar *nnp,int nnl)
{
	int		f = false ;

#if	CF_DEBUGS
	debugprintf("namematch: ent np=%s\n",np) ;
	debugprintf("namematch: nnl=%d nnp=%r\n",nnl,nnp,lenstr(nnp,nnl)) ;
#endif

	if (nnl <= MSFILEE_LNODENAME) {
	    f = (strncmp(np,nnp,nnl) == 0) ;
	    f = f && (np[nnl] == '\0') ;
	}

#if	CF_DEBUGS
	debugprintf("namematch: ret f=%u\n",f) ;
#endif

	return f ;
}
/* end subroutine (namematch) */


