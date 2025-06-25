/* votdc HEADER (VOTD Cache management) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* VOTDs system Cache management */
/* version %I% last-modified %G% */

#define	CF_GETACOUNT	0		/* |votdc_getacount()| */
#define	CF_UPDATE	0		/* |votdc_update()| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This is really a new thing but with pieces borrowed from
	things from the way past.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	votdc

	Description:
	This module provides management for the system Verse-of-the-Day
	(VOTD) cache.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<climits>		/* |INT_MAX| + |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<endian.h>
#include	<intceil.h>
#include	<estrings.h>
#include	<filer.h>
#include	<storebuf.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<mkx.h>			/* |mkshmname(3uc)| */
#include	<sfx.h>
#include	<strwcpy.h>
#include	<matxstr.h>
#include	<cvtdater.h>
#include	<shmalloc.h>
#include	<localmisc.h>

#include	"votdc.h"
#include	"votdchdr.h"


/* local defines */

#define	VOTDC_OBJNAME		"votdc"
#define	VOTDC_SHMPOSTFIX	"votdc"
#define	VOTDC_PERMS		0666
#define	VOTDC_MUSIZE		szof(ptm)
#define	VOTDC_BOOKSIZE		(VOTDC_NBOOKS*szof(votdc_book))
#define	VOTDC_VERSESIZE		(VOTDC_NVERSES*szof(votdc_verse))
#define	VOTDC_MINSIZE		\
				VOTDC_BOOKSIZE+ \
				VOTDC_VERSESIZE+ \
				VOTDC_BSTRSIZE+ \
				VOTDC_VSTRSIZE+ \
				(2*szof(shmalloc))
#define	VOTDC_BOOKLEN		80 /* should match 'BIBLEBOOK_LEN' */

#ifndef	SHMPREFIXLEN
#define	SHMPREFIXLEN	8
#endif

#ifndef	SHMPOSTFIXLEN
#define	SHMPOSTFIXLEN	4
#endif

#ifndef	TO_WAITSHM
#define	TO_WAITSHM	30		/* seconds */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mode_t		m_t ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pagesize ;
	int		maxnamelen ;
	int		hdrbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}

/* forward references */

