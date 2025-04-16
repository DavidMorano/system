/* entbuf SUPPORT (Entry-Buffer) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage entry buffering of a file */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-10-22, David A­D­ Morano
	This code module is an attempt to make a better entry-buffer
	mamager than what was used in the old (quite old) SRVREG and
	MSGID objects.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	entbuf

	Description:
	This module manages the entry-buffering of a read-write
	file database.  We ignore what the entries actually are and
	just (really) cache them.  Our purpose is to try to avoid
	(or minimize) excess accesses to the actual underlying
	database file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<usystem.h>
#include	<intfloor.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"entbuf.h"


/* local defines */

#define	WAY	entbuf_way


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct oldentry {
	time_t		utime ;
	int		ei ;
} ;


/* forward references */

template<typename ... Args>
static inline int entbuf_magic(entbuf *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ENTBUF_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (entbuf_magic) */

static int	entbuf_waybegin(entbuf *,int) noex ;
static int	entbuf_wayend(entbuf *,int) noex ;
static int	entbuf_search(entbuf *,int,char **) noex ;
static int	entbuf_get(entbuf *,int,int,char **) noex ;
static int	entbuf_load(entbuf *,int,char **) noex ;
static int	entbuf_wayfindfin(entbuf *) noex ;
static int	entbuf_wayfindevict(entbuf *) noex ;
static int	entbuf_wayevict(entbuf *,int) noex ;
static int	entbuf_wayloadread(entbuf *,int,int,char **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int entbuf_start(entbuf *op,int fd,uint soff,int esize,int nways,int npw) noex {
    	ENTBUF		*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(hop) ;
	    if (nways < 0) nways = 1 ;
	    if (npw < ENTBUF_NENTS) npw = ENTBUF_NENTS ;
	    op->fd = fd ;
	    op->soff = soff ;
	    op->esize = esize ;
	    op->nways = nways ;
	    op->iways = 0 ;
	    op->npw = npw ;
	    if (USTAT sb ; (rs = u_fstat(fd,&sb)) >= 0) {
		csize	fsz = size_t(sb.st_size) ;
	        int	sz ;
	        if (uint foff = intsat(fsz) ; foff > soff) {
	            op->nentries = ((foff - soff) / esize) ;
	        }
	        sz = nways * szof(WAY) ;
	        if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		    op->ways = (WAY *) vp ;
		    memset(op->ways,0,sz) ;
		    op->magic = ENTBUF_MAGIC ;
	        } /* end if (m-a) */
	    } /* end if (fstat) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entbuf_start) */

int entbuf_finish(entbuf *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    for (int i = 0 ; i < op->nways ; i += 1) {
	        rs1 = entbuf_wayend(op,i) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    if (op->ways != nullptr) {
	        rs1 = uc_free(op->ways) ;
	        if (rs >= 0) rs = rs1 ;
	        op->ways = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entbuf_finish) */

int entbuf_read(entbuf *op,int ei,char **rpp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		n = 1 ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    if ((rs = entbuf_search(op,ei,rpp)) == rsn) {
	        rs = entbuf_load(op,ei,rpp) ;
	        n = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (entbuf_read) */

int entbuf_write(entbuf *op,int ei,cvoid *ubuf) noex {
	int		rs ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (ei >= 0) {
		char	*rp = nullptr ;
	        if ((rs = entbuf_search(op,ei,&rp)) >= 0) {
	            if (ubuf) {
	                memcpy(rp,ubuf,op->esize) ;
	            }
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	            if (ubuf) {
	                rp = charp(ubuf) ;
	            }
	        }
	        if (rs >= 0) {
	            if (ei < 0) ei = op->nentries ;
	            if (rp) {
		        coff	poff = op->soff + (ei * op->esize) ;
	                if ((rs = u_pwrite(op->fd,rp,op->esize,poff)) >= 0) {
		            if (ei >= op->nentries) {
		                op->nentries = (ei + 1) ;
		            }
	                }
	            } /* end if (result available) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entbuf_write) */

int entbuf_invalidate(entbuf *op,int n) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    for (int i = 0 ; (rs >= 0) && (i < op->iways) ; i += 1) {
	        rs1 = entbuf_wayevict(op,i) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    if ((rs >= 0) && (n >= 0)) {
	        op->nentries = n ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entbuf_invalidate) */

int entbuf_count(entbuf *op) noex {
	int		rs ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    rs = op->nentries ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entbuf_count) */

int entbuf_sync(entbuf *op) noex {
	int		rs ;
	if ((rs = entbuf_magic(op)) >= 0) {
	    rs = uc_fsync(op->fd) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entbuf_count) */


/* private subroutines */

static int entbuf_waybegin(entbuf *op,int wi) noex {
	int		rs ;
	WAY		*wp = (op->ways + wi) ;
	int		wsize = (op->npw * op->esize) ;
	if (void *vp ; (rs = uc_malloc(wsize,&vp)) >= 0) {
	    wp->wbuf = charp(vp) ;
	}
	return rs ;
}
/* end subroutine (entbuf_waybegin) */

static int entbuf_wayend(entbuf *op,int wi) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	WAY		*wp = (op->ways + wi) ;
	if (wp->wbuf != nullptr) {
	    rs1 = uc_free(wp->wbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    wp->wbuf = nullptr ;
	}
	memclear(wp) ;
	return rs ;
}
/* end subroutine (entbuf_wayend) */

static int entbuf_search(entbuf *op,int ei,char **rpp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_NOTFOUND ;
	for (int wi = 0 ; wi < op->iways ; wi += 1) {
	    rs = entbuf_get(op,wi,ei,rpp) ;
	    if ((rs >= 0) || (rs != rsn)) break ;
	} /* end if */
	if ((rs < 0) && rpp) {
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end subroutine (entbuf_search) */

static int entbuf_get(entbuf *op,int wi,int ei,char **rpp) noex {
	int		rs = SR_OK ;
	WAY		*wp = (op->ways + wi) ;
	uint	roff = 0 ;
	rs = SR_NOTFOUND ;
	if ((wp->wbuf != nullptr) && (wp->wlen > 0)) {
	    uint	eoff = (op->soff + (ei * op->esize)) ;
	    if ((eoff >= wp->woff) && (eoff < (wp->woff + wp->wlen))) {
		rs = SR_OK ;
		wp->utime = ++op->utimer ;
		roff = intconv(eoff - wp->woff) ;
	    }
	} /* end if */
	if (rpp) {
	    *rpp = (rs >= 0) ? (wp->wbuf + roff) : nullptr ;
	}
	return rs ;
}
/* end subroutine (entbuf_get) */

static int entbuf_load(entbuf *op,int ei,char **rpp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		n = 0 ;
	int		wi ;
	if ((rs = entbuf_wayfindfin(op)) >= 0) {
	    wi = rs ;
	} else if (rs == rsn) {
	    rs = entbuf_wayfindevict(op) ;
	    wi = rs ;
	} /* end if */
	if (rs >= 0) {
	    rs = entbuf_wayloadread(op,wi,ei,rpp) ;
	    n = rs ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (entbuf_load) */

static int entbuf_wayfindfin(entbuf *op) noex {
	int		rs = SR_OK ;
	int		wi = 0 ;
	WAY		*wp{} ;
	for (wi = 0 ; wi < op->iways ; wi += 1) {
	    wp = (op->ways + wi) ;
	    if (wp->wlen == 0) break ;
	} /* end if */
	if (wi >= op->iways) {
	    /* look for un-initialized and not used once yet */
	    if (op->iways >= op->nways) {
		/* look for un-initialized anywhere */
	        for (wi = 0 ; wi < op->iways ; wi += 1) {
	            wp = (op->ways + wi) ;
	            if (wp->wbuf == nullptr) break ;
	        } /* end if */
	        if (wi >= op->iways) {
	            rs = SR_NOTFOUND ;
	        }
	    } else {
	        wi = op->iways ;
	    }
	} /* end if */
	return (rs >= 0) ? wi : rs ;
}
/* end subroutine (entbuf_wayfindfin) */

static int entbuf_wayfindevict(entbuf *op) noex {
	WAY		*wp = nullptr ;
	uint		otime = INT_MAX ;
	int		rs = SR_OK ;
	int		wi = 0 ;
	for (int i = 0 ; i < op->nways ; i += 1) {
	    wp = (op->ways + i) ;
	    if (wp->utime < otime) {
		wi = i ;
		otime = wp->utime ;
	    }
	} /* end for */
	wp = (op->ways + wi) ;
	if (wp->wlen != 0) {
	    rs = entbuf_wayevict(op,wi) ;
	}
	return (rs >= 0) ? wi : rs ;
}
/* end subroutine (entbuf_wayfindevict) */

static int entbuf_wayevict(entbuf *op,int wi) noex {
	WAY		*wp = (op->ways + wi) ;
	int		rs = SR_OK ;
	wp->woff = 0 ;
	wp->wlen = 0 ;
	wp->nvalid = 0 ;
	wp->utime = 0 ;
	return rs ;
}
/* end subroutine (entbuf_wayevict) */

static int entbuf_wayloadread(entbuf *op,int wi,int ei,char **rpp) noex {
	WAY		*wp = (op->ways + wi) ;
	off_t		poff ;
	int		rs = SR_OK ;
	int		n = 0 ;
	if (wp->wbuf == nullptr) {
	    rs = entbuf_waybegin(op,wi) ;
	}
	if (rs >= 0) {
	    uint	woff = op->soff + (ei * op->esize) ;
	    int		wsize = (op->npw * op->esize) ;
	    poff = off_t(woff) ;
	    if ((rs = u_pread(op->fd,wp->wbuf,wsize,poff)) > 0) {
	        cint	len = rs ;
		n = (len / op->esize) ;
		wp->woff = woff ;
		wp->wlen = ifloor(len,op->esize) ;
		wp->utime = ++op->utimer ;
		wp->nvalid = n ;
		if (op->iways < op->nways) {
		    op->iways += 1 ;
		}
	    } /* end if (non-zero positive) */
	} /* end if (ok) */
	if (rpp) {
	    *rpp = (rs >= 0) ? wp->wbuf : nullptr ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (entbuf_wayloadread) */


