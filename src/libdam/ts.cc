/* ts SUPPORT (Time-Stamp) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* time-stamp file manager */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */
#define	CF_LOCKF	0		/* use |lockf(3c)| */
#define	CF_SOLARISBUG	1		/* work around Solaris® MMAP bug */
#define	CF_NIENUM	0		/* perform NI updates on ENUM */

/* revision history:

	= 1991-06-01, David A­D­ Morano
	This code was originally written.

	= 1999-06-26, David A­D­ Morano
	Although this object works, it was only a micracle that it
	did.  There is a feature-bug in Solaris® that does not allow
	a file to be both mapped and locked at the same time (in
	either order).  But there seems to be a crack in the stupid
	Solaris® implementation because it does not enforce its stupid
	bug carefully enough and this object here fell through the
	cracks and continued working by accident.  We were locking
	the whole file beyond its end and that appears to get by
	the Solaris® police-state bug-patrol and was accidentally
	being allowed.  I reworked a good bit of this code to
	eliminate any file mapping (so that we can continue to use
	file-record locks).  This whole Solaris® crap (this is being
	done on Solaris® 8 right now) is really a pain and Sun should
	face punitive charges for inhumanity to the programmer
	community.  Solaris® has some nice things since it was derived
	from the older (and better?) System V UNIX®, but has really
	messed it up by not allowing what used to be allowed in the
	old days with things like the old RFS facility.  Oh, while
	we are on the subject: NFS sucks cock meat!

*/

/* Copyright © 1991,1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	ts

	Description:
	This subroutine maintains a Time-Stamp (TS) file.  
	Format of file records:

	- (see the 'entry' structure in the header)

	Design note: 
	In summary, Solaris® sucks cock meat!  Solaris® does not allow
	a file to be memory-mapped from an NFS remote server *and*
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
	designed under System V UNIX® that used file-locking *and*
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
	mapping and file locking.  And while you're at it, give me
	back RFS also! And to you stupid Solaris® VM developers, get
	out of Solaris® development.  Either get a new job somewhere
	else or think about committing suicide.  Either way, we can
	all be happier with one (or more) of those alternatives.

	Anecdotal note:
	Hey, you stupid Solaris® developers: give me back the ability
	to push SOCKMOD on a TPI endpoint also!  Since you're so
	stupid,  I know that you forgot that this was possible at
	one time.  You hosed that ability away when you botched up
	making Solaris® 2.6.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| + |strnlen(3c)| */
#include	<new>			/* |nothrow(3c++)| */
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
#include	<hash.h>		/* |hash_elf(3dam)| */
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<sncpyx.h>		/* |sncpy(3uc)| */
#include	<mkx.h>			/* |mkmagic(3uc)| */
#include	<getfstype.h>
#include	<entbuf.h>
#include	<funmode.hh>
#include	<isnot.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"ts.h"
#include	"tse.hh"

import libutil ;

/* local defines */

#define	TS_IDOFF	0
#define	TS_HEADTABOFF	(TS_IDOFF + (16 + szof(uint)))
#define	TS_TABOFF	(TS_HEADTABOFF + (3 * szof(uint)))

#define	TS_NWAYS	4
#define	TS_NEPW		(((8*1024) / var.entsz) - 1)
#define	TS_NIDXENT	100

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		30		/* seconds */

#ifndef	CF_CREAT
#define	CF_CREAT	0		/* always create the file? */
#endif
#ifndef	CF_LOCKF
#define	CF_LOCKF	0		/* use |lockf(3c)| */
#endif
#ifndef	CF_SOLARISBUG
#define	CF_SOLARISBUG	1		/* work around Solaris® MMAP bug */
#endif
#ifndef	CF_NIENUM
#define	CF_NIENUM	0		/* perform NI updates on ENUM */
#endif

#define	FV		vetu[0]
#define	FE		vetu[1]
#define	FT		vetu[2]


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef ts_ent		tsent ;


/* external subroutines */


/* external variables */


/* local structures */

enum vetus {
    vetu_version,
    vetu_endian,
    vetu_type,
    vetu_unused,
    vetu_overlast
} ;

namespace {
    struct vars {
	int	entsz ;
	operator int () noex ;
    } ;
}


/* forward references */

template<typename ... Args>
static int ts_ctor(ts *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    ts_head	*hop = op ;
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->ebmp = new(nothrow) entbuf) != np) {
	        if ((op->nip = new(nothrow) mapstrint) != np) {
		    rs = SR_OK ;
		} /* end if (new-mapstrint) */
		if (rs < 0) {
		    delete op->ebmp ;
		    op->ebmp = nullptr ;
		}
	    } /* end if (new-entbuf) */
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
static int	ts_lockacq(ts *,time_t,int) noex ;
static int	ts_lockrel(ts *) noex ;
static int	ts_filebegin(ts *,time_t) noex ;
static int	ts_acquire(ts *,time_t,int) noex ;
static int	ts_filecheck(ts *,time_t) noex ;
static int	ts_ebufstart(ts *) noex ;
static int	ts_ebuffinish(ts *) noex ;

static int	ts_filetopwrite(ts *,time_t) noex ;
static int	ts_filetopread(ts *) noex ;
static int	ts_fileverify(ts *) noex ;
static int	ts_headtab(ts *,funmode) noex ;

static int	ts_updater(ts *,time_t,ts_ent *,cc *,int) noex ;
static int	ts_findname(ts *,cchar *,int,char **) noex ;
static int	ts_search(ts *,cchar *,int,char **) noex ;
static int	ts_readentry(ts *,int,char **) noex ;
static int	ts_index(ts *,cchar *,int,int) noex ;
static int	ts_headwrite(ts *) noex ;

static int	tsent_load(tsent *,cchar *,char * = nullptr,int = -1) noex ;
static int	tsent_trans(tsent *,tse *) noex ;

static bool	namematch(cchar *,cchar *,int) noex ;


/* local variables */

static vars		var ;

constexpr int		toplen = TS_TOPLEN ;
constexpr int		taboff = TS_TABOFF ;
constexpr int		nidxent = TS_NIDXENT ;
constexpr int		maglen = TS_FILEMAGICSIZE ;

constexpr char		magstr[] = TS_FILEMAGIC ;

constexpr bool		f_comment = false ;
constexpr bool		f_creat = CF_CREAT ;
constexpr bool		f_lockf = CF_LOCKF ;
constexpr bool		f_nienum = CF_NIENUM ;
constexpr bool		f_solarisbug = CF_SOLARISBUG ;


/* exported variables */


/* exported subroutines */

static int ts_opens(ts *) noex ;

int ts_open(ts *op,cchar *fname,int oflags,mode_t operm) noex {
	int		rs ;
	int		f_created = false ;
	if ((rs = ts_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            op->fd = -1 ;
	            op->oflags = oflags ;
	            op->operm = operm ;
		    if_constexpr (f_creat) {
	                oflags |= O_CREAT ;
		    } /* end if_constexpr (f_creat) */
	            oflags = (oflags & (~ O_TRUNC)) ;
	            if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
        	        op->fname = cp ;
		        rs = ts_opens(op) ;
		        f_created = (rs > 0) ;
			if (rs < 0) {
			    uc_free(op->fname) ;
			    op->fname = nullptr ;
			}
	            } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		ts_dtor(op) ;
	    }
	} /* end if (ts_ctor) */
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ts_open) */

static int ts_opens(ts *op) noex {
    	int		rs ;
	int		fcreated = false ;
        custime     dt = getustime ;
        if ((rs = ts_fileopen(op,dt)) >= 0) {
            fcreated = (rs > 0) ;
            if ((rs = ts_filebegin(op,dt)) >= 0) {
                if ((rs = mapstrint_start(op->nip,nidxent)) >= 0) {
                    op->magic = TS_MAGIC ;
                }
            }
            if (rs < 0) {
                ts_fileclose(op) ;
            }
        } /* end if (ts_fileopen) */
	return (rs >= 0) ? fcreated : rs ;
}
/* end subroutine (ts_opens) */

int ts_close(ts *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ts_magic(op)) >= 0) {
	    if (op->nip) {
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
	int		c = 0 ;
	if ((rs = ts_magic(op)) >= 0) {
	    c = (op->filesize - taboff) / var.entsz ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ts_count) */

int ts_curbegin(ts *op,ts_cur *curp) noex {
	int		rs ;
	if ((rs = ts_magic(op,curp)) >= 0) {
	    op->ncursors += 1 ;
	    op->fl.cursorlockbroken = false ;
	    op->fl.cursoracc = false ;
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
	    if (op->fl.cursoracc) {
	        op->tiaccess = getustime ;
	    } /* end if */
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	    f = f && (op->ncursors == 0) ;
	    f = f && (op->fl.lockedread || op->fl.lockedwrite) ;
	    if (f) {
	        rs1 = ts_lockrel(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ts_curend) */

int ts_curenum(ts *op,ts_cur *curp,char *rbuf,int rlen,ts_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = ts_magic(op,curp)) >= 0) {
	    custime	dt = getustime ;
	    if ((rs = ts_acquire(op,dt,1)) >= 0) {
	        ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	        if (char *bp ; (rs = ts_readentry(op,ei,&bp)) >= 0) {
	            if (ep && bp) {
			rs = tsent_load(ep,bp,rbuf,rlen) ;
		    }
	        } /* end if */
	        if (rs >= 0) {
		    curp->i = ei ;
		}
	        op->fl.cursoracc = true ;
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
	    int		idx = var.entsz ;
	    if (nnl >= 0) {
	        idx = min(nnl,var.entsz) ;
	    }
	    nnl = lenstr(nnp,idx) ;
	    if (dt == 0) dt = getustime ;
	    if ((rs = ts_acquire(op,dt,1)) >= 0) {
	        if (char *bp ; (rs = ts_findname(op,nnp,nnl,&bp)) >= 0) {
	            ei = rs ;
	            if (ep && bp) {
	                rs = tsent_load(ep,bp) ;
	            } /* end if */
		    /* optionally release our lock */
	            if (op->ncursors == 0) {
	                rs = ts_lockrel(op) ;
	            }
		    /* update access time as appropriate */
	            if (op->ncursors == 0) {
	                op->tiaccess = dt ;
	            } else {
	                op->fl.cursoracc = true ;
	            }
	        } /* end if (ts_findname) */
	    } /* end if (ts_acquire) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_match) */

/* write an entry (match on a key-name) */
int ts_write(ts *op,time_t dt,cchar *nnp,int nnl,ts_ent *ep) noex {
	int		rs ;
	int		rs1 ;
	int		ei = 0 ;
	if ((rs = ts_magic(op,nnp,ep)) >= 0) {
	    int		i = var.entsz ;
	    bool	f_newentry = false ;
	    if (nnl >= 0) {
	        i = min(nnl,var.entsz) ;
	    }
	    nnl = lenstr(nnp,i) ;
	    if ((rs = ts_acquire(op,dt,1)) >= 0) {
		if (tse ew ; (rs = ew.start) >= 0) {
	            if (char *bp ; (rs = ts_findname(op,nnp,nnl,&bp)) >= 0) {
		        ei = rs ;
		        /* write the entry */
		        if (dt == 0) dt = getustime ;
  	                if (ep) {
			    rs = tsent_trans(ep,&ew) ;
			}
	                if (ew.count == 0) ew.count = 1 ;
	                if (ew.utime == 0) ew.utime = uint(dt) ;
	                if (ew.ctime == 0) ew.ctime = uint(dt) ;
	                if (ew.keyname[0] == '\0') {
		            strdcpy1w(ew.keyname,TSE_LKEYNAME,nnp,nnl) ;
	                }
	                if (ew.hash == 0) {
		            ew.hash = hash_elf(ew.keyname,-1) ;
	                }
	                if ((rs = ew.rd(bp,var.entsz)) >= 0) {
	                    rs = entbuf_write(op->ebmp,ei,nullptr) ; /* sync */
		        }
	            } else if (rs == SR_NOTFOUND) {
	                f_newentry = true ;
  	                if (ep) {
			    rs = tsent_trans(ep,&ew) ;
			}
			if (rs >= 0) {
	                    char	ebuf[var.entsz + 2] ;
	                    if (ew.count == 0) ew.count = 1 ;
	                    if (ew.utime == 0) ew.utime = uint(dt) ;
	                    if (ew.ctime == 0) ew.ctime = uint(dt) ;
	                    if (ew.keyname[0] == '\0') {
		                rs = ew.loadkey(nnp,nnl) ;
	                    }
	                    if (ew.hash == 0) {
			        ew.hash = hash_elf(ew.keyname,-1) ;
			    }
			    if (rs >= 0) {
	                        if ((rs = ew.rd(ebuf,var.entsz)) >= 0) {
	                            ei = op->h.nentries ;
	                            rs = entbuf_write(op->ebmp,ei,ebuf) ;
		                }
			    } /* end if (ok) */
			} /* end if (ok) */
	            } /* end if (existing or new entry) */
		    rs1 = ew.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (tse) */
		/* update the file header-table (for a write) */
	        if ((rs >= 0) && op->fl.writable) {
	            op->h.wcount += 1 ;
	            op->h.wtime = uint(dt) ;
	            if (f_newentry) {
	                op->h.nentries += 1 ;
	                op->filesize += var.entsz ;
	            }
	            if ((rs = ts_headwrite(op)) >= 0) {
		        rs = entbuf_sync(op->ebmp) ;
	            }
	        } /* end if (updating header-table) */
	        /* optionally release our lock if no cursor outstanding */
	        if ((rs >= 0) && (op->ncursors == 0)) {
	    	    rs = ts_lockrel(op) ;
		}
		/* update access time as appropriate */
	        if (op->ncursors == 0) {
	            if (dt == 0) dt = getustime ;
	            op->tiaccess = dt ;
	        } else {
	            op->fl.cursoracc = true ;
	        }
	    } /* end if (ts_acquire) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_write) */

/* update an entry */
int ts_update(ts *op,time_t dt,cc *kp,int kl,ts_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = ts_magic(op,ep,kp)) >= 0) {
	    rs = SR_INVALID ;
	    if (kp[0]) {
		if (dt == 0) dt = getustime ;
		if ((rs = ts_acquire(op,dt,1)) >= 0) {
		    rs = ts_updater(op,dt,ep,kp,kl) ;
		    ei = rs ;
		} /* end if (rs_acquire) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_update) */

/* do some checking */
int ts_check(ts *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = ts_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        if ((! op->fl.lockedread) && (! op->fl.lockedwrite)) {
	            if (dt == 0) dt = getustime ;
	            f = ((dt - op->tiaccess) >= TO_ACCESS) ;
	            f = f || ((dt - op->tiopen) >= TO_OPEN) ;
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

static int ts_updater(ts *op,time_t dt,ts_ent *ep,cc *nnp,int nnl) noex {
    	int		rs ;
	int		rs1 ;
	int		ei = 0 ; /* return-value */
	bool		f_newentry = false ;
	if (tse ew ; (rs = ew.start) >= 0) {
	    if (char *bp ; (rs = ts_findname(op,nnp,nnl,&bp)) >= 0) {
	        ei = rs ;
	        /* update the entry that we found and write it back */
	        ew.wr(bp,var.entsz) ;
	        ew.utime = uint(dt) ;
	        ew.count += 1 ;
	        ew.rd(bp,var.entsz) ;
	        rs = entbuf_write(op->ebmp,ei,nullptr) ; /* sync */
	    } else if (rs == SR_NOTFOUND) {
		if ((rs = tsent_trans(ep,&ew)) >= 0) {
	            char	ebuf[var.entsz + 2] ;
	            f_newentry = true ;
	            if (ew.count == 0) ew.count = 1 ;
	            if (ew.utime == 0) ew.utime = uint(dt) ;
	            if (ew.ctime == 0) ew.ctime = uint(dt) ;
	            ew.hash = hash_elf(ew.keyname,-1) ;
	            ew.rd(ebuf,var.entsz) ;
	            ei = op->h.nentries ;
	            rs = entbuf_write(op->ebmp,ei,ebuf) ;
		} /* end if (tsent_trans) */
	    } /* end if (entry update) */
	    rs1 = ew.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (tse) */
	/* update the file header */
	if (rs >= 0) {
	    if (dt == 0) dt = getustime ;
	    op->h.wcount += 1 ;
	    op->h.wtime = uint(dt) ;
	    if (f_newentry) {
	        op->h.nentries += 1 ;
	        op->filesize += var.entsz ;
	    }
	    if ((rs = ts_headwrite(op)) >= 0) {
		rs = entbuf_sync(op->ebmp) ;
	    }
	} /* end if (updating header-table) */
	/* optionally release lock if we did not have a cursor outstanding */
	if (op->ncursors == 0) {
	    rs1 = ts_lockrel(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	/* update access time as appropriate */
	if (op->ncursors == 0) {
	    if (dt == 0) dt = getustime ;
	    op->tiaccess = dt ;
	} else {
	    op->fl.cursoracc = true ;
	}
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (rs_updater) */

static int ts_findname(ts *op,cchar *nnp,int nnl,char **rpp) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		ei = 0 ; /* return-value */
	char		*bp = nullptr ;
	if ((rs = mapstrint_fetch(op->nip,nnp,nnl,np,&ei)) >= 0) {
	    if ((rs = entbuf_read(op->ebmp,ei,&bp)) > 0) {
		if (tse ew ; (rs = ew.start) >= 0) {
		    if ((rs = ew.wr(bp)) >= 0) {
	                if (! namematch(ew.keyname,nnp,nnl)) {
	                    rs = SR_NOTFOUND ;
	                    mapstrint_delkey(op->nip,nnp,nnl) ;
	                } /* end if */
		    }
		    rs1 = ew.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (tse) */
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
	int		rs ;
	int		rs1 ;
	int		ne = 0 ; /* used-throughout */
	int		ei = 0 ; /* return-value */
	bool		f_found = false ;
	char		*bp = nullptr ;
	if (nnl < 0) nnl = lenstr(nnp) ;
	if (tse ew ; (rs = ew.start) >= 0) {
	    while ((rs >= 0) && (! f_found)) {
	        int	i ; /* used-afterwards */
	        rs = entbuf_read(op->ebmp,ei,&bp) ;
	        ne = rs ;
	        if (rs <= 0) break ;
	        for (i = 0 ; (rs >= 0) && (i < ne) ; i += 1) {
		    if ((rs = ew.wr(bp)) >= 0) {
	                f_found = namematch(ew.keyname,nnp,nnl) ;
		    }
		    if (f_found) break ;
	        } /* end for (looping through entries) */
	        ei += i ; /* from above (the 'for' loop) */
	        if (f_found) break ;
	    } /* end while */
	    rs1 = ew.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (tse) */
	if (rs >= 0) {
	    if ((ne != 0) && f_found) {
		if (rpp) {
		    *rpp = bp ;
		}
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (ok) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_search) */

static int ts_acquire(ts *op,time_t dt,int f_read) noex {
	int		rs ;
	int		f_changed = false ;
	if (dt == 0) dt = getustime ;
	if ((rs = ts_fileopen(op,dt)) >= 0) {
	    bool	f ;
	    if ((! f_read) && op->fl.lockedread) {
	        rs = ts_lockrel(op) ;
	    }
	    /* capture the lock if we do not already have it */
	    f = (op->fl.lockedread || op->fl.lockedwrite) ;
	    if ((rs >= 0) && (! f)) {
	        if ((rs = ts_lockacq(op,dt,f_read)) >= 0) {
	            f_changed = (rs > 0) ;
	            if ((rs = ts_filecheck(op,dt)) >= 0) {
	                f_changed = f_changed || (rs > 0) ;
	                if (f_changed) {
		            cint n = (op->filesize - toplen) / var.entsz ;
	                    rs = entbuf_invalidate(op->ebmp,n) ;
		        }
		    }
	        }
	    } /* end if (need lock) */
	} /* end if (ts_fileopen) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_acquire) */

/* initialize the file header (either read it only or write it) */
static int ts_filebegin(ts *op,time_t dt) noex {
	int		rs = SR_OK ;
	bool		f_locked = false ;
	if (op->filesize == 0) {
	    op->fl.fileinit = false ;
	    if (op->fl.writable) {
		if (op->fl.lockedread) {
		    rs = ts_lockrel(op) ;
		}
	        if ((rs >= 0) && (! op->fl.lockedwrite)) {
	            f_locked = true ;
	            rs = ts_lockacq(op,dt,0) ;
	        }
		if (rs >= 0) {
	            rs = ts_filetopwrite(op,dt) ;
	            f_locked = (rs > 0) ;
		}
	    } /* end if (writable) */
	} else if (op->filesize >= taboff) {
	    /* read the file header */
	    bool	f = (op->fl.lockedread || op->fl.lockedwrite) ;
	    if (! f) {
	        rs = ts_lockacq(op,dt,1) ;
	        f_locked = (rs >= 0) ;
	    }
	    if (rs >= 0) {
	        rs = ts_filetopread(op) ;
	    }
	    if (rs >= 0) {
	        rs = ts_fileverify(op) ;
	    }
	    if (rs >= 0) {
	        rs = ts_headtab(op,funmode::wr) ;
	        op->fl.fileinit = (rs >= 0) ;
	    }
	} /* end if */
	/* if we locked, we unlock it, otherwise leave it! */
	if (f_locked) {
	    ts_lockrel(op) ;
	}
	return rs ;
}
/* end subroutine (ts_filebegin) */

static int ts_filecheck(ts *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	if (op->filesize < taboff) {
	    f_changed = true ;
	    if (op->fl.writable) {
		rs = ts_filetopwrite(op,dt) ;
	    }
	} else {
	    if ((rs = ts_filetopread(op)) >= 0) {
	        if ((rs = ts_fileverify(op)) >= 0) {
	            if ((rs = ts_headtab(op,funmode::wr)) >= 0) {
	                op->fl.fileinit = true ;
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
	if ((rs = mkmagic(bp,maglen,magstr)) >= 0) {
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
	        bl = intconv(bp - op->topbuf) ;
	        op->vetu[vetu_version] = TS_FILEVERSION ;
	        op->vetu[vetu_type] = 0 ;
	        op->h = {} ;
	    }
	    if ((rs = u_pwrite(op->fd,op->topbuf,bl,poff)) >= 0) {
	        op->filesize = rs ;
	        op->topsize = rs ;
	        op->fl.fileinit = true ;
	    } /* end if (u_pwrite) */
	} /* end if (mkmagic) */
	return rs ;
}
/* end subroutine (ts_filetopwrite) */

static int ts_filetopread(ts *op) noex {
	coff		poff = 0z ;
	int		rs ;
	if ((rs = u_pread(op->fd,op->topbuf,toplen,poff)) >= 0) {
	    op->topsize = rs ;
	}
	return rs ;
}
/* end subroutine (ts_filetopread) */

static int ts_fileverify(ts *op) noex {
	static cint	magl = lenstr(magstr) ;
	cchar		*magp = magstr ;
	int		rs = SR_INVALID ;
	bool		f ;
	if (op->topsize >= toplen) {
	    cchar	*cp = op->topbuf ;
	    f = (strncmp(cp,magp,magl) == 0) ;
	    f = f && (*(cp + magl) == '\n') ;
	    rs = SR_BADFMT ;
	    if (f) {
	        rs = SR_NOTSUP ;
		cp += 16 ;
		if (cp[0] <= TS_FILEVERSION) {
		    op->FV = cp[0] ;
		    if (cp[1] == TS_ENDIAN) {
	    		rs = SR_OK ;
		    }
		    op->FT = cp[2] ;
	        } /* end if (supported) */
	    } /* end if (valid) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (ts_fileverify) */

/* read or write the file header */
static int ts_headtab(ts *op,funmode fc) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	char		*bp = (op->topbuf + TS_HEADTABOFF) ;
	switch (fc) {
    	using enum	funmode ;
	case wr:
	    {
	        ts_hdr	h{} ;
	        stdorder_rui(bp,&h.nentries) ;
	        bp += szof(uint) ;
	        stdorder_rui(bp,&h.wtime) ;
	        bp += szof(uint) ;
	        stdorder_rui(bp,&h.wcount) ;
	        bp += szof(uint) ;
	        f_changed = f_changed || (h.nentries != op->h.nentries) ;
	        f_changed = f_changed || (h.wtime != op->h.wtime) ;
	        f_changed = f_changed || (h.wcount != op->h.wcount) ;
	        op->h = h ;
	    } /* end block */
	    break ;
	case rd:
	    stdorder_wui(bp,op->h.nentries) ;
	    bp += szof(uint) ;
	    stdorder_wui(bp,op->h.wtime) ;
	    bp += szof(uint) ;
	    stdorder_wui(bp,op->h.wcount) ;
	    bp += szof(uint) ;
	    break ;
	default:
	    rs = SR_BUGCHECK ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_headtab) */

/* acquire access to the file */
static int ts_lockacq(ts *op,time_t dt,int f_read) noex {
	int		rs ;
	int		f_changed = false ;
	if ((rs = ts_fileopen(op,dt)) >= 0) {
	    int		lockcmd{} ;
	    bool	f_already = false ;
	    /* acquire a file record lock */
	    if (f_read || (! op->fl.writable)) {
	        f_already = op->fl.lockedread ;
	        op->fl.lockedread = true ;
	        op->fl.lockedwrite = false ;
	        lockcmd = F_RLOCK ;
	    } else {
	        f_already = op->fl.lockedwrite ;
	        op->fl.lockedread = false ;
	        op->fl.lockedwrite = true ;
	        lockcmd = F_WLOCK ;
	    } /* end if */
	    /* get out if we have the lock that we want already */
	    if (! f_already) {
		/* we need to actually do the lock */
		if_constexpr (f_lockf) {
	    	    rs = uc_lockf(op->fd,F_LOCK,0z) ;
		} else {
	    	    if_constexpr (f_solarisbug) {
	        	rs = lockfile(op->fd,lockcmd,0z,0z,TO_LOCK) ;
	    	    } else {
	        	coff	fs = op->filesize ;
	        	rs = lockfile(op->fd,lockcmd,0z,fs,TO_LOCK) ;
	    	    } /* end if_constexpr (f_solarisbug) */
		} /* end if_constexpr (f_lockf) */
		if (rs >= 0) {
		    /* has the file changed at all? */
	            if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
			f_changed = f_changed || (sb.st_size != op->filesize) ;
			f_changed = f_changed || (sb.st_mtime != op->timod) ;
		        if (f_changed) {
	    		    if (op->fl.bufvalid) {
				op->fl.bufvalid = false ;
			    }
	    		    op->filesize = intsat(sb.st_size) ;
	    		    op->timod = sb.st_mtime ;
			} /* end if */
	                if (op->filesize < taboff) {
	                    op->fl.fileinit = false ;
	                }
		    } /* end if (u_fstat) */
		} else {
	            op->fl.fileinit = false ;
	            if_constexpr (f_lockf) {
	                rs = uc_lockf(op->fd,F_ULOCK,0z) ;
	            } else {
	                if_constexpr (f_solarisbug) {
	                    rs = lockfile(op->fd,F_ULOCK,0z,0z,TO_LOCK) ;
	                } else {
	                    coff	fs = op->filesize ;
	                    rs = lockfile(op->fd,F_ULOCK,0z,fs,TO_LOCK) ;
	                } /* end if_constexpr (f_solarisbug) */
	            } /* end if_constexpr (f_lockf) */
	            op->fl.lockedread = false ;
	            op->fl.lockedwrite = false ;
		} /* end if (ok) */
	    } /* end if (was not already) */
	} /* end if (ts_fileopen) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_lockacq) */

static int ts_lockrel(ts *op) noex {
	int		rs = SR_OK ;
	if ((op->fl.lockedread || op->fl.lockedwrite)) {
	    if (op->fd >= 0) {
		cint	cmd = F_ULOCK ;
		if_constexpr (f_lockf) {
	            rs = uc_lockf(op->fd,cmd,0z) ;
	 	} else {
	            if_constexpr (f_solarisbug) {
	                rs = lockfile(op->fd,cmd,0z,0z,TO_LOCK) ;
	            } else {
		        coff	fs = op->filesize ;
	                rs = lockfile(op->fd,cmd,0z,fs,TO_LOCK) ;
		    } /* end if_constexpr (f_solarisbug) */
		} /* end if_constexpr (f_lockf) */
	    } /* end if (file was open) */
	    op->fl.lockedread = false ;
	    op->fl.lockedwrite = false ;
	} /* end if (there was a possible lock set) */
	return rs ;
}
/* end subroutine (ts_lockrel) */

static int ts_fileopen(ts *op,time_t dt) noex {
	int	rs = SR_OK ;
	int	f_created = false ;
	if (op->fd < 0) {
	    int		of = (op->oflags & (~ O_CREAT)) ;
	    op->fl.fileinit = false ;
	    op->fl.lockedread = false ;
	    op->fl.lockedwrite = false ;
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
	        op->tiopen = dt ;
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

static int ts_fileclose(ts *op) noex {
	int	rs = SR_OK ;
	int	rs1 ;
	if (op->fl.entbuf) {
	    rs1 = ts_ebuffinish(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->fd >= 0) {
	    op->fl.lockedread = false ;
	    op->fl.lockedwrite = false ;
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (ts_fileclose) */

static int ts_filesetinfo(ts *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		am = (op->oflags & O_ACCMODE) ;
	(void) dt ;
	op->fl.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	    op->timod = sb.st_mtime ;
	    op->filesize = intsat(sb.st_size) ;
	    if (char *fsbuf ; (rs = malloc_fs(&fsbuf)) >= 0) {
		cint	fslen = rs ;
	        if ((rs = getfstype(fsbuf,fslen,op->fd)) >= 0) {
		    cbool	f = (matlocalfs(fsbuf,rs) >= 0) ;
	            op->fl.remote = (! f) ; /* remote if not local! */
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
	if (! op->fl.entbuf) {
	    if (op->fd >= 0) {
		cuint	soff = taboff ;
		cint	esize = var.entsz ;
		cint	nways = TS_NWAYS ;
		cint	n = TS_NEPW ;
		rs = entbuf_start(op->ebmp,op->fd,soff,esize,nways,n) ;
		op->fl.entbuf = (rs >= 0) ;
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
	if (op->fl.entbuf) {
	    op->fl.entbuf = false ;
	    rs1 = entbuf_finish(op->ebmp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (ts_ebuffinish) */

static int ts_readentry(ts *op,int ei,char **rpp) noex {
	int		rs ;
	int		rs1 ;
	char		*bp = nullptr ; /* used-afterwards */
	if ((rs = entbuf_read(op->ebmp,ei,&bp)) > 0) {
	    if_constexpr (f_nienum) {
		if (tse ew ; (rs = ew.start) >= 0) {
		    if ((rs = ew.wr(bp)) >= 0) {
		        {
	                    cchar	*kp = ew.keyname ;
			    cint	kl = lenstr(ew.keyname) ;
	                    rs = ts_index(op,kp,kl,ei) ;
		        }
		    } /* end if (tse_wr) */
		    rs1 = ew.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (tse) */
	    } /* end if_constexpr (f_nienum) */
	} else if (rs == 0) {
	    rs = SR_NOTFOUND ;
	} /* end if (entbuf_read) */
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
	if (sl < 0) sl = lenstr(sp) ;
	if ((rs = mapstrint_fetch(op->nip,sp,sl,nullptr,&ei2)) >= 0) {
	    if (ei != ei2) {
	        if ((rs = mapstrint_delkey(op->nip,sp,sl)) >= 0) {
	            cint	nl2 = lenstr(sp,TSE_LKEYNAME) ;
	            rs = mapstrint_add(op->nip,sp,nl2,ei) ;
		}
	    }
	} else if (rs == rsn) {
	    cint	nl2 = lenstr(sp,TSE_LKEYNAME) ;
	    rs = mapstrint_add(op->nip,sp,nl2,ei) ;
	} /* end if (not found) */
	return rs ;
}
/* end subroutine (ts_index) */

static int ts_headwrite(ts *op) noex {
	int		rs ;
	int		headtaboff = TS_HEADTABOFF ;
	if ((rs = ts_headtab(op,funmode::rd)) >= 0) {
	    cchar	*bp = (op->topbuf + headtaboff) ;
	    cint	bl = (taboff - headtaboff) ;
	    coff	poff = headtaboff ;
	    rs = u_pwrite(op->fd,bp,bl,poff) ;
	}
	return rs ;
}
/* end subroutine (ts_headwrite) */

vars::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if (tse e ; (rs = e.start) >= 0) {
	    {
		var.entsz = e.entsz ;
	    }
	    rs1 = e.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (tse) */
    	return rs ;
}
/* end method (vars::operator) */

static int tsent_load(tsent *ep,cc *bp,char *rb,int rl) noex {
    	int		rs ;
	int		rs1 ;
	if (tse e ; (rs = e.start) >= 0) {
	    if ((rs = e.wr(bp,var.entsz)) >= 0) {
		ep->count = e.count ;
		ep->utime = e.utime ;
		ep->ctime = e.ctime ;
		if (rb) {
		    rs = sncpy(rb,rl,e.keyname) ;
		}
	    }
	    rs1 = e.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (tse) */
	return rs ;
}
/* end subroutine (tsent_load) */

static int tsent_trans(tsent *ep,tse *tep) noex {
    	int		rs = SR_FAULT ;
	if (tep) {
	    rs = SR_OK ;
	    tep->count = ep->count ;
	    tep->utime = ep->utime ;
	    tep->ctime = ep->ctime ;
	}
	return rs ;
}
/* end subroutine (tsent_trans) */

static bool namematch(cc *sp,cc *nnp,int nnl) noex {
	bool		f = false ;
	if (nnl <= TSE_LKEYNAME) {
	    f = (strncmp(sp,nnp,nnl) == 0) ;
	    f = f && (sp[nnl] == '\0') ;
	}
	return f ;
}
/* end subroutine (namematch) */