template<typename ... Args>
static int votdc_ctor(votdc *op,Args ... args) noex {
    	VOTDC		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (votdc_ctor) */

static int votdc_dtor(votdc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (votdc_dtor) */

template<typename ... Args>
static inline int votdc_magic(votdc *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VOTDC_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (votdc_magic) */

static int	votdc_shmhandbegin(votdc *,cchar *) noex ;
static int	votdc_shmhandend(votdc *) noex ;

static int	votdc_strbegin(votdc *,cchar *,cchar *) noex ;
static int	votdc_strend(votdc *) noex ;

static int	votdc_shmbegin(votdc *,int,mode_t) noex ;
static int	votdc_shmbeginer(votdc *,time_t,int,mode_t,int) noex ;
static int	votdc_shmend(votdc *) noex ;

static int	votdc_mapbegin(votdc *,time_t,int) noex ;
static int	votdc_mapend(votdc *) noex ;

static int	votdc_shmhdrin(votdc *,votdchdr *) noex ;
static int	votdc_shmprep(votdc *,time_t,int,mode_t,votdchdr *) noex ;
static int	votdc_shmpreper(votdc *,time_t,int,mode_t,votdchdr *) noex ;
static int	votdc_shmwriter(votdc *,time_t,int fd,votdchdr *,cc*,int) noex ;
static int	votdc_allocinit(votdc *,votdchdr *) noex ;
static int	votdc_mutexinit(votdc *) noex ;
static int	votdc_shmchown(votdc *) noex ;
static int	votdc_verify(votdc *) noex ;

static int	votdc_bookslotfind(votdc *,cchar *) noex ;
static int	votdc_bookslotload(votdc *,time_t,int,cchar *,cchar **) noex ;
static int	votdc_bookslotdump(votdc *,int) noex ;
static int	votdc_verselangdump(votdc *,int) noex ;

static int	votdc_booklanghave(votdc *,cchar *) noex ;
static int	votdc_versehave(votdc *,int,int) noex ;
static int	votdc_verseslotnext(votdc *,int) noex ;
static int	votdc_verseslotfind(votdc *) noex ;
static int	votdc_verseslotfinder(votdc *) noex ;

static int	votdc_mktitles(votdc *,cchar *,int) noex ;
static int	votdc_titlematcher(votdc *,int,int,cchar *,int) noex ;
static int	votdc_titlevalid(votdc *,int) noex ;
static int	votdc_titletouse(votdc *) noex ;
static int	votdc_access(votdc *) noex ;
static int	votdc_getwcount(votdc *) noex ;
static int	votdc_titlefins(votdc *) noex ;

#if	CF_GETACOUNT
static int	votdc_getacount(votdc *) noex ;
#endif /* CF_GETACOUNT */

#if	CF_UPDATE
static int	votdc_update(votdc *) noex ;
#endif /* CF_UPDATE */

static int	verse_dump(votdc_verse *,shmalloc *,int) noex ;
static int	verse_match(votdc_verse *,int,int) noex ;
static int	verse_read(votdc_verse *,char *,
			votdc_cite *,char *,int) noex ;
static int	verse_load(votdc_verse *,time_t,int,shmalloc *,char *,
			votdc_cite *,int,cchar *,int) noex ;
static int	verse_isempty(votdc_verse *) noex ;
static int	verse_isused(votdc_verse *) noex ;
static int	verse_isleast(votdc_verse *,time_t *) noex ;
static int	verse_accessed(votdc_verse *,time_t) noex ;

static int	book_load(votdc_book *,shmalloc *,char *,time_t,int,
			cchar *,cchar **) noex ;
static int	book_dump(votdc_book *,shmalloc *) noex ;
static int	book_getwmark(votdc_book *) noex ;
static int	book_getwmarklang(votdc_book *,cchar **) noex ;
static int	book_read(votdc_book *,char *,int,char *,int,int) noex ;

static int	titlecache_load(VOTDC_TC *,int,cchar *,char *,int *) noex ;
static int	titlecache_release(VOTDC_TC *) noex ;


/* local variables */

static vars		var ;


/* exported variables */

votdc_obj	votdc_modinfo = {
	VOTDC_OBJNAME,
	szof(votdc),
	0
} ;


/* exported subroutines */

int votdc_open(votdc *op,cchar *pr,cchar *lang,int of) noex {
	int		rs ;
	if ((lang == nullptr) || (lang[0] == '\0')) {
 	    lang = VOTDC_DEFLANG ;
	}
	if ((rs = votdc_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        static cint		rsv = var ;
	        if ((rs = rsv) >= 0) {
	            op->pagesize = var.pagesize ;
	            of &= (~ O_ACCMODE) ;
	            of |= O_RDWR ;
	            op->fd = -1 ;
	            if ((rs = votdc_shmhandbegin(op,pr)) >= 0) {
	                if ((rs = votdc_strbegin(op,pr,lang)) >= 0) {
		            cmode	om = VOTDC_PERMS ;
	                    if ((rs = votdc_shmbegin(op,of,om)) >= 0) {
	   	                op->magic = VOTDC_MAGIC ;
	                    } /* end if (votdc_shmbegin) */
	                    if (rs < 0) {
		                votdc_strend(op) ;
		            }
	                } /* end if (memory-allocation) */
	                if (rs < 0) {
		            votdc_shmhandend(op) ;
	                }
	            } /* end if (votdc-shmname) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0)  {
		votdc_dtor(op) ;
	    }
	} /* end if (votdc_ctor) */
	return rs ;
}
/* end subroutine (votdc_open) */

int votdc_close(votdc *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = votdc_magic(op)) >= 0) {
	    {
	        rs1 = votdc_titlefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = votdc_shmend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = votdc_strend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = votdc_shmhandend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = votdc_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (votdc_close) */

/* loads all book-titles with one call */
int votdc_titleloads(votdc *op,cchar *lang,cchar **tv) noex {
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;
	if (tv == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if (lang[0] == '\0') return SR_INVALID ;

	if (sigblocker s ; (rs = s.start) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		if ((rs = votdc_access(op)) >= 0) {
		    custime	dt = getustime ;
	            if ((rs = votdc_bookslotfind(op,lang)) >= 0) {
	    	        rs = votdc_bookslotload(op,dt,rs,lang,tv) ;
	            } /* end if (votdc_findslot) */
		} /* end if (votdc_access) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = s.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return rs ;
}
/* end subroutine (votdc_titleloads) */

/* do we have book-titles in a language? */
int votdc_titlelang(votdc *op,cchar *lang) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */

	if (op == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if (lang[0] == '\0') return SR_INVALID ;

	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        for (int bi = 0 ; bi < VOTDC_NBOOKS ; bi += 1) {
		    cchar	*blang = op->books[bi].lang ;
		    f = (strcasecmp(blang,lang) == 0) ;
		    if (f) break ;
	        } /* end for */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (votdc_titlelang) */

int votdc_titleget(votdc *op,char *rbuf,int rlen,int li,int ti) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if ((li < 0) || (li >= VOTDC_NBOOKS)) return SR_INVALID ;
	if ((ti < 0) || (ti >= VOTDC_NTITLES)) return SR_INVALID ;

	rbuf[0] = '\0' ;
	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		if ((rs = votdc_access(op)) >= 0) {
		    votdc_book	*bep = (op->books+li) ;
		    cint	ac = rs ;
		    char	*bstr = op->bstr ;
		    rs = book_read(bep,bstr,ac,rbuf,rlen,ti) ;
		    rl = rs ;
		} /* end if (votdc_access) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (votdc_titleget) */

int votdc_titlefetch(votdc *op,char *rbuf,int rlen,cchar *lang,int ti) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if (lang[0] == '\0') return SR_INVALID ;
	if ((ti < 0) || (ti >= VOTDC_NTITLES)) return SR_INVALID ;

	rbuf[0] = '\0' ;
	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		if ((rs = votdc_access(op)) >= 0) {
		    cint	ac = rs ;
	            int		bi ;
	            int		f = false ;
	            for (bi = 0 ; bi < VOTDC_NBOOKS ; bi += 1) {
		        cchar	*blang = op->books[bi].lang ;
		        f = (strcasecmp(blang,lang) == 0) ;
		        if (f) break ;
	            } /* end for */
	            if (f) {
			votdc_book	*bep = (op->books+bi) ;
			char		*bstr = op->bstr ;
			rs = book_read(bep,bstr,ac,rbuf,rlen,ti) ;
		        rl = rs ;
		    } else {
		        rs = SR_NOTFOUND ;
		    }
		} /* end if (votdc_access) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (votdc_titlefetch) */

int votdc_titlematch(votdc *op,cchar *lang,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		bi = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if (lang[0] == '\0') return SR_INVALID ;
	if (sp[0] == '\0') return SR_INVALID ;

	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		if ((rs = votdc_access(op)) >= 0) {
		    cint	n = int(op->hdr.booklen) ;
		    cint	ac = rs ;
	            int		f = false ;
	            cchar	*blang ;
	            for (bi = 0 ; bi < n ; bi += 1) {
		        blang = op->books[bi].lang ;
		        f = (strcasecmp(blang,lang) == 0) ;
		        if (f) break ;
	            } /* end for */
	            if (f) {
		        cint	clen = VOTDC_BOOKLEN ;
		        int	cl ;
		        char	cbuf[VOTDC_BOOKLEN+1] ;
			cint	tl = min(clen,sl) ;
		        cl = intconv(strwcpyopaque(cbuf,sp,tl) - cbuf) ;
		        if ((rs = votdc_mktitles(op,lang,bi)) >= 0) {
			    rs = votdc_titlematcher(op,ac,rs,cbuf,cl) ;
			    bi = rs ;
		        }
	            } else {
		        rs = SR_NOTFOUND ;
		    }
		} /* end if (votdc_access) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return (rs >= 0) ? bi : rs ;
}
/* end subroutine (votdc_titlematch) */

int votdc_versefetch(votdc *op,votdc_cite *citep,char *rbuf,int rlen,
			cchar *lang,int mjd) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if (op == nullptr) return SR_FAULT ;
	if (citep == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;
	if (lang[0] == '\0') return SR_INVALID ;
	if (mjd < 0) return SR_INVALID ;
	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if ((rs = votdc_booklanghave(op,lang)) >= 0) {
	            int	li = rs ;
		    if ((rs = votdc_versehave(op,li,mjd)) >= 0) {
		        votdc_verse	*vep = (op->verses+rs) ;
			char		*vstr = op->vstr ;
		        if ((rs = verse_read(vep,vstr,citep,rbuf,rlen)) >= 0) {
			    const time_t	dt = getustime ;
			    vl = rs ;
			    verse_accessed(vep,dt) ;
			}
		    } /* end if (votdc_versehave) */
	        } /* end if (votdc_booklanghave) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (votdc_versefetch) */


int votdc_verseload(votdc *op,cc *lang,votdc_cite *citep,int mjd,
		cc *vp,int vl) noex {
	int		rs ;
	int		rs1 ;
	if (op == nullptr) return SR_FAULT ;
	if (lang == nullptr) return SR_FAULT ;
	if (citep == nullptr) return SR_FAULT ;
	if (vp == nullptr) return SR_FAULT ;
	if (vp[0] == '\0') return SR_INVALID ;
	if (mjd < 0) return SR_INVALID ;
	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		if ((rs = votdc_access(op)) >= 0) {
		    cint	ac = rs ;
	            if ((rs = votdc_booklanghave(op,lang)) >= 0) {
		        shmalloc	*vap = op->vall ;
		        custime		dt = getustime ;
			cint		rsn = SR_NOTFOUND ;
	                int		li = rs ;
		        int		vi = -1 ;
		        char		*vstr = op->vstr ;
			citep->l = uchar(li) ;
		        if ((rs = votdc_versehave(op,li,mjd)) >= 0) {
			    votdc_verse	*vep = (op->verses+rs) ;
			    vi = rs ;
			    rs = verse_dump(vep,vap,li) ;
		        } else if (rs == rsn) {
			    rs = SR_OK ;
			}
		        if (rs >= 0) {
			    if (vi < 0) {
			        if ((rs = votdc_verseslotfind(op)) == rsn) {
				    if ((rs = votdc_verseslotfinder(op)) >= 0) {
			    		votdc_verse	*vep = (op->verses+rs) ;
			            	vi = rs ;
			    		rs = verse_dump(vep,vap,li) ;
				    }
			        } else {
			            vi = rs ;
				}
			    } /* end if (need slot) */
			    if (rs >= 0) {
			        votdc_verse	*vep = (op->verses+vi) ;
				votdc_cite	*cp = citep ;
		                rs = verse_load(vep,dt,ac,vap,vstr,
					cp,mjd,vp,vl) ;
				vl = rs ;
			    }
		        } /* end if (ok) */
	            } /* end if (votdc_booklanghave) */
		} /* end if (votdc_access) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (votdc_verseload) */

int votdc_getinfo(votdc *op,VOTDC_INFO *bip) noex {
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (bip == nullptr) return SR_FAULT ;

	if (op->magic != VOTDC_MAGIC) return SR_NOTOPEN ;

	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        votdchdr	*hdrp = &op->hdr ;
	        bip->wtime = (hdrp->wtime & UINT_MAX) ;
	        bip->atime = (hdrp->atime & UINT_MAX) ;
	        {
	            votdc_book	*vbp = (votdc_book *) op->books ;
		    cint	n = int(op->hdr.booklen) ;
		    int		c = 0 ;
	            for (int i = 0 ; i < n ; i += 1) {
		        if (vbp[i].b[0] != '\0') c += 1 ;
	            } /* end for */
	            bip->nbooks = c ;
	        }
	        {
	            votdc_verse	*vvp = (votdc_verse *) op->verses ;
		    cint	n = int(op->hdr.reclen) ;
		    int		c = 0 ;
	            for (int i = 0 ; i < n ; i += 1) {
		        if (vvp[i].voff != '\0') c += 1 ;
	            } /* end for */
	            bip->nverses = c ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return rs ;
}
/* end subroutine (votdc_getinfo) */

int votdc_vcurbegin(votdc *op,VOTDC_VCUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (votdc_vcurbegin) */

int votdc_vcurend(votdc *op,VOTDC_VCUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (votdc_vcurend) */

int votdc_vcurenum(votdc *op,VOTDC_VCUR *curp,votdc_cite *citep,
		char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (citep == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;
	if (sigblocker s ; (rs = sigblocker_start(&s,nullptr)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		cint	vi = (curp->i >= 0) ? (curp->i+1) : 0 ;
		if ((rs = votdc_verseslotnext(op,vi)) >= 0) {
		    votdc_verse	*vep = (op->verses+rs) ;
		    cint	ci = rs ;
		    char	*vstr = op->vstr ;
		    if ((rs = verse_read(vep,vstr,citep,rbuf,rlen)) >= 0) {
			vl = rs ;
			curp->i = ci ;
		    }
		} /* end if (votdc_verseslotnext) */
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = sigblocker_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (votdc_vcurenum) */


/* private subroutines */

static int votdc_strbegin(votdc *op,cchar *pr,cchar *lang) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (xstrlen(pr) + 1) ;
	sz += (xstrlen(lang) + 1) ;
	if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    op->lang = bp ;
	    bp = (strwcpy(bp,lang,-1)+1) ;
	}
	return rs ;
}
/* end subroutine (votdc_strbegin) */

static int votdc_strend(votdc *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	    op->pr = nullptr ;
	    op->lang = nullptr ;
	}

	return rs ;
}
/* end subroutine (votdc_strend) */

static int votdc_shmbegin(votdc *op,int of,mode_t om) noex {
	votdchdr	*hdrp = &op->hdr ;
	custime		dt = getustime ;
	cint		rsn = SR_NOENT ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	int		fninit = false ;
	cchar	*shmname = op->shmname ;

#ifdef	COMMENT
	of &= (~ O_ACCMODE) ;
	of |= O_RDWR ;
#endif /* COMMENT */

	memclear(hdrp) ;

	if (rs >= 0) {
	    cint	mof = (of & (~ O_CREAT)) ;
	    if ((rs = uc_openshm(shmname,mof,om)) == rsn) {
	        if (of & O_CREAT) {
		    cmode	mom = mode_t(om & 0444) ;
	            of |= O_EXCL ;
	            if ((rs = uc_openshm(shmname,of,mom)) >= 0) {
	                fd = rs ;
	                if ((rs = votdc_shmprep(op,dt,fd,om,hdrp)) >= 0) {
	                    fninit = true ;
		        }
	            } /* end if (uc_openshm) */
	        } /* end if (create mode) */
	    } else {
	        fd = rs ;
	    }
	} /* end if (uc_openshm) */

	if ((rs == SR_ACCESS) || (rs == SR_EXIST)) {
	    cint	to = TO_WAITSHM ;
	    op->shmsize = 0 ;
	    rs = uc_openshmto(shmname,of,om,to) ;
	    fd = rs ;
	    if ((rs == SR_TIMEDOUT) && (of & O_CREAT)) {
		if ((rs = uc_unlinkshm(shmname)) >= 0) {
	            if ((rs = uc_openshm(shmname,of,(om & 0444))) >= 0) {
	                fd = rs ;
	                if ((rs = votdc_shmprep(op,dt,fd,om,hdrp)) >= 0) {
	                    fninit = true ;
		        }
		    }
		} /* end if (uc_unlinkshm) */
	    } /* end if (timed-out) */
	} /* end if (waiting for file to be ready) */

	if (rs >= 0) { /* opened */
	    if ((rs = uc_fsize(fd)) > 0) {
	        op->shmsize = rs ;
		rs = votdc_shmbeginer(op,dt,fd,om,fninit) ;
	    } else if ((rs == 0) && (of & O_CREAT)) {
		if ((rs = votdc_shmprep(op,dt,fd,om,hdrp)) >= 0) {
	    	    if ((rs = uc_fsize(fd)) > 0) {
	                op->shmsize = rs ;
			rs = votdc_shmbeginer(op,dt,fd,om,true) ;
		    } else {
			rs = SR_LIBACC ;
		    }
		}
	    } /* end if (uc_fsize) */
	    if (rs < 0) {
		u_close(fd) ;
		op->fd = -1 ;
	    }
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (votdc_shmbegin) */

static int votdc_shmbeginer(votdc *op,time_t dt,int fd,m_t om,int fninit) noex {
	int		rs ;
	if ((rs = votdc_mapbegin(op,dt,fd)) >= 0) {
	    votdchdr	*hdrp = &op->hdr ;
	    if ((rs = votdc_shmhdrin(op,hdrp)) >= 0) {
		op->fd = fd ;
		if (fninit) {
		    if ((rs = votdc_allocinit(op,hdrp)) >= 0) {
			if ((rs = votdc_mutexinit(op)) >= 0) {
			    if ((rs = u_fchmod(fd,om)) >= 0) {
				rs = votdc_shmchown(op) ;
			    }
			}
		    }
		} /* end if (needed it) */
	    } /* end if (votdc_shmhdrin) */
	    if (rs < 0)
	        votdc_mapend(op) ;
	} /* end if (votdc_mapbegin) */
	return rs ;
}
/* end subroutine (votdc_shmbeginer) */

static int votdc_shmend(votdc *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    rs1 = votdc_mapend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (votdc_shmend) */

static int votdc_mapbegin(votdc *op,time_t dt,int fd) noex {
	int		rs = SR_INVALID ;
	if (op && (fd >= 0)) {
	    cnullptr	np{} ;
	    csize	ms = op->shmsize ;
	    cint	mp = (PROT_READ | PROT_WRITE) ;
	    cint	mf = MAP_SHARED ;
	    void	*md ;
	    if (dt == 0) dt = getustime ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	        op->mapdata = caddr_t(md) ;
	        op->mapsize = ms ;
	        op->ti_map = dt ;
	    } /* end if (map) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (votdc_mapbegin) */

static int votdc_mapend(votdc *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    caddr_t	md = op->mapdata ;
	    size_t	ms = op->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->mxp = nullptr ;
	    op->ti_map = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (votdc_mapend) */

static int votdc_shmprep(votdc *op,time_t dt,int fd,m_t om,
		votdchdr *hdrp) noex {
	int		rs ;
	int		foff = 0 ;

	op->shmsize = 0 ;
	if (dt == 0) dt = getustime ;

	hdrp->vetu[0] = VOTDCHDR_VERSION ;
	hdrp->vetu[1] = uchar(ENDIAN) ;
	hdrp->vetu[2] = 0 ;
	hdrp->vetu[3] = 0 ;
	hdrp->wtime = uint(dt) ;

	rs = votdc_shmpreper(op,dt,fd,om,hdrp) ;
	foff = rs ;

	return (rs >= 0) ? foff : rs ;
}
/* end subroutine (votdc_shmprep) */

static int votdc_shmpreper(votdc *op,time_t dt,int fd,mode_t om,
		votdchdr *hdrp) noex {
    	cint		hsz = (var.hdrbuflen + 1) ;
	int		rs ;
	int		foff = 0 ; /* return-value */
	if (char *hbuf ; (rs = uc_malloc(hsz,&hbuf)) >= 0) {
	    cint	hlen = rs ;
	    op->shmsize = 0 ;
	    if (dt == 0) dt = getustime ;
	    if ((rs = votdchdr_rd(hdrp,hbuf,hlen)) >= 0) {
	        int	hl = rs ;
	        if ((rs = votdc_shmwriter(op,dt,fd,hdrp,hbuf,hl)) >= 0) {
	            hdrp->shmsize = foff ;
	            if ((rs = u_rewind(fd)) >= 0) {
	                if ((rs = votdchdr_rd(hdrp,hbuf,hlen)) >= 0) {
	                    if ((rs = u_write(fd,hbuf,rs)) >= 0) {
	    		        op->shmsize = foff ;
	    		        rs = u_fchmod(fd,om) ;
		            }
		        } /* end if (votdchdr_rd) */
		    } /* end if (u_rewind) */
	        } /* end if (votdc_shmwriter) */
	    } /* end if (votdchdr_rd) */
	    rs = rsfree(rs,hbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? foff : rs ;
}
/* end subroutine (votdc_shmpreper) */

static int votdc_shmwriter(votdc *op,time_t dt,int fd,votdchdr *hdrp,
		cchar *hbuf,int hlen) noex {
	filer		sfile, *sfp = &sfile ;
	cint		bsz = 2048 ;
	int		rs ;
	int		rs1 ;
	int		size ;
	int		foff = 0 ;

	op->shmsize = 0 ;
	if (dt == 0) dt = getustime ;

	if ((rs = filer_start(sfp,fd,0,bsz,0)) >= 0) {
	    cint	asize = SHMALLOC_ALIGNSIZE ;

	    if (rs >= 0) {
	        rs = filer_write(sfp,hbuf,hlen) ;
	        foff += rs ;
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = VOTDC_MUSIZE ;
		    hdrp->muoff = foff ;
		    hdrp->musize = size ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = VOTDC_BOOKSIZE ;
		    hdrp->bookoff = foff ;
		    hdrp->booklen = VOTDC_NBOOKS ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = VOTDC_VERSESIZE ;
		    hdrp->recoff = foff ;
		    hdrp->reclen = VOTDC_NVERSES ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = szof(shmalloc) ;
		    hdrp->balloff = foff ;
		    hdrp->ballsize = size ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = szof(shmalloc) ;
		    hdrp->valloff = foff ;
		    hdrp->vallsize = size ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = VOTDC_BSTRSIZE ;
		    hdrp->bstroff = foff ;
		    hdrp->bstrlen = size ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = filer_writealign(sfp,asize)) >= 0) {
	            foff += rs ;
		    size = VOTDC_VSTRSIZE ;
		    hdrp->vstroff = foff ;
		    hdrp->vstrlen = size ;
		    rs = filer_writezero(sfp,size) ;
	            foff += rs ;
	        }
	    }

	    if (rs >= 0) {
		size = iceil(foff,op->pagesize) - foff ;
		rs = filer_writezero(sfp,size) ;
	        foff += rs ;
	    }

	    rs1 = filer_finish(sfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */

	hdrp->shmsize = foff ;
	return (rs >= 0) ? foff : rs ;
}
/* end subroutine (votdc_shmwriter) */

static int votdc_shmhdrin(votdc *op,votdchdr *hdrp) noex {
	int		rs ;
	cint		hsz = int(op->mapsize) ;
	if ((rs = votdchdr_wr(hdrp,op->mapdata,hsz)) >= 0) {
	    if ((rs = votdc_verify(op)) >= 0) {
	        op->mxp = (ptm *) (op->mapdata + hdrp->muoff) ;
	        op->books = (votdc_book *) (op->mapdata + hdrp->bookoff) ;
	        op->verses = (votdc_verse *) (op->mapdata + hdrp->recoff) ;
	        op->ball = (shmalloc *) (op->mapdata + hdrp->balloff) ;
	        op->vall = (shmalloc *) (op->mapdata + hdrp->valloff) ;
	        op->bstr = charp(op->mapdata + hdrp->bstroff) ;
	        op->vstr = charp(op->mapdata + hdrp->vstroff) ;
		op->nents = int(hdrp->reclen) ;
	    }
	} /* end if (votdchdr_wr) */
	return rs ;
}
/* end subroutine (votdc_shmhdrin) */

static int votdc_allocinit(votdc *op,votdchdr *hdrp) noex {
	int		rs ;
	if ((rs = shmalloc_init(op->ball,op->bstr,hdrp->bstrlen)) >= 0) {
	    rs = shmalloc_init(op->vall,op->vstr,hdrp->vstrlen) ;
	    if (rs < 0) {
	        shmalloc_fini(op->ball) ;
	    }
	} /* end if (shmalloc initialization) */
	return rs ;
}
/* end subroutine (votdc_allocinit) */

static int votdc_mutexinit(votdc *op) noex {
	int		rs ;
	int		rs1 ;
	if (ptma ma ; (rs = ptma_create(&ma)) >= 0) {
	    cint	cmd = PTHREAD_PROCESS_SHARED ;
	    if ((rs = ptma_setpshared(&ma,cmd)) >= 0) {
	        ptm	*mp = (ptm *) op->mxp ;

#ifdef	OPTIONAL
	        memclear(mp) ;
#endif

	        rs = ptm_create(mp,&ma) ; /* we leave the MUTEX initialized */

	    } /* end if (ptma_setpshared) */
	    rs1 = ptma_destroy(&ma) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-lock attribute) */
	return rs ;
}
/* end subroutine (votdc_mutexinit) */

static int votdc_shmchown(votdc *op) noex {
	int		rs ;
	int		rs1 ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    cint	pwlen = rs ;
	    cchar	*cp ;
	    if (int cl ; (cl = sfrootname(op->pr,-1,&cp)) > 0) {
	        cint	ulen = USERNAMELEN ;
	        char	ubuf[USERNAMELEN+1] ;
		strdcpy1w(ubuf,ulen,cp,cl) ;
	 	if (ucentpw pw ; (rs = pw.getnam(pwbuf,pwlen,ubuf)) >= 0) {
		    const uid_t		uid = pw.pw_uid ;
		    u_fchown(op->fd,uid,-1) ;
	        } else if (rs == SR_NOENT) {
		    rs = SR_OK ;
		}
	    } /* end if (sfrootname) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (votdc_shmchown) */

static int votdc_verify(votdc *op) noex {
	votdchdr	*hdrp = &op->hdr ;
	uint		ushmsz = int(op->shmsize) ;;
	int		rs = SR_OK ;
	int		sz ;
	bool		f = true ;
	f = f && (hdrp->muoff > 0) ;
	f = f && (hdrp->muoff < ushmsz) ;
	f = f && (hdrp->bookoff > 0) ;
	f = f && (hdrp->bookoff < ushmsz) ;
	f = f && (hdrp->recoff > 0) ;
	f = f && (hdrp->recoff < ushmsz) ;
	f = f && (hdrp->balloff > 0) ;
	f = f && (hdrp->balloff < ushmsz) ;
	f = f && (hdrp->valloff > 0) ;
	f = f && (hdrp->valloff < ushmsz) ;
	f = f && (hdrp->bstroff > 0) ;
	f = f && (hdrp->bstroff < ushmsz) ;
	f = f && (hdrp->vstroff > 0) ;
	f = f && (hdrp->vstroff < ushmsz) ;
	f = f && ((hdrp->muoff + hdrp->musize) < ushmsz) ;
	sz = szof(votdc_book) ;
	f = f && ((hdrp->bookoff + (hdrp->booklen * sz)) < ushmsz) ;
	sz = szof(votdc_verse) ;
	f = f && ((hdrp->recoff + (hdrp->reclen * sz)) < ushmsz) ;
	f = f && ((hdrp->balloff + hdrp->ballsize) < ushmsz) ;
	f = f && ((hdrp->valloff + hdrp->vallsize) < ushmsz) ;
	f = f && ((hdrp->bstroff + hdrp->bstrlen) < ushmsz) ;
	f = f && ((hdrp->vstroff + hdrp->vstrlen) < ushmsz) ;
	if (! f) rs = SR_BADFMT ;
	return rs ;
}
/* end subroutine (votdc_verify) */

static int votdc_shmhandbegin(votdc *op,cchar *pr) noex {
	int		rs = SR_OK ;
	cchar		*rn ;
	if (int rl ; (rl = sfrootname(pr,-1,&rn)) > 0) {
	    cchar	*suf = VOTDC_SHMPOSTFIX ;
	    char	rbuf[MAXNAMELEN+1] ;
	    if ((rs = mkshmname(rbuf,rn,rl,suf)) >= 0) {
	        if (cchar *cp ; (rs = uc_mallocstrw(rbuf,rs,&cp)) >= 0) {
	            op->shmname = cp ;
	        }
	    } /* end if (mkourname) */
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
}
/* end subroutine (votdc_shmhandbegin) */

static int votdc_shmhandend(votdc *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->shmname != nullptr) {
	    rs1 = uc_free(op->shmname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->shmname = nullptr ;
	}
	return rs ;
}
/* end subroutine (votdc_shmhandend) */

static int votdc_bookslotload(votdc *op,time_t dt,int si,cc *lang,
		cc **tv) noex {
	int		rs ;
	if ((rs = votdc_getwcount(op)) >= 0) {
	    votdc_book	*blp = (op->books + si) ;
	    shmalloc	*bap = op->ball ;
	    cint	wc = rs ;
	    rs = book_load(blp,bap,op->bstr,dt,wc,lang,tv) ;
	}
	return rs ;
}
/* end subroutine (votdc_bookslotload) */

static int votdc_bookslotfind(votdc *op,cchar *lang) noex {
	votdc_book	*blp = op->books ;
	cint		n = int(op->hdr.booklen) ;
	int		rs = SR_OK ;
	int		idx = 0 ; /* return-value */
	int		i_empty = -1 ;
	bool		f_same = false ;
	for (int i = 0 ; i < n ; i += 1) {
	    cchar	*elang = blp[i].lang ;
	    if (elang[0] != '\0') {
		f_same = (strcmp(elang,lang) == 0) ;
		idx = i ;
	    } else {
		i_empty = i ;
	    }
	    if (f_same) break ;
	} /* end for */
	if (f_same) {
	    rs = votdc_bookslotdump(op,idx) ;
	} else {
	    if (i_empty < 0) {
	        time_t	oldest = INT_MAX ;
	        int	oi = 0 ;
	        for (int i = 0 ; i < n ; i += 1) {
	            if (blp[i].atime < oldest) {
		        oldest = op->books[i].atime ;
		        oi = i ;
		    }
	        } /* end for */
	        rs = votdc_bookslotdump(op,oi) ;
		idx = oi ;
	    } else {
		idx = i_empty ;
	    }
	} /* end if (same or not) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (votdc_bookslotfind) */

static int votdc_bookslotdump(votdc *op,int ei) noex {
	int		rs ;
	if ((rs = votdc_verselangdump(op,ei)) >= 0) {
	    votdc_book	*blp = (op->books + ei) ;
	    shmalloc	*bap = op->ball ;
	    rs = book_dump(blp,bap) ;
	} /* end if (votdc_verselangdump) */

	return rs ;
}
/* end subroutine (votdc_bookslotdump) */

static int votdc_verselangdump(votdc *op,int li) noex {
	votdc_verse	*vep = op->verses ;
	shmalloc	*vap = op->vall ;
	cint		n = int(op->hdr.reclen) ;
	int		rs = SR_OK ;
	for (int i = 0 ; i < n ; i += 1) {
	    rs = verse_dump((vep+i),vap,li) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (votdc_verselangdump) */

static int votdc_booklanghave(votdc *op,cchar *lang) noex {
	votdc_book	*bap = op->books ;
	cint		n = int(op->hdr.booklen) ;
	int		rs = SR_NOTFOUND ;
	int		i ; /* used-afterwards return-value */
	for (i = 0 ; i < n ; i += 1) {
	    if (strcmp(bap[i].lang,lang) == 0) {
		rs = SR_OK ;
		break ;
	    }
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (votdc_booklanghave) */

static int votdc_versehave(votdc *op,int li,int mjd) noex {
	votdc_verse	*vep = op->verses ;
	cint		n = int(op->hdr.reclen) ;
	int		rs = SR_NOTFOUND ;
	int		i ; /* used-afterwards return-value */
	for (i = 0 ; i < n ; i += 1) {
	    rs = verse_match((vep+i),li,mjd) ;
	    if (rs != SR_NOTFOUND) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (votdc_versehave) */

static int votdc_verseslotnext(votdc *op,int vi) noex {
	votdc_verse	*vep = op->verses ;
	cint		n = int(op->hdr.reclen) ;
	int		rs = SR_NOTFOUND ;
	int		i ; /* used-afterwards */
	for (i = vi ; i < n ; i += 1) {
	    rs = verse_isused(vep+i) ;
	    if (rs != SR_NOTFOUND) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (votdc_verseslotnext) */

static int votdc_verseslotfind(votdc *op) noex {
	votdc_verse	*vep = op->verses ;
	cint		n = int(op->hdr.reclen) ;
	int		rs = SR_NOTFOUND ;
	int		i ; /* used-afterwards return-value */
	for (i = 0 ; i < n ; i += 1) {
	    rs = verse_isempty((vep+i)) ;
	    if (rs != SR_NOTFOUND) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (votdc_verseslotfind) */

static int votdc_verseslotfinder(votdc *op) noex {
	votdc_verse	*vep = op->verses ;
	time_t		mt = INT_MAX ;
	cint		n = int(op->hdr.reclen) ;
	int		rs = SR_OK ;
	int		mi = 0 ; /* return-value */
	for (int i = 0 ; i < n ; i += 1) {
	    if ((rs = verse_isleast((vep+i),&mt)) > 0) {
		mi = i ;
	    } /* end if (verse_isleast) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? mi : rs ;
}
/* end subroutine (votdc_verseslotfinder) */

static int votdc_getwcount(votdc *op) noex {
	int		rs ;
	char		*mdp = (char *) op->mapdata ;
	int		*htab ;
	int		*wcp ;
	htab = intp(mdp + VOTDCHDR_IDLEN) ;
	wcp = intp(htab + votdchdrh_wcount) ;
	rs = *wcp ;
	return rs ;
}
/* end subroutine (votdc_getwcount) */

static int votdc_access(votdc *op) noex {
	uint		*htab ;
	int		rs ;
	char		*mdp = charp(op->mapdata) ;
	htab = (uint *) (mdp + VOTDCHDR_IDLEN) ;
	{
	    int	*acp = intp(htab + votdchdrh_acount) ;
	    *acp += 1 ;
	    rs = *acp ;
	}
	return rs ;
}
/* end subroutine (votdc_access) */

#if	CF_UPDATE
static int votdc_update(votdc *op) noex {
	uint		*htab ;
	int		rs ;
	char		*mdp = (char *) op->mapdata ;
	htab = (uint *) (mdp + VOTDCHDR_IDLEN) ;
	{
	    int	*wcp = intp(htab + votdchdrh_wcount) ;
	    rs = *wcp ;
	    *wcp += 1 ;
	}
	return rs ;
}
/* end subroutine (votdc_update) */
#endif /* CF_UPDATE */

#if	CF_GETACOUNT
static int votdc_getacount(votdc *op) noex {
	uint		*htab ;
	int		rs ;
	int		*acp ;
	char		*mdp = (char *) op->mapdata ;
	htab = uintp(mdp + VOTDCHDR_IDLEN) ;
	acp = intp(htab + votdchdrh_acount) ;
	rs = *acp ;
	return rs ;
}
/* end subroutine (votdc_getacount) */
#endif /* CF_GETACOUNT */

static int votdc_mktitles(votdc *op,cchar *lang,int bi) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		ci = 0 ;
	if ((rs = votdc_titlevalid(op,bi)) == rsn) {
	    if ((rs = votdc_titletouse(op)) >= 0) {
	        VOTDC_TC	*tcp = (op->tcs+rs) ;
		votdc_book	*bep = (op->books+bi) ;
		ci = rs ;
		if ((rs = book_getwmark(bep)) >= 0) {
		    cint	wm = rs ;
		    char	*bstr = op->bstr ;
		    rs = titlecache_load(tcp,wm,lang,bstr,bep->b) ;
		} /* end if (book_getwmark) */
	    } /* end if (votdc_titletouse) */
	} else {
	    ci = rs ;
	}
	return (rs >= 0) ? ci : rs ;
}
/* end subroutine (votdc_mktitles) */

static int votdc_titlematcher(votdc *op,int ac,int ci,cc *cbuf,int cl) noex {
	VOTDC_TC	*tcp = (op->tcs+ci) ;
	int		rs = SR_NOTFOUND ;
	if (tcp->titles != nullptr) {
	    tcp->amark = ac ;
	    if ((rs = matostr(tcp->titles,2,cbuf,cl)) < 0) {
		rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (votdc_titlematcher) */

static int votdc_titletouse(votdc *op) noex {
	VOTDC_TC	*tcp = op->tcs ;
	int		rs = SR_OK ;
	int		ci = -1 ;
	for (int i = 0 ; i < VOTDC_NTITLES ; i += 1) {
	    if (tcp[i].lang[0] == '\0') {
		ci = i ;
		break ;
	    }
	} /* end for */
	if (ci < 0) {
	    int		minamark = INT_MAX ;
	    for (int i = 0 ; i < VOTDC_NTITLES ; i += 1) {
	        if ((tcp[i].lang[0] != '\0') && (tcp[i].amark < minamark)) {
		    ci = i ;
		    minamark = tcp[i].amark ;
	        }
	    } /* end for */
	} /* end if (find oldest) */
	return (rs >= 0) ? ci : rs ;
}
/* end subroutine (votdc_titletouse) */

static int votdc_titlevalid(votdc *op,int bi) noex {
	votdc_book	*bep = (op->books+bi) ;
	int		rs ;
	int		i = 0 ; /* return-value */
	cchar		*blang ;
	if ((rs = book_getwmarklang(bep,&blang)) >= 0) {
	    VOTDC_TC	*tcp = op->tcs ;
	    cint	wm = rs ;
	    cint	n = VOTDC_NBOOKS ;
	    bool	f = false ;
	    for (i = 0 ; i < n ; i += 1) {
		if ((tcp[i].lang[0] != '\0') && (tcp[i].wmark == wm)) {
		    f = (strcmp(tcp[i].lang,blang) == 0) ;
		    if (f) break ;
		}
	    } /* end for */
	    if (!f) rs = SR_NOTFOUND ;
	} /* end if (votdc_getwcount) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (votdc_titlevalid) */

static int votdc_titlefins(votdc *op) noex {
	cint		n = VOTDC_NBOOKS ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < n ; i += 1) {
	    rs1 = titlecache_release((op->tcs+i)) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */
	return rs ;
}
/* end subroutine (votdc_titlefins) */

static int verse_dump(votdc_verse *vep,shmalloc *vap,int li) noex {
	int		rs = SR_OK ;
	li &= UCHAR_MAX ;
	if ((vep->lang == li) && (vep->vlen > 0)) {
	    vep->mjd = 0 ;
	    if ((rs = shmalloc_free(vap,vep->voff)) >= 0) {
	        vep->voff = 0 ;
	        vep->vlen = 0 ;
	        vep->ctime = 0 ;
	        vep->atime = 0 ;
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (verse_dump) */

static int verse_match(votdc_verse *vep,int li,int mjd) noex {
	int		rs = SR_NOTFOUND ;
	if ((vep->lang == li) && (vep->mjd == mjd)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (verse_match) */

static int verse_read(votdc_verse *vep,char *vstr,votdc_cite *citep,
		char *rbuf,int rlen) noex {
	int		rs ;
	citep->b = vep->book ;
	citep->c = vep->chapter ;
	citep->v = vep->verse ;
	citep->l = vep->lang ;
	rs = sncpy1(rbuf,rlen,(vstr+vep->voff)) ;
	return rs ;
}
/* end subroutine (verse_read) */

static int verse_accessed(votdc_verse *vep,time_t dt) noex {
	int		rs = SR_OK ;
	vep->atime = dt ;
	return rs ;
}
/* end subroutine (verse_accessed) */

static int verse_load(votdc_verse *vep,time_t dt,int wm,shmalloc *vap,
		char *vstr,votdc_cite *citep,int mjd,cc *valp,int vall) noex {
	int		rs ;
	int		voff = 0 ;
	if (vall < 0) vall = xstrlen(valp) ;
	if ((rs = shmalloc_alloc(vap,(vall + 1))) >= 0) {
	    char	*bp = (vstr + rs) ;
	    voff = rs ;
 	    vep->ctime = dt ;
	    vep->atime = dt ;
	    vep->voff = voff ;
	    vep->vlen = vall ;
	    vep->wmark = wm ;
	    vep->amark = 0 ;
	    vep->book  = citep->b ;
	    vep->chapter = citep->c ;
	    vep->verse = citep->v ;
	    vep->lang = citep->l ;
	    vep->mjd = mjd ;
	    strwcpy(bp,valp,vall) ;
	} /* end if (m-a) */
	return (rs >= 0) ? voff : rs ;
}
/* end subroutine (verse_load) */

static int verse_isempty(votdc_verse *vep) noex {
	int		rs = SR_OK ;
	if (vep->mjd > 0) rs = SR_NOTFOUND ;
	return rs ;
}
/* end subroutine (verse_isempty) */

static int verse_isused(votdc_verse *vep) noex {
	int		rs = SR_OK ;
	if (vep->mjd == 0) rs = SR_NOTFOUND ;
	return rs ;
}
/* end subroutine (verse_isused) */

static int verse_isleast(votdc_verse *vep,time_t *tp) noex {
	int		rs = 0 ;
	if ((vep->mjd == 0) || (vep->atime < *tp)) {
	    rs = 1 ;
	    *tp = vep->atime ;
	}
	return rs ;
}
/* end subroutine (verse_isleast) */

static int book_load(votdc_book *blp,shmalloc *bap,char *bsp,time_t dt,
		int wm,cchar *lang,cchar **tv) noex {
    	cint		n = VOTDC_NTITLES ;
	int		rs = SR_OK ;
	char		*bp ;
	strwcpy(blp->lang,lang,VOTDC_LANGLEN) ;
	blp->ctime = dt ;
	blp->atime = dt ;
	blp->wmark = wm ;
	for (int i = 0 ; (i < n) && tv[i] ; i += 1) {
	    cint	bl = xstrlen(tv[i]) ;
	    if ((rs = shmalloc_alloc(bap,(bl+1))) >= 0) {
		blp->b[i] = rs ;
		bp = (bsp+rs) ;
		strwcpy(bp,tv[i],bl) ;
	    } /* end if (shmalloc_alloc) */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (book_load) */

static int book_dump(votdc_book *blp,shmalloc *bap) noex {
    	cint		n = VOTDC_NTITLES ;
	int		rs = SR_OK ;
	blp->ctime = 0 ;
	blp->atime = 0 ;
	blp->lang[0] = '\0' ;
	for (int i = 0 ; i < n ; i += 1) {
	    int	toff = blp->b[i] ;
	    if (toff > 0) {
	        rs = shmalloc_free(bap,toff) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (book_dump) */

static int book_getwmark(votdc_book *bep) noex {
	int		rs = SR_NOTFOUND ;
	if (bep->lang[0] != '\0') {
	    rs = bep->wmark ;
	}
	return rs ;
}
/* end subroutine (book_getwmark) */

static int book_getwmarklang(votdc_book *bep,cchar **lpp) noex {
	int		rs = SR_NOTFOUND ;
	if (bep->lang[0] != '\0') {
	    if (lpp != nullptr) *lpp = bep->lang ;
	    rs = bep->wmark ;
	}
	return rs ;
}
/* end subroutine (book_getwmarklang) */

static int book_read(votdc_book *bep,char *bstr,int ac,char *rbuf,int rlen,
		int ti) noex {
	cint		boff = bep->b[ti] ;
	int		rs ;
	bep->amark = ac ;
	rs = sncpy1(rbuf,rlen,(bstr+boff)) ;
	return rs ;
}
/* end subroutine (book_read) */

static int titlecache_load(VOTDC_TC *tcp,int wm,cc *lang,
		char *bstr,int *boffs) noex {
	cint		n = VOTDC_NTITLES ;
	int		rs ;
	int		sz = szof(cchar *) ;
	cchar		*cp ;
	memclear(tcp) ;
	tcp->wmark = wm ;
	tcp->amark = 0 ;
	strwcpy(tcp->lang,lang,VOTDC_LANGLEN) ;
	for (int i = 0 ; i < n ; i += 1) {
	    cp = (bstr + boffs[i]) ;
	    sz += szof(cchar *) ;
	    sz += (xstrlen(cp) + 1) ;
	} /* end for */
	if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    int		i ; /* used-afterwards */
	    tcp->a = ccharp(bp) ;
	    tcp->titles = ccharpp(bp) ;
	    bp += ((n + 1) * szof(cchar *)) ;
	    for (i = 0 ; i < n ; i += 1) {
		cp = (bstr + boffs[i]) ;
		tcp->titles[i] = cp ;
	    } /* end for */
	    tcp->titles[i] = nullptr ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (titlecache_load) */

static int titlecache_release(VOTDC_TC *tcp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tcp->a != nullptr) {
	    rs1 = uc_free(tcp->a) ;
	    if (rs >= 0) rs = rs1 ;
	    tcp->a = nullptr ;
	    tcp->titles = nullptr ;
	}
	tcp->lang[0] = '\0' ;
	tcp->wmark = 0 ;
	tcp->amark = 0 ;
	return rs ;
}
/* end subroutine (titlecache_release) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = ucpagesize) >= 0) {
	    pagesize = rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
		maxnamelen = rs ;
		hdrbuflen = (szof(votdchdr) + maxnamelen) ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */


