/* ustream_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-07-09, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	ustream

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>		/* |SEEK_{xx}| */
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usyscalls.h>
#include	<funcodes.h>		/* |FM_{x}(3uc)| */
#include	<intfloor.h>
#include	<fmtstr.h>
#include	<localmisc.h>
#include	<dprintf.hh>		/* debugging */

#include	"ustream.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |memcopy(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

/* local defines */

#define	MEMCPYLEN	100

#define	PIPEBUFLEN	1024
#define	BLOCKBUFLEN	512

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))

#ifndef	CF_DEBUG
#define	CF_DEBUG	0	/* debugging */
#endif


/* imported namespaces */

using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */
using libu::uread ;		/* subroutine */
using libu::ureade ;		/* subroutine */
using libu::umem ;		/* variable */
using ustream_ns::ustream_reserve ;
using ustream_ns::ustream_flush ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int ustream_ctor(ustream *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if ((args && ...)) ylikely {
	    rs = SR_OK ;
	    op->magval = 0 ;
	    op->foff = 0 ;
	    op->dbuf = nullptr ;
	    op->bptr = nullptr ;
	    op->fl = {} ;
	    op->fd = 0 ;
	    op->oflags = 0 ;
	    op->dt = 0 ;
	    op->dlen = 0 ;
	    op->blen = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subriutine (ustream_ctor) */

local int ustream_dtor(ustream *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->dbuf = nullptr ;
	    op->bptr = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ustream_dtor) */

local int	ustream_adjbuf(ustream *,int) noex ;
local int	ustream_bufcpy(ustream *,cchar *,int) noex ;


/* local variables */

cint		nfds = 1 ;

cbool		f_debug		= CF_DEBUG ;

static cint	pagesz		= ulibval.pagesz ;
static cint	maxlinelen	= ulibval.maxline ;


/* exported variables */


/* exported subroutines */

local int ustream_opener(ustream *) noex ;

int ustream::open(cchar *fn,int of,mode_t om) noex {
    	int		rs ;
	DPRINTF("ent fn=%s\n",fn) ;
	if ((rs = ustream_ctor(this,fn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		rs = SR_OK ;
		oflags = (of) ? of : O_RDONLY ;
		if ((rs = u_open(fn,oflags,om)) >= 0) {
		    fd = rs ;
		    rs = ustream_opener(this) ;
		    if (rs < 0) {
			u_close(fd) ;
			fd = -1 ;
		    }
		} /* end if (u_open) */
	    } /* end if (valid) */
	    if (rs < 0) {
		ustream_dtor(this) ;
	    }
	} /* end if (non-null) */
	DPRINTF("ret rs=%d \n",rs) ;
    	return rs ;
} /* end method (ustream::open) */

local int ustream_opener(ustream *op) noex {
    	cint		bsz = 0 ;
	int		rs ;
	DPRINTF("ent\n") ;
        if ((rs = ustream_adjbuf(op,bsz)) >= 0) ylikely {
	    cint dsz = (op->dlen + 1) ;
	    DPRINTF("adjbuf() rs=%d\n",rs) ;
            if (char *p ; (rs = umem.vall(dsz,&p)) >= 0) ylikely {
	        DPRINTF("vall() rs=%d\n",rs) ;
                op->dbuf = p ;
                op->bptr = p ;
                if (rs >= 0) {
                    if (op->oflags & O_NETWORK) {
                        op->fl.net = true ;
                    }
                    op->magval = ustream_magicval ;
                } /* end if (ok) */
                if (rs < 0) {
                    umem.free(op->dbuf) ;
                    op->dbuf = nullptr ;
                }       
            } /* end if (memory-allocation) */
        } /* end if (ustream_adjbuf) */
	DPRINTF("ret rs=%d \n",rs) ;
	return rs ;
}
/* end subroutine (ustream_opener) */

namespace ustream_ns {
    int ustream_close(ustream *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	    if (op->fl.write && (op->blen > 0)) {
	        rs1 = u_writen(op->fd,op->dbuf,op->blen) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbuf) ylikely {
	        rs1 = umem.free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    if (op->fd >= 0) {
		rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
		op->fd = -1 ;
	    }
	    {
	        rs1 = ustream_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->blen = 0 ;
	    op->magval = 0 ;
	return rs ;
    } /* end subroutine (ustream_close) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_read(ustream *op,void *rbuf,int rlen,int to) noex {
	int		rs = SR_OK ;
	int		tlen = 0 ;
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->fl.net) ? USTREAM_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*dbp = charp(rbuf) ;
	    char	*bp ;
	    char	*lastp ;
	    while (tlen < rlen) {
	        int	mlen ;
	        while ((op->blen == 0) && (rc-- > 0)) {
		    cint	fd = op->fd ;
		    cint	bsz = op->dlen ;
		    char	*buf = op->dbuf ;
	            op->bptr = op->dbuf ;
		    if (to >= 0) {
	                rs = ureade(fd,buf,bsz,to,fmo) ;
		    } else {
	                rs = uread(fd,buf,bsz) ;
		    }
	            if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	                f_timedout = true ;
	                rs = SR_OK ;
	                break ;
	            }
	            if (rs < 0) break ;
	            op->blen = rs ;
	        } /* end while (refill) */
	        if ((op->blen == 0) || f_timedout) break ;
	        mlen = min(op->blen,(rlen - tlen)) ;
	        bp = op->bptr ;
	        lastp = op->bptr + mlen ;
	        while (bp < lastp) {
	            *dbp++ = *bp++ ;
	        }
	        op->bptr += mlen ;
	        tlen += mlen ;
	        op->blen -= mlen ;
	    } /* end while */
	    if (rs >= 0) op->foff += tlen ;
	return (rs >= 0) ? tlen : rs ;
    } /* end subroutine (ustream_read) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
   int ustream_readp(ustream *op,void *rbuf,int rlen,off_t off,int to) noex {
	int		rs = SR_OK ;
	int		tlen = 0 ;
	(void) op ;
	(void) rbuf ;
	    (void) rlen ;
	    (void) off ;
	    (void) to ;
	    rs = SR_NOSYS ;
	return (rs >= 0) ? tlen : rs ;
    } /* end subroutine (ustream_readp) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_readln(ustream *op,char *rbuf,int rlen,int to) noex {
	int		rs = SR_OK ;
	int		tlen = 0 ;
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->fl.net) ? USTREAM_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*rbp = rbuf ;
	    char	*bp ;
	    char	*lastp ;
	    while ((rs >= 0) && (tlen < rlen)) {
	        int	mlen ;
	        while ((op->blen == 0) && (rc-- > 0)) {
		    cint	fd = op->fd ;
		    cint	bsz = op->dlen ;
		    char	*buf = op->dbuf ;
	            op->bptr = op->dbuf ;
		    if (to >= 0) {
	                rs = ureade(fd,buf,bsz,to,fmo) ;
		    } else {
	                rs = uread(fd,buf,bsz) ;
		    }
	            if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	                f_timedout = true ;
	                rs = SR_OK ;
	                break ;
	            }
	            if (rs < 0) break ;
	            op->blen = rs ;
	        } /* end while (refilling up buffer) */
	        if (rs >= 0) ylikely {
		    int		i ; /* used-afterwards */
	            if ((op->blen == 0) || f_timedout) break ;
	            mlen = min(op->blen,(rlen - tlen)) ;
	            bp = op->bptr ;
	            lastp = op->bptr + mlen ;
	            while (bp < lastp) {
	                if ((*rbp++ = *bp++) == '\n') break ;
	            } /* end while */
	            i = intconv(bp - op->bptr) ;
	            op->bptr += i ;
	            tlen += i ;
	            op->blen -= i ;
	            if ((i > 0) && (rbp[-1] == '\n')) break ;
	        } /* end if (ok) */
	    } /* end while (trying to satisfy request) */
	    if (rs >= 0) {
	        op->foff += tlen ;
	    }
	return (rs >= 0) ? tlen : rs ;
    } /* end subroutine (ustream_readln) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_readlns(ustream *op,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* return-value */
	    int		lines = 0 ;
	    bool	f_cont = false ;
	    lbuf[0] = '\0' ;
	    while ((lines == 0) || (f_cont = ISCONT(lbuf,i))) {
	        if (f_cont) i -= 2 ;
	        rs = ustream_readln(op,(lbuf + i),(llen - i),to) ;
	        if (rs <= 0) break ;
	        i += rs ;
	        lines += 1 ;
	    } /* end while */
	    if (lcp) *lcp = lines ;
	return (rs >= 0) ? i : rs ;
    } /* end subroutine (ustream_readlns) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_write(ustream *op,cvoid *abuf,int alen) noex {
	    int		rs = SR_OK ;
	    int		alenr ;
	    int		blenr ;
	    int		mlen ;
	    int		len ;
	    cchar	*abp = charp(abuf) ;
	    op->fl.write = true ;
	    if (alen < 0) alen = lenstr(abp) ;
	    alenr = alen ;
	    while ((rs >= 0) && (alenr > 0)) {
	        if ((rs >= 0) && (op->blen == 0) && (alenr >= op->dlen)) {
	            mlen = ifloor(alenr,op->dlen) ;
	            rs = u_write(op->fd,abp,mlen) ;
	            len = rs ;
	            if (rs >= 0) {
	                abp += len ;
	                alenr -= len ;
	            }
	        } /* end if */
	        if ((rs >= 0) && (alenr > 0)) {
	            blenr = op->dlen - op->blen ;
	            mlen = min(alenr,blenr) ;
	            ustream_bufcpy(op,abp,mlen) ;
	            len = mlen ;
	            op->blen += len ;
	            abp += len ;
	            alenr -= len ;
	            if (op->blen == op->dlen) {
	                rs = ustream_flush(op) ;
		    }
	        } /* end if */
	    } /* end while */
	    if (rs >= 0) {
	        op->foff += alen ;
	    }
	return (rs >= 0) ? alen : rs ;
    } /* end subroutine (ustream_write) */
} /* end nameapce (ustream_ns) */

