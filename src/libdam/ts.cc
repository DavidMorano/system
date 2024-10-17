/* ts SUPPORT (Time-Stamp) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* time-stamp file manager */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */
#define	CF_LOCKF	0		/* use |lockf(3c)| */
#define	CF_SOLARISBUG	1		/* work around Solaris MMAP bug */
#define	CF_NIENUM	0		/* perform NI updates on ENUM */
#define	CF_NISEARCH	0		/* perform NI updates on SEARCH */

/* revision history:

	= 1991-06-01, David A­D­ Morano
	This code was originally written.

	= 2003-06-26, David A­D­ Morano
	Although this object works, it was only a micracle that it
	did.  There is a feature-bug in Solaris that does not allow
	a file to be both mapped and locked at the same time (in
	either order).  But there seems to be a crack in the stupid
	Solaris implementation because it does not enforce its stupid
	bug carefully enough and this object here fell through the
	cracks and continued working by accident.  We were locking
	the whole file beyond its end and that appears to get by
	the Solaris police-state bug-patrol and was accidentally
	being allowed.  I reworked a good bit of this code to
	eliminate any file mapping (so that we can continue to use
	file-record locks).  This whole Solaris crap (this is being
	done on Solaris 8 right now) is really a pain and Sun should
	face punitive charges for inhumanity to the programmer
	community.  Solaris has some nice things since it was derived
	from the older (and better?) System V UNIX®, but has really
	messed it up by not allowing what used to be allowed in the
	old days with things like the old RFS facility.  Oh, while
	we are on the subject: NFS sucks cock meat!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	ts

	Description:
	This subroutine maintains a Time-Stamp (TS) file.  
	Format of file records:

	- (see the 'entry' structure in the header)

	Design note: 
	In summary, Solaris sucks cock meat!  Solaris does not allow
	a file to be memory-mapped from an NFS remote server *and*
	also be file-locked at the same time.  A lot of stupid Solaris
	documentation notes say something to the effect that the
	Solaris VM system cannot handle a remote file that is both
	mapped and subject to file-locking at the same time.  They
	use some sort of stupid circular reasoning that if any file
	is being file-locked, then obviously it cannot be memory-mapped
	since the file locking indicates that file-locking is taking
	place, and that obviously any file that is being file-locked
	cannot therefore also be memory mapped.  That is pretty much
	their reasoning -- I kid you not!

	Unfortunately, code, like this code here, that was first
	designed under System V UNIX® that used file-locking *and*
	memory mapping together really needs to be changed to
	eliminate either the file locking or the memory mapping.
	Remote files were cross mounted in the late 80s and very
	early 90s using RFS (not stupid NFS).  The use of RFS provided
	many advantages not the least of them being full UFS
	file-system semantics, but it is not clear why Solaris took
	a step backward from simply allowing remote files to be
	both memory-mapped and file-locked at the same time.  Some
	bright light-bulb of a software developer must have gotten
	his underwear in a bunch at some point and decided to
	disallow both of these from ever occurring at the same time
	in Solaris.  We all have suffered from these dumb-butt Solaris
	developers since we have to take time out to re-debug-write
	old code (like this code here) to handle the case of stupid
	Solaris not allowing memory mapping for a file that is also
	file-locked.

	Implementation note:
	The code was actually running when files were being locked
	in their entirety and beyond their ends.  There was some
	sort of loop-hole in the stupid Solaris code that allowed
	a file to be both file-locked and memory mapped at the same
	time under certain circumstances.  However, there seemed to
	be problems with this code when other parties on other
	(remote) systems tried to do the same thing.  They sometimes
	failed with dead-lock types of errors (I forget the details).
	As a result, I decided to change the code to fully comply
	with the stupid Solaris requirements that no remote file
	be both memory mapped and file locked at the same time.  Any
	code that is here now that has to be with mapping of files
	is really just code that now allocates local private memory.
	This is done instead of using the process heap but was
	really done because it seemed to offer the minimal changes
	to the code to get a private memory access to the file while
	still retaining the ability to file-lock the file.  Finally,
	let me finish with the comment that Solaris sucks cock meat.

	Final note:
	Solaris sucks cock meat! Give me back simultaneous memory
	mapping and file locking.  And while you're at it, give me
	back RFS also! And to you stupid Solaris VM developers, get
	out of Solaris development.  Either get a new job somewhere
	else or think about committing suicide.  Either way, we can
	all be happier with one (or more) of those alternatives.

	Anecdotal note:
	Hey, you stupid Solaris developers: give me back the ability
	to push SOCKMOD on a TPI endpoint also! Since you're so
	stupid, I know that you forgot that this was possible at
	one time.  You hosed that ability away when you botched up
	making Solaris 2.6.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<unistd.h>		/* |getpagesize(3c)| */
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cinttypes>
#include	<cstring>		/* |strlen(3c)| */
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<sysval.hh>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<mapstrint.h>
#include	<stdorder.h>
#include	<localmisc.h>
#include	<hash.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<mkx.h>			/* |mkmagic(3uc)| */
#include	<getfstype.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"ts.h"
#include	"tse.h"
#include	"ebuf.h"


/* local defines */

#define	TS_IDOFF	0
#define	TS_HEADTABOFF	(TS_IDOFF + (16 + sizeof(uint)))
#define	TS_TABOFF	(TS_HEADTABOFF + (3 * sizeof(uint)))

#define	TS_ENTSIZE	TSE_SIZE
#define	TS_MAXFILESIZE	(4 * 1024 * 1024)
#define	TS_NWAYS	4
#define	TS_NEPW		(((8*1024) / TS_ENTSIZE) - 1)
#define	TS_NIDXENT	100

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		30		/* seconds */

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
static int ts_ctor(ts *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    ts_head	*hop = op ;
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->ebmp = new(nothrow) ebuf) != np) {
	        if ((op->nip = new(nothrow) mapstrint) != np) {
		    op->pagesize = getpagesize() ;
		    rs = SR_OK ;
		} /* end if (new-mapstrint) */
		if (rs < 0) {
		    delete op->ebmp ;
		    op->ebmp = nullptr ;
		}
	    } /* end if (new-ebuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ts_ctor) */

static int ts_dtor(ts *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->nip) {
		delete op->nip ;
		op->nip = nullptr ;
	    }
	    if (op->ebmp) {
		delete op->ebmp ;
		op->ebmp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ts_dtor) */

template<typename ... Args>
static inline int ts_magic(ts *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ts_magic) */

static int	ts_fileopen(ts *,time_t) noex ;
static int	ts_fileclose(ts *) noex ;
static int	ts_filesetinfo(ts *,time_t) noex ;
static int	ts_lockget(ts *,time_t,int) noex ;
static int	ts_lockrelease(ts *) noex ;
static int	ts_filebegin(ts *,time_t) noex ;
static int	ts_acquire(ts *,time_t,int) noex ;
static int	ts_filecheck(ts *,time_t) noex ;
static int	ts_ebufstart(ts *) noex ;
static int	ts_ebuffinish(ts *) noex ;

static int	ts_filetopwrite(ts *,time_t) noex ;
static int	ts_filetopread(ts *) noex ;
static int	ts_fileverify(ts *) noex ;
static int	ts_headtab(ts *,int) noex ;

static int	ts_findname(ts *,cchar *,int,char **) noex ;
static int	ts_search(ts *,cchar *,int,char **) noex ;
static int	ts_readentry(ts *,int,char **) noex ;
static int	ts_index(ts *,cchar *,int,int) noex ;
static int	ts_headwrite(ts *) noex ;

static bool	namematch(cchar *,cchar *,int) noex ;


/* local variables */

constexpr int		toplen = TS_TOPLEN ;
constexpr int		taboff = TS_TABOFF ;
constexpr int		entsize = TS_ENTSIZE ;
constexpr int		nidxent = TS_NIDXENT ;
constexpr int		maglen = TS_FILEMAGICSIZE ;

constexpr bool		f_sunos = F_SUNOS ;

constexpr bool		f_creat = CF_CREAT ;
constexpr bool		f_lockf = CF_LOCKF ;
constexpr bool		f_nienum = CF_NIENUM ;
constexpr bool		f_solarisbug = CF_SOLARISBUG ;


/* exported variables */


/* exported subroutines */