namespace ustream_ns {
    int ustream_println(ustream *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	    int		reslen ;
	    bool	feol = false ;
	    sl = lenstr(sp,sl) ;
	    feol = feol || (sl == 0) ;
	    feol = feol || (sp[sl-1] != '\n') ;
	    reslen = (feol) ? (sl+1) : sl ;
	    if (reslen > 1) {
	        rs = ustream_reserve(op,reslen) ;
	    }
	    if ((rs >= 0) && (sl > 0)) {
	        rs = ustream_write(op,sp,sl) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && feol) {
	        char	buf[2] = { '\n' } ;
	        rs = ustream_write(op,buf,1) ;
	        wlen += rs ;
	    }
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (ustream_println) */
} /* end nameapce (ustream_ns) */

namespace ustream_ns {
    int ustream_reserve(ustream *op,int len) noex {
	int		rs = SR_OK ;
	    if (op->fl.write && (len > 0)) {
	        if (len > (op->dlen - op->blen)) {
		    rs = ustream_flush(op) ;
	        }
	    }
	return rs ;
    } /* end subroutine (ustream_reserve) */
} /* end nameapce (ustream_ns) */

/* update a section of the buffer */
namespace ustream_ns {
    int ustream_update(ustream *op,off_t roff,cchar *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	    uint	boff, bext ;
	    uint	rext = uint(intconv(roff) + rlen) ;
	    int		bdiff ;
	    bool	f_exit = false ;
	    boff = intconv(op->foff - (op->bptr - op->dbuf)) ;
	    bext = intconv(op->foff + op->blen) ;
	    if (roff < boff) {
	        if (rext > boff) {
	            rbuf += (boff - roff) ;
	            rlen -= intconv(boff - roff) ;
	            roff = boff ;
	        } else {
		    f_exit = true ;
		    rlen = 0 ;
	        }
	    }
	    if ((! f_exit) && (rext > bext)) {
	        if (roff < bext) {
	            rlen -= (rext - bext) ;
	            rext = bext ;
	        } else {
		    f_exit = true ;
		    rlen = 0 ;
	        }
	    }
	    if ((! f_exit) && (rlen > 0)) {
	        bdiff = intconv(roff - boff) ;
	        memcopy((op->dbuf + bdiff),rbuf,rlen) ;
	    }
	return (rs >= 0) ? rlen : rs ;
    } /* end subroutine (ustream_update) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_adv(ustream *op,int inc) noex {
	int		rs = SR_INVALID ;
	    if (inc >= 0) ylikely {
		rs = SR_OK ;
	        if (inc > 0) {
	            if (op->fl.write) {
	                rs = ustream_flush(op) ;
	            } else {
	                cint	ml = min(inc,op->blen) ;
	                if (ml > 0) {
	                    inc -= ml ;
	                    op->blen -= ml ;
	                    op->bptr += ml ;
	                    op->foff += ml ;
	                }
	            } /* end if (reading) */
	            if ((rs >= 0) && (inc > 0)) {
		        op->foff += inc ;
		        rs = u_seek(op->fd,inc,SEEK_CUR) ;
	            } /* end if */
	        } /* end if (positive) */
	    } /* end if (valid) */
	return rs ;
    } /* end subroutine (ustream_adv) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_seek(ustream *op,off_t woff,int w) noex {
	int		rs = SR_NOTSEEK ;
	    if (! op->fl.net) {
	        rs = SR_OK ;
	        if (op->fl.write) { /* write */
	            if (op->blen > 0) {
	                rs = ustream_flush(op) ;
	            }
	            if (rs >= 0) {
			off_t	noff ;
	                rs = u_seeko(op->fd,woff,w,&noff) ;
	                op->foff = noff ;
	            }
	        } else { /* read */
	            off_t	aoff = 0 ;
	            switch (w) {
	            case SEEK_CUR:
	                aoff = (- op->blen) ;
	                break ;
	            case SEEK_SET:
	            case SEEK_END:
	                break ;
	            default:
	                rs = SR_INVALID ;
		        break ;
	            } /* end switch */
	            if (rs >= 0) {
			off_t	noff ;
	                rs = u_seeko(op->fd,(woff + aoff),w,&noff) ;
	                op->foff = noff ;
	            }
	            op->bptr = op->dbuf ;
	            op->blen = 0 ;
	        } /* end if (write or read)  */
	    } /* end if (seekable) */
	return rs ;
    } /* end subroutine (ustream_seek) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_tell(ustream *op,off_t *offp) noex {
	int		rs = SR_OK ;
	    if (offp) *offp = op->foff ;
	    rs = int(op->foff & INT_MAX) ;
	return rs ;
    } /* end subroutine (ustream_tell) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_invalidate(ustream *op) noex {
	int		rs = SR_OK ;
	    if ((! op->fl.write) && (op->blen > 0)) {
	        if (! op->fl.net) {
	            rs = u_seek(op->fd,op->foff,SEEK_SET) ;
		}
	    }
	    op->blen = 0 ;
	    op->bptr = op->dbuf ;
	return rs ;
    } /* end subroutine (ustream_invalidate) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_flush(ustream *op) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	    if (op->fl.write && (op->blen > 0)) {
	        rs = u_writen(op->fd,op->dbuf,op->blen) ;
	        len = rs ;
	        op->bptr = op->dbuf ;
	        op->blen = 0 ;
	    } /* end if */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ustream_flush) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_poll(ustream *op,int mto) noex {
	POLLFD		fds[nfds] = {} ;
	    fds[0].fd = op->fd ;
	    fds[0].events = 0 ;
	    fds[0].revents = 0 ;
	return u_poll(fds,nfds,mto) ;
    } /* end subroutine (ustream_poll) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_stat(ustream *op,ustat *sbp) noex {
	return u_fstat(op->fd,sbp) ;
    } /* end subroutine (ustream_stat) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_lockbegin(ustream *op,int lt,int to) noex {
    	int		rs = SR_INVALID ;
	(void) to ;
	    if (lt >= 0) {
		cint cmd = (lt) ? F_RLOCK : F_WLOCK ;
		rs = u_lockfile(op->fd,cmd,0z,0z,to) ;
	    }
	return rs ;
    } /* end subroutine (ustream_lockbegin) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_lockend(ustream *op) noex {
	return u_lockf(op->fd,F_UNLOCK,0z) ;
    } /* end subroutine (ustream_lockend) */
} /* end namespace (ustream_ns) */


/* private subroutines */

local int ustream_adjbuf(ustream *op,int bufsz) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) ylikely {
	    op->dt = filetype(sb.st_mode) ;
	    if (bufsz <= 0) {
	        if (S_ISFIFO(sb.st_mode)) {
	            bufsz = PIPEBUFLEN ;
	        } else {
		    if ((rs = pagesz) >= 0) ylikely {
			coff	ps = off_t(rs) ;
		        off_t	cs ;
	        	cint	of = op->oflags ;
		        if ((of & O_ACCMODE) == O_RDONLY) {
		            csize fs = ((sb.st_size == 0) ? 1 : sb.st_size) ;
		            cs = BCEIL(fs,BLOCKBUFLEN) ;
	                    bufsz = (int) min(ps,cs) ;
	                } else {
		            bufsz = intconv(ps) ;
		        }
		    } /* end if (pagesz) */
	        } /* end if */
	    } /* end if (bufsz) */
	    op->dlen = bufsz ;
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (ustream_adjbuf) */

local int ustream_bufcpy(ustream *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcopy(op->bptr,abp,mlen) ;
	} else {
	    char	*bp = op->bptr ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	} /* end if */
	op->bptr += mlen ;
	return mlen ;
}
/* end subroutine (ustream_bufcpy) */