int ts_open(ts *op,cchar *fname,int oflags,mode_t operm) noex {
	int		rs ;
	int		f_created = false ;
	if ((rs = ts_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        cchar		*cp ;
	        op->fd = -1 ;
	        op->oflags = oflags ;
	        op->operm = operm ;

		if_constexpr (f_creat) {
	            oflags |= O_CREAT ;
		}
	        oflags = (oflags & (~ O_TRUNC)) ;
	        if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
		    custime	dt = getustime ;
	            op->fname = cp ;
	            if ((rs = ts_fileopen(op,dt)) >= 0) {
		        if ((rs = ts_filebegin(op,dt)) >= 0) {
		            if ((rs = mapstrint_start(op->nip,nidxent)) >= 0) {
			        op->magic = TS_MAGIC ;
		            }
		        }
		        if (rs < 0) {
		            ts_fileclose(op) ;
		        }
	            }
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (mallocstr) */
	    } /* end if (valid) */
	    if (rs < 0) {
		ts_dtor(op) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ts_open) */

int ts_close(ts *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ts_magic(op)) >= 0) {
	    {
	        rs1 = mapstrint_finish(op->nip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ts_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = ts_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ts_close) */

int ts_count(ts *op) noex {
	int		rs ;
	int		c ;
	if ((rs = ts_magic(op)) >= 0) {
	    c = (op->filesize - taboff) / entsize ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ts_count) */

int ts_curbegin(ts *op,ts_cur *curp) noex {
	int		rs ;
	if ((rs = ts_magic(op,curp)) >= 0) {
	    op->ncursors += 1 ;
	    op->f.cursorlockbroken = false ;
	    op->f.cursoracc = false ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ts_curbegin) */

int ts_curend(ts *op,ts_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ts_magic(op,curp)) >= 0) {
	    bool	f = true ;
	    if (op->f.cursoracc) {
	        op->ti_access = getustime ;
	    } /* end if */
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	    f = f && (op->ncursors == 0) ;
	    f = f && (op->f.lockedread || op->f.lockedwrite) ;
	    if (f) {
	        rs1 = ts_lockrelease(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ts_curend) */

int ts_curenum(ts *op,ts_cur *curp,ts_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = ts_magic(op,curp)) >= 0) {
	    custime	dt = getustime ;
	    if ((rs = ts_acquire(op,dt,1)) >= 0) {
	        char	*bp ;
	        ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	        rs = ts_readentry(op,ei,&bp) ;
	        if ((rs >= 0) && (ep != nullptr) && (bp != nullptr)) {
	            rs = tse_all(ep,1,bp,entsize) ;
	        } /* end if */
	        if (rs >= 0) curp->i = ei ;
	        op->f.cursoracc = true ;
	    } /* end if (acquire) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_curenum) */

/* match on a key-name */
int ts_match(ts *op,time_t dt,cchar *nnp,int nnl,ts_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = ts_magic(op,nnp)) >= 0) {
	    int		idx = TS_KEYNAMELEN ;
	    char		*bp ;
	    if (nnl >= 0) {
	        idx = min(nnl,TS_KEYNAMELEN) ;
	    }
	    nnl = strnlen(nnp,idx) ;
	    if (dt == 0) dt = time(nullptr) ;
	    if ((rs = ts_acquire(op,dt,1)) >= 0) {
	        if ((rs = ts_findname(op,nnp,nnl,&bp)) >= 0) {
	            ei = rs ;
	            if (ep) {
	                rs = tse_all(ep,1,bp,entsize) ;
	            } /* end if */
/* optionally release our lock if we did not have a cursor outstanding */
	            if (op->ncursors == 0) {
	                ts_lockrelease(op) ;
	            }
/* update access time as appropriate */
	            if (op->ncursors == 0) {
	                if (dt == 0) dt = time(nullptr) ;
	                op->ti_access = dt ;
	            } else {
	                op->f.cursoracc = true ;
	            }
	        } /* end if (ts_findname) */
	    } /* end if (ts_acquire) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_match) */

/* write an entry (match on a key-name) */
int ts_write(ts *op,time_t dt,cchar *nnp,int nnl,ts_ent *ep) noex {
	int		rs = SR_OK ;
	int		i ;
	int		ei ;
	int		f_newentry = false ;
	char		*bp ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;

	if (nnp == nullptr) return SR_FAULT ;

#ifdef	COMMENT
	if (ep == nullptr) return SR_FAULT ;
#endif

	i = TS_KEYNAMELEN ;
	if (nnl >= 0) {
	    i = MIN(nnl,TS_KEYNAMELEN) ;
	}
	nnl = strnlen(nnp,i) ;

	rs = ts_acquire(op,dt,1) ;
	if (rs < 0)
	     goto ret0 ;

	rs = ts_findname(op,nnp,nnl,&bp) ;
	ei = rs ;

/* write the entry */

	if (dt == 0) dt = time(nullptr) ;

	if (rs >= 0) {
	    ts_ent	ew ;

  	    if (ep != nullptr) {
		ew = *ep ;
	    } else {
		memset(&ew,0,sizeof(ts_ent)) ;
	    }

	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    if (ew.keyname[0] == '\0') {
		strdcpy1w(ew.keyname,TSE_LKEYNAME,nnp,nnl) ;
	    }
	    if (ew.hash == 0) {
		ew.hash = hash_elf(ew.keyname,-1) ;
	    }

	    tse_all(&ew,0,bp,entsize) ;

	    rs = ebuf_write(op->ebmp,ei,nullptr) ; /* sync */

	} else if (rs == SR_NOTFOUND) {
	    ts_ent	ew ;
	    char	ebuf[TS_ENTSIZE + 2] ;

	    f_newentry = true ;
  	    if (ep != nullptr) {
		ew = *ep ;
	    } else {
		memset(&ew,0,sizeof(ts_ent)) ;
	    }

	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    if (ew.keyname[0] == '\0') {
		strdcpy1w(ew.keyname,TSE_LKEYNAME,nnp,nnl) ;
	    }
	    if (ew.hash == 0) ew.hash = hash_elf(ew.keyname,-1) ;

	    tse_all(&ew,0,ebuf,entsize) ;

	    ei = op->h.nentries ;
	    rs = ebuf_write(op->ebmp,ei,ebuf) ;

	} /* end if (existing or new entry) */

/* update the file header-table (for a write) */

	if ((rs >= 0) && op->f.writable) {

#ifdef	COMMENT
	    if (dt == 0) dt = time(nullptr) ;
#endif

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;
	    if (f_newentry) {
	        op->h.nentries += 1 ;
	        op->filesize += entsize ;
	    }
	    if ((rs = ts_headwrite(op)) >= 0) {
		rs = ebuf_sync(op->ebmp) ;
	    }

	} /* end if (updating header-table) */

/* optionally release our lock if we didn't have a cursor outstanding */

	if (op->ncursors == 0)
	    ts_lockrelease(op) ;

/* update access time as appropriate */

	if (op->ncursors == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_access = dt ;
	} else {
	    op->f.cursoracc = true ;
	}

ret0:
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_write) */

/* update an entry */
int ts_update(ts *op,time_t dt,ts_ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nnl ;
	int		ei ;
	int		f_newentry = false ;
	cchar	*nnp ;
	char		ebuf[TS_ENTSIZE + 2] ;
	char		*bp ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->keyname[0] == '\0') return SR_INVALID ;

	rs = ts_acquire(op,dt,1) ;

	if (rs < 0)
	     goto ret0 ;

/* do the search */

	nnp = ep->keyname ;
	nnl = strnlen(nnp,TS_KEYNAMELEN) ;

	if (nnl == 0) {
	    rs = SR_INVALID ;
	    goto ret2 ;
	}

	rs = ts_findname(op,nnp,nnl,&bp) ;
	ei = rs ;

/* update the entry that we found and write it back */

	if (rs >= 0) {
	    ts_ent	ew ;

	    tse_all(&ew,1,bp,entsize) ;

	    ew.utime = dt ;
	    ew.count += 1 ;
	    tse_all(&ew,0,bp,entsize) ;

	    rs = ebuf_write(op->ebmp,ei,nullptr) ; /* sync */

	} else if (rs == SR_NOTFOUND) {
	    ts_ent	ew = *ep ;

	    f_newentry = true ;
	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    ew.hash = hash_elf(ew.keyname,-1) ;

	    tse_all(&ew,0,ebuf,entsize) ;

	    ei = op->h.nentries ;
	    rs = ebuf_write(op->ebmp,ei,ebuf) ;

	} /* end if (entry update) */

/* update the file header */

	if (rs >= 0) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;
	    if (f_newentry) {
	        op->h.nentries += 1 ;
	        op->filesize += entsize ;
	    }

	    rs = ts_headwrite(op) ;

	    if (rs >= 0)
		rs = ebuf_sync(op->ebmp) ;

	} /* end if (updating header-table) */

/* optionally release our lock if we didn't have a cursor outstanding */
ret2:

	if (op->ncursors == 0) {
	    rs1 = ts_lockrelease(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

/* update access time as appropriate */
	if (op->ncursors == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_access = dt ;
	} else {
	    op->f.cursoracc = true ;
	}

ret0:
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_update) */

/* do some checking */
int ts_check(ts *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = ts_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        if ((! op->f.lockedread) && (! op->f.lockedwrite)) {
	            if (dt == 0) dt = time(nullptr) ;
	            f = ((dt - op->ti_access) >= TO_ACCESS) ;
	            f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	            if (f) {
	                rs = ts_fileclose(op) ;
	            }
	        }
	    } /* end if (file-desc) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ts_check) */


/* private subroutines */

static int ts_findname(ts *op,cchar *nnp,int nnl,char **rpp) noex {
	cnullptr	np{} ;
	int		rs ;
	int		ei = 0 ;
	char		*bp = nullptr ;
	if ((rs = mapstrint_fetch(op->nip,nnp,nnl,np,&ei)) >= 0) {
	    if ((rs = ebuf_read(op->ebmp,ei,&bp)) > 0) {
	        cchar	*sp = bp + TSE_OKEYNAME ;
	        if (! namematch(sp,nnp,nnl)) {
	            rs = SR_NOTFOUND ;
	            mapstrint_delkey(op->nip,nnp,nnl) ;
	        } /* end if */
	    } else {
	        if ((rs = ts_search(op,nnp,nnl,&bp)) >= 0) {
	            ei = rs ;
	            rs = mapstrint_add(op->nip,nnp,nnl,ei) ;
	        }
	    } /* end if */
	} /* end if (fetch) */
	if (rpp) {
	    *rpp = (rs >= 0) ? bp : nullptr ;
	}
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_findname) */

/* search for an entry */
static int ts_search(ts *op,cchar *nnp,int nnl,char **rpp) noex {
	int		rs = SR_OK ;
	int		i ;
	int		ne = 0 ;
	int		ei = 0 ;
	int		f_found = false ;
	char		*bp = nullptr ;

	if (nnl < 0) nnl = strlen(nnp) ;

	while ((rs >= 0) && (! f_found)) {

	    rs = ebuf_read(op->ebmp,ei,&bp) ;
	    ne = rs ;
	    if (rs <= 0) break ;

	    for (i = 0 ; (rs >= 0) && (i < ne) ; i += 1) {

	        cchar	*sp = bp + TSE_OKEYNAME ;
/* is this a match for what we want? */
	        if (namematch(sp,nnp,nnl)) {
		    f_found = true ;
	            break ;
		}

		bp += entsize ;

	    } /* end for (looping through entries) */

	    ei += i ;

	    if (f_found) break ;
	} /* end while */

	if (rs >= 0) {
	    if ((ne != 0) && f_found) {
		if (rpp) {
		    *rpp = bp ;
		}
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_search) */

static int ts_acquire(ts *op,time_t dt,int f_read) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	int		f ;

	if (dt == 0) dt = getustime ;

/* is the file open? */

	if (op->fd < 0) {
	    rs = ts_fileopen(op,dt) ;
	}

	if ((rs >= 0) && (! f_read) && op->f.lockedread) {
	    rs = ts_lockrelease(op) ;
	}

/* capture the lock if we do not already have it */

	f = (op->f.lockedread || op->f.lockedwrite) ;
	if ((rs >= 0) && (! f)) {
	    if ((rs = ts_lockget(op,dt,f_read)) >= 0) {
	        f_changed = (rs > 0) ;
	        rs = ts_filecheck(op,dt) ;
	        f_changed = f_changed || (rs > 0) ;
	        if ((rs >= 0) && f_changed) {
		    int n = (op->filesize - toplen) / entsize ;
	            rs = ebuf_invalidate(op->ebmp,n) ;
		}
	    }
	} /* end if (need lock) */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_acquire) */

/* initialize the file header (either read it only or write it) */
static int ts_filebegin(ts *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_locked = false ;
	int		f ;

	if (op->filesize == 0) {

	    op->f.fileinit = false ;
	    if (op->f.writable) {

		if (op->f.lockedread) {
		    rs = ts_lockrelease(op) ;
		}

	        if ((rs >= 0) && (! op->f.lockedwrite)) {
	            f_locked = true ;
	            rs = ts_lockget(op,dt,0) ;
	        }

		if (rs >= 0) {
	            rs = ts_filetopwrite(op,dt) ;
	            f_locked = (rs > 0) ;
		}

	    } /* end if (writable) */

	} else if (op->filesize >= taboff) {

/* read the file header */

	    f = (op->f.lockedread || op->f.lockedwrite) ;
	    if (! f) {
	        rs = ts_lockget(op,dt,1) ;
	        f_locked = (rs >= 0) ;
	    }

	    if (rs >= 0)
	        rs = ts_filetopread(op) ;

	    if (rs >= 0)
	        rs = ts_fileverify(op) ;

	    if (rs >= 0) {
	        rs = ts_headtab(op,1) ;
	        op->f.fileinit = (rs >= 0) ;
	    }

	} /* end if */

/* if we locked, we unlock it, otherwise leave it! */

	if (f_locked) {
	    ts_lockrelease(op) ;
	}

	return rs ;
}
/* end subroutine (ts_filebegin) */

static int ts_filecheck(ts *op,time_t dt) noex {
	int	rs = SR_OK ;
	int	f_changed = false ;
	if (op->filesize < taboff) {
	    f_changed = true ;
	    if (op->f.writable) {
		rs = ts_filetopwrite(op,dt) ;
	    }
	} else {
	    if ((rs = ts_filetopread(op)) >= 0) {
	        if ((rs = ts_fileverify(op)) >= 0) {
	            if ((rs = ts_headtab(op,1)) >= 0) {
	                op->f.fileinit = true ;
		        f_changed = (rs > 0) ;
		    }
		}
	    }
	} /* end if */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_filecheck) */

static int ts_filetopwrite(ts *op,time_t dt) noex {
	char		*bp = op->topbuf ;
	int		rs ;
	(void) dt ;
	if ((rs = mkmagic(bp,maglen,TS_FILEMAGIC)) >= 0) {
	    coff	poff = 0 ;
	    int		bl ;
	    bp += rs ;
	    {
	        *bp++ = TS_FILEVERSION ;
	        *bp++ = TS_ENDIAN ;
	        *bp++ = 0 ;		/* file type */
	        *bp++ = 0 ;		/* unused */
	    }
            /* next is the header (we just write zeros here) */
	    {
	        memset(bp,0,TS_HEADTABLEN) ;
	        bp += TS_HEADTABLEN ;
	    }
	    {
	        bl = (bp - op->topbuf) ;
	        op->fileversion = TS_FILEVERSION ;
	        op->filetype = 0 ;
	        op->h = {} ;
	    }
	    if ((rs = u_pwrite(op->fd,op->topbuf,bl,poff)) >= 0) {
	        op->filesize = rs ;
	        op->topsize = rs ;
	        op->f.fileinit = true ;
	    } /* end if (u_pwrite) */
	} /* end if (mkmagic) */
	return rs ;
}
/* end subroutine (ts_filetopwrite) */

static int ts_filetopread(ts *op) noex {
	coff		poff = 0L ;
	int		rs ;
	if ((rs = u_pread(op->fd,op->topbuf,toplen,poff)) >= 0) {
	    op->topsize = rs ;
	}
	return rs ;
}
/* end subroutine (ts_filetopread) */

static int ts_fileverify(ts *op) noex {
	static cint	magl = strlen(TS_FILEMAGIC) ;
	int		rs = SR_OK ;
	bool		f ;
	cchar		*magp = TS_FILEMAGIC ;
	cchar		*cp ;

	if (op->topsize < toplen) {
	    rs = SR_INVALID ;
	    goto ret0 ;
	}

	cp = op->topbuf ;
	f = (strncmp(cp,magp,magl) == 0) ;
	f = f && (*(cp + magl) == '\n') ;

	if (! f) {

	    rs = SR_BADFMT ;
	    goto ret0 ;
	}

	cp += 16 ;
	if (cp[0] > TS_FILEVERSION) {
	    rs = SR_NOTSUP ;
	    goto ret0 ;
	}

	op->fileversion = cp[0] ;

	if (cp[1] != TS_ENDIAN) {
	    rs = SR_NOTSUP ;
	    goto ret0 ;
	}

	op->filetype = cp[2] ;

ret0:

	return rs ;
}
/* end subroutine (ts_fileverify) */

/* read or write the file header */
static int ts_headtab(ts *op,int f_read) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	char		*bp = (op->topbuf + TS_HEADTABOFF) ;
	if (f_read) {
	    ts_hdr	h{} ;
	    int		hsize = sizeof(ts_hdr) ;
	    stdorder_rui(bp,&h.nentries) ;
	    bp += sizeof(uint) ;
	    stdorder_rui(bp,&h.wtime) ;
	    bp += sizeof(uint) ;
	    stdorder_rui(bp,&h.wcount) ;
	    bp += sizeof(uint) ;
	    f_changed = (memcmp(&h,&op->h,hsize) != 0) ;
	    op->h = h ;
	} else {
	    stdorder_wui(bp,op->h.nentries) ;
	    bp += sizeof(uint) ;
	    stdorder_wui(bp,op->h.wtime) ;
	    bp += sizeof(uint) ;
	    stdorder_wui(bp,op->h.wcount) ;
	    bp += sizeof(uint) ;
	} /* end if */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_headtab) */

/* acquire access to the file */
static int ts_lockget(ts *op,time_t dt,int f_read) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		lockcmd ;
	int		f_already = false ;
	int		f_changed = false ;

	if (op->fd < 0) {

	    rs = ts_fileopen(op,dt) ;

	    if (rs < 0)
	        goto bad0 ;

	} /* end if (needed to open the file) */

/* acquire a file record lock */

	if (f_read || (! op->f.writable)) {

	    f_already = op->f.lockedread ;
	    op->f.lockedread = true ;
	    op->f.lockedwrite = false ;
	    lockcmd = F_RLOCK ;

	} else {

	    f_already = op->f.lockedwrite ;
	    op->f.lockedread = false ;
	    op->f.lockedwrite = true ;
	    lockcmd = F_WLOCK ;

	} /* end if */

/* get out if we have the lock that we want already */

	if (f_already)
	    goto ret0 ;

/* we need to actually do the lock */

	if_constexpr (f_lockf) {
	    rs = uc_lockf(op->fd,F_LOCK,0L) ;
	} else {
	    if_constexpr (f_solarisbug) {
	        rs = lockfile(op->fd,lockcmd,0L,0L,TO_LOCK) ;
	    } else {
	        coff	fs = op->filesize ;
	        rs = lockfile(op->fd,lockcmd,0L,fs,TO_LOCK) ;
	    } /* end if_constexpr (f_solarisbug) */
	} /* end if_constexpr (f_lockf) */

	if (rs < 0)
	    goto bad2 ;

/* has the file changed at all? */

	rs = u_fstat(op->fd,&sb) ;

#ifdef	COMMENT
	if (rs == SR_NOENT)
	    rs = SR_OK ;
#endif /* COMMENT */

	if (rs < 0)
	    goto bad2 ;

	f_changed = 
	    (sb.st_size != op->filesize) ||
	    (sb.st_mtime != op->ti_mod) ;

	if (f_changed) {

	    if (op->f.bufvalid)
	        op->f.bufvalid = false ;

	    op->filesize = sb.st_size ;
	    op->ti_mod = sb.st_mtime ;

	} /* end if */

	if (op->filesize < taboff) {
	    op->f.fileinit = false ;
	}

ret0:

	return (rs >= 0) ? f_changed : rs ;

/* bad stuff */
bad2:
	op->f.fileinit = false ;

#if	CF_LOCKF
	rs = uc_lockf(op->fd,F_ULOCK,0L) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	lockfile(op->fd,F_ULOCK,0L,0L,TO_LOCK) ;
#else
	{
	    off_t	fs = op->filesize ;
	    lockfile(op->fd,F_ULOCK,0L,fs,TO_LOCK) ;
	}
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

	op->f.lockedread = false ;
	op->f.lockedwrite = false ;

bad0:
	goto ret0 ;
}
/* end subroutine (ts_lockget) */

static int ts_lockrelease(ts *op) noex {
	int		rs = SR_OK ;
	if ((op->f.lockedread || op->f.lockedwrite)) {
	    if (op->fd >= 0) {
		cint	cmd = F_ULOCK ;
		if_constexpr (f_lockf) {
	            rs = uc_lockf(op->fd,cmd,0L) ;
	 	} else {
	            if_constexpr (f_solarisbug) {
	                rs = lockfile(op->fd,cmd,0L,0L,TO_LOCK) ;
	            } else {
		        coff	fs = op->filesize ;
	                rs = lockfile(op->fd,cmd,0L,fs,TO_LOCK) ;
		    } /* end if_constexpr (f_solarisbug) */
		} /* end if_constexpr (f_lockf) */
	    } /* end if (file was open) */
	    op->f.lockedread = false ;
	    op->f.lockedwrite = false ;
	} /* end if (there was a possible lock set) */
	return rs ;
}
/* end subroutine (ts_lockrelease) */

static int ts_fileopen(ts *op,time_t dt) noex {
	int	rs = SR_OK ;
	int	f_created = false ;
	if (op->fd < 0) {
	    int		of = (op->oflags & (~ O_CREAT)) ;
	    op->f.fileinit = false ;
	    op->f.lockedread = false ;
	    op->f.lockedwrite = false ;
	    if ((rs = u_open(op->fname,of,op->operm)) >= 0) {
	        op->fd = rs ;
	    } else if (isNotPresent(rs) && (op->oflags & O_CREAT)) {
	        f_created = true ;
	        of = op->oflags ;
	        rs = u_open(op->fname,of,op->operm) ;
	        op->fd = rs ;
	    } /* end if (creating file) */
	    if (rs >= 0) {
	        if (dt == 0) dt = getustime ;
	        op->ti_open = dt ;
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	            if ((rs = ts_filesetinfo(op,dt)) >= 0) {
	    	        rs = ts_ebufstart(op) ;
		    }
	        }
	    } /* end if (ok) */
	} /* end if (needed to be opened) */
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ts_fileopen) */

int ts_fileclose(ts *op) noex {
	int	rs ;
	int	rs1 ;
	if ((rs = ts_magic(op)) >= 0) {
	    if (op->f.ebuf) {
	        rs1 = ts_ebuffinish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fd >= 0) {
	        op->f.lockedread = false ;
	        op->f.lockedwrite = false ;
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ts_fileclose) */

static int ts_filesetinfo(ts *op,time_t dt) noex {
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		am = (op->oflags & O_ACCMODE) ;
	(void) dt ;
	op->f.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	    char	*fsbuf{} ;
	    op->ti_mod = sb.st_mtime ;
	    op->filesize = sb.st_size ;
	    if ((rs = malloc_fs(&fsbuf)) >= 0) {
		cint	fslen = rs ;
	        if ((rs = getfstype(fsbuf,fslen,op->fd)) >= 0) {
		    cbool	f = (matlocalfs(fsbuf,rs) >= 0) ;
	            op->f.remote = (! f) ; /* remote if not local! */
	        } /* end if (getfstype) */
		rs1 = uc_free(fsbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (u_fstat) */
	return rs ;
}
/* end subroutine (ts_filesetinfo) */

static int ts_ebufstart(ts *op) noex {
	int		rs = SR_OK ;
	if (! op->f.ebuf) {
	    if (op->fd >= 0) {
		cuint	soff = taboff ;
		cint	esize = TS_ENTSIZE ;
		cint	nways = TS_NWAYS ;
		cint	n = TS_NEPW ;
		rs = ebuf_start(op->ebmp,op->fd,soff,esize,nways,n) ;
		op->f.ebuf = (rs >= 0) ;
	    } else {
		rs = SR_NOANODE ;
	    }
	}
	return rs ;
}
/* end subroutine (ts_ebufstart) */

static int ts_ebuffinish(ts *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->f.ebuf) {
	    op->f.ebuf = false ;
	    rs1 = ebuf_finish(op->ebmp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (ts_ebuffinish) */

static int ts_readentry(ts *op,int ei,char **rpp) noex {
	int		rs ;
	char		*bp = nullptr ;
	if ((rs = ebuf_read(op->ebmp,ei,&bp)) > 0) {
	    if_constexpr (f_nienum) {
	        int	nl ;
	        char	*sp = bp + TSE_OKEYNAME ;
	        nl = strnlen(sp,TSE_LKEYNAME) ;
	        rs = ts_index(op,sp,nl,ei) ;
	    } /* end if_constexpr (f_nienum) */
	} else if (rs == 0) {
	    rs = SR_NOTFOUND ;
	} /* end if (ebuf_read) */
	if (rpp) {
	    *rpp = (rs >= 0) ? bp : nullptr ;
	}
	return rs ;
}
/* end subroutine (ts_readentry) */

/* add to the name-index if necessary */
static int ts_index(ts *op,cchar *sp,int sl,int ei) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs  ;
	int		ei2 ;
	if (sl < 0) sl = strlen(sp) ;
	if ((rs = mapstrint_fetch(op->nip,sp,sl,nullptr,&ei2)) >= 0) {
	    if (ei != ei2) {
	        if ((rs = mapstrint_delkey(op->nip,sp,sl)) >= 0) {
	            cint	nl2 = strnlen(sp,TSE_LKEYNAME) ;
	            rs = mapstrint_add(op->nip,sp,nl2,ei) ;
		}
	    }
	} else if (rs == rsn) {
	    cint	nl2 = strnlen(sp,TSE_LKEYNAME) ;
	    rs = mapstrint_add(op->nip,sp,nl2,ei) ;
	} /* end if (not found) */
	return rs ;
}
/* end subroutine (ts_index) */

static int ts_headwrite(ts *op) noex {
	int		rs ;
	int		headtaboff = TS_HEADTABOFF ;
	if ((rs = ts_headtab(op,0)) >= 0) {
	    cchar	*bp = (op->topbuf + headtaboff) ;
	    cint	bl = (taboff - headtaboff) ;
	    coff	poff = headtaboff ;
	    rs = u_pwrite(op->fd,bp,bl,poff) ;
	}
	return rs ;
}
/* end subroutine (ts_headwrite) */

static bool namematch(cc *sp,cc *nnp,int nnl) noex {
	bool		f = false ;
	if (nnl <= TSE_LKEYNAME) {
	    f = (strncmp(sp,nnp,nnl) == 0) ;
	    f = f && (sp[nnl] == '\0') ;
	}
	return f ;
}
/* end subroutine (namematch) */


