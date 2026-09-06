/* filer_prime SUPPORT */
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
	filer

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX |SEEK_{xx}| */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |memcpy(3c)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<funcodes.h>		/* LIBU |FM_{x}(3uc)| */
#include	<intfloor.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<snflags.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"filer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memcopy(3u)| */
import filer_mag ;

/* local defines */

#define	MEMCPYLEN	100

#define	PIPEBUFLEN	1024
#define	BLOCKBUFLEN	512

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif


/* imported namespaces */

using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */
using libuc::libmem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int filer_ctor(filer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->magval = 0 ;
	    op->off = 0 ;
	    op->dbuf = nullptr ;
	    op->bp = nullptr ;
	    op->fl = {} ;
	    op->fd = 0 ;
	    op->of = 0 ;
	    op->dt = 0 ;
	    op->dlen = 0 ;
	    op->len = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subriutine (filer_ctor) */

local int filer_dtor(filer *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filer_dtor) */

local int	filer_adjbuf(filer *,int) noex ;
local int	filer_bufcpy(filer *,cchar *,int) noex ;

[[maybe_unused]] local int	debflags	(int) noex ;
[[maybe_unused]] local int	debdesc		(int) noex ;
[[maybe_unused]] local int	debsize		(int) noex ;


/* local variables */

static sysval		pagesz(sysval_ps) ;

static bufsizevar	maxlinelen(bufsize_ml) ;

cint			netrc = FILER_RCNET ;
cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int filer_start(filer *op,int fd,off_t foff,int bsz,int of) noex {
	int		rs ;
	DPRINTF("ent fd=%d foff=%lld of=%08X\n",fd,foff,of) ;
	if_constexpr (f_debug) {
	    debflags(of) ;
	}
	if ((rs = filer_ctor(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((fd >= 0) && (foff >= 0) && (of >= 0)) {
	        DPRINTF("valid\n") ;
		op->fd = fd ;
		op->of = of ;
	        if_constexpr (f_debug) {
	            debdesc(fd) ;
	            debsize(fd) ;
	        }
		if ((rs = filer_adjbuf(op,bsz)) >= 0) ylikely {
	            DPRINTF("filer_adjbuf() rs=%d\n",rs) ;
	            DPRINTF("-> libmem.vall dlen=%d\n",op->dlen) ;
	            if (char *p ; (rs = libmem.vall(op->dlen,&p)) >= 0) {
	                DPRINTF("libmem.vall() rs=%d\n",rs) ;
	                op->dbuf = p ;
	                op->bp = p ;
	                if (foff < 0) {
			    rs = uc_tell(fd,&foff) ;
			}
	                DPRINTF("mid rs=%d\n",rs) ;
	                if (rs >= 0) {
		            op->off = foff ;
		            if (of & FILER_ONET) op->fl.net = true ;
			    op->magval = FILER_MAGIC ;
	                } /* end if (ok) */
	                if (rs < 0) {
		            libmem.free(op->dbuf) ;
		            op->dbuf = nullptr ;
	                } /* end if (error) */
	                DPRINTF("mid2 rs=%d\n",rs) ;
	            } /* end if (memory-acquire) */
	            DPRINTF("libmem.vall-out rs=%d\n",rs) ;
		} /* end if (filer_adjbuf) */
	    } /* end if (valid) */
	    if (rs < 0) {
		filer_dtor(op) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d bsz=%d\n",rs,bsz) ;
	return (rs >= 0) ? bsz : rs ;
} /* end subroutine (filer_start) */

int filer_finish(filer *op) noex {
	int		rs ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    if (op->fl.write && (op->len > 0)) {
	        rs1 = uc_writen(op->fd,op->dbuf,op->len) ;
	        if (rs >= 0) rs = rs1 ;
		DPRINTF("1 rs=%d\n",rs) ;
	    }
	    if (op->dbuf) ylikely {
	        rs1 = libmem.free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
		DPRINTF("2 rs=%d\n",rs) ;
	    }
	    {
	        rs1 = filer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
		DPRINTF("3 rs=%d\n",rs) ;
	    }
	    op->len = 0 ;
	    op->magval = 0 ;
	} /* end if (magic) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (filer_finish) */

int filer_readp(filer *op,void *rbuf,int rlen,off_t off,int to) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = filer_magic(op,rbuf)) >= 0) ylikely {
	    (void) rlen ;
	    (void) off ;
	    (void) to ;
	    rs = SR_NOSYS ;
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (filer_readp) */

int filer_readln(filer *op,char *rbuf,int rlen,int to) noex {
	int		rs ;
	int		tlen = 0 ;
	DPRINTF("ent\n") ;
	if ((rs = filer_magic(op,rbuf)) >= 0) ylikely {
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->fl.net) ? FILER_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*rbp = rbuf ;
	    char	*bp ;
	    char	*lastp ;
	    while ((rs >= 0) && (tlen < rlen)) {
	        int	mlen ;
	        while ((op->len == 0) && (rc-- > 0)) {
		    cint	fd = op->fd ;
		    cint	bsz = op->dlen ;
		    char	*buf = op->dbuf ;
	            op->bp = op->dbuf ;
		    if (to >= 0) {
	                rs = uc_reade(fd,buf,bsz,to,fmo) ;
		    } else {
	                rs = uc_read(fd,buf,bsz) ;
		    }
	            if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	                f_timedout = true ;
	                rs = SR_OK ;
	                break ;
	            }
	            if (rs < 0) break ;
	            op->len = rs ;
	        } /* end while (refilling up buffer) */
	        if (rs >= 0) ylikely {
		    int		i ; /* used-afterwards */
	            if ((op->len == 0) || f_timedout) break ;
	            mlen = min(op->len,(rlen - tlen)) ;
	            bp = op->bp ;
	            lastp = op->bp + mlen ;
	            while (bp < lastp) {
	                if ((*rbp++ = *bp++) == '\n') break ;
	            } /* end while */
	            i = intconv(bp - op->bp) ;
	            op->bp += i ;
	            tlen += i ;
	            op->len -= i ;
	            if ((i > 0) && (rbp[-1] == '\n')) break ;
	        } /* end if (ok) */
	    } /* end while (trying to satisfy request) */
	    if (rs >= 0) {
	        op->off += tlen ;
	    }
	} /* end if (magic) */
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (filer_readln) */

int filer_readlns(filer *op,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = filer_magic(op,lbuf)) >= 0) ylikely {
	    int		lines = 0 ;
	    bool	f_cont = false ;
	    lbuf[0] = '\0' ;
	    while ((lines == 0) || (f_cont = ISCONT(lbuf,i))) {
	        if (f_cont) i -= 2 ;
	        rs = filer_readln(op,(lbuf + i),(llen - i),to) ;
	        if (rs <= 0) break ;
	        i += rs ;
	        lines += 1 ;
	    } /* end while */
	    if (lcp) *lcp = lines ;
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (filer_readlns) */

/* update a section of the buffer */
int filer_update(filer *op,off_t roff,cchar *rbuf,int rlen) noex {
	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = filer_magic(op,rbuf)) >= 0) ylikely {
	    uint	boff, bext ;
	    uint	rext = uint(intconv(roff) + rlen) ;
	    int		bdiff ;
	    bool	f_exit = false ;
	    boff = intconv(op->off - (op->bp - op->dbuf)) ;
	    bext = intconv(op->off + op->len) ;
	    if (roff < boff) {
	        if (rext > boff) {
	            rbuf += (boff - roff) ;
	            rlen -= intconv(boff - roff) ;
	            roff = boff ;
	        } else {
		    f_exit = true ;
		    rlen = 0 ;
	        }
	    } /* end if */
	    if ((! f_exit) && (rext > bext)) {
	        if (roff < bext) {
	            rlen -= (rext - bext) ;
	            rext = bext ;
	        } else {
		    f_exit = true ;
		    rlen = 0 ;
	        }
	    } /* end if */
	    if ((! f_exit) && (rlen > 0)) {
	        bdiff = intconv(roff - boff) ;
	        memcopy((op->dbuf + bdiff),rbuf,rlen) ;
	    } /* end if */
	} /* end if (magic) */
	DPRINTF("ret rs=%d len=%d\n",rs,rlen) ;
	return (rs >= 0) ? rlen : rs ;
} /* end subroutine (filer_update) */

int filer_writeto(filer *op,cvoid *abuf,int alen,int) noex {
    	return filer_write(op,abuf,alen) ;
} /* end subroutine (filer_writeto) */

int filer_write(filer *op,cvoid *abuf,int alen) noex {
	int		rs ;
	cchar		*abp = charp(abuf) ;
	DPRINTF("ent wlen=%d\n",alen) ;
	if ((rs = filer_magic(op,abuf)) >= 0) ylikely {
	    int		alenr ;
	    int		blenr ;
	    int		mlen ;
	    int		len ;
	    op->fl.write = true ;
	    if (alen < 0) alen = lenstr(abp) ;
	    alenr = alen ;
	    while ((rs >= 0) && (alenr > 0)) {
	        if ((rs >= 0) && (op->len == 0) && (alenr >= op->dlen)) {
	            mlen = ifloor(alenr,op->dlen) ;
		    DPRINTF("mlen=%d\n",mlen) ;
	            rs = uc_writen(op->fd,abp,mlen) ;
		    DPRINTF("uc_writen() rs=%d\n",rs) ;
	            len = rs ;
	            if (rs >= 0) {
	                abp += len ;
	                alenr -= len ;
	            } /* end if (ok) */
	        } /* end if */
	        if ((rs >= 0) && (alenr > 0)) {
	            blenr = op->dlen - op->len ;
	            mlen = min(alenr,blenr) ;
	            filer_bufcpy(op,abp,mlen) ;
	            len = mlen ;
	            op->len += len ;
	            abp += len ;
	            alenr -= len ;
	            if (op->len == op->dlen) {
	                rs = filer_flush(op) ;
		        DPRINTF("filer_flush() rs=%d\n",rs) ;
		    } /* end if (flush) */
	        } /* end if */
	    } /* end while */
	    if (rs >= 0) {
	        op->off += alen ;
	    } /* end if */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d len=%d\n",rs,alen) ;
	return (rs >= 0) ? alen : rs ;
} /* end subroutine (filer_write) */

int filer_reserve(filer *op,int len) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    if (op->fl.write && (len > 0)) {
	        if (len > (op->dlen - op->len)) {
		    rs = filer_flush(op) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_reserve) */

int filer_println(filer *op,cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,sp)) >= 0) ylikely {
	    int		reslen ;
	    bool	feol = false ;
	    sl = lenstr(sp,sl) ;
	    feol = feol || (sl == 0) ;
	    feol = feol || (sp[sl-1] != '\n') ;
	    reslen = (feol) ? (sl+1) : sl ;
	    if (reslen > 1) {
	        rs = filer_reserve(op,reslen) ;
	    }
	    if ((rs >= 0) && (sl > 0)) {
	        rs = filer_write(op,sp,sl) ;
	        wlen += rs ;
	    } /* end if */
	    if ((rs >= 0) && feol) {
	        char	buf[2] = { '\n' } ;
	        rs = filer_write(op,buf,1) ;
	        wlen += rs ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filer_println) */

int filer_vprintf(filer *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,fmt,ap)) >= 0) ylikely {
	    if ((rs = maxlinelen) >= 0) ylikely {
		cint	llen = rs ;
		if (char *lbuf ; (rs = libmem.mall((llen + 1),&lbuf)) >= 0) {
	    	    if ((rs = fmtstr(lbuf,llen,0,fmt,ap)) >= 0) {
	    	        rs = filer_write(op,lbuf,rs) ;
			wlen = rs ;
		    }
		    rs1 = libmem.free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxlinelen) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filer_vprintf) */

int filer_printf(filer *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && fmt) ylikely {
	    va_begin(ap,fmt) ;
	    rs = filer_vprintf(op,fmt,ap) ;
	    wlen = rs ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filer_printf) */

int filer_adv(filer *op,int inc) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (inc >= 0) ylikely {
		rs = SR_OK ;
	        if (inc > 0) {
	            if (op->fl.write) {
	                rs = filer_flush(op) ;
	            } else {
	                if (cint ml = min(inc,op->len) ; ml > 0) {
	                    inc -= ml ;
	                    op->len -= ml ;
	                    op->bp += ml ;
	                    op->off += ml ;
	                } /* end if */
	            } /* end if (reading) */
	            if ((rs >= 0) && (inc > 0)) {
		        op->off += inc ;
		        rs = uc_seek(op->fd,inc,SEEK_CUR) ;
	            } /* end if */
	        } /* end if (positive) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_adv) */

int filer_seek(filer *op,off_t woff,int w) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    rs = SR_NOTSEEK ;
	    if (! op->fl.net) {
	        rs = SR_OK ;
	        if (op->fl.write) { /* write */
	            if (op->len > 0) {
	                rs = filer_flush(op) ;
	            }
	            if (rs >= 0) {
			off_t	noff ;
	                rs = uc_seeko(op->fd,woff,w,&noff) ;
	                op->off = noff ;
	            } /* end if (ok) */
	        } else { /* read */
	            off_t	aoff = 0 ;
	            switch (w) {
	            case SEEK_CUR:
	                aoff = (neg op->len) ;
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
	                rs = uc_seeko(op->fd,(woff + aoff),w,&noff) ;
	                op->off = noff ;
	            } /* end if (ok) */
	            op->bp = op->dbuf ;
	            op->len = 0 ;
	        } /* end if (write or read)  */
	    } /* end if (seekable) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_seek) */

int filer_tell(filer *op,off_t *offp) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    if (offp) *offp = op->off ;
	    rs = int(op->off & INT_MAX) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_tell) */

int filer_invalidate(filer *op) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    if ((! op->fl.write) && (op->len > 0)) {
	        if (! op->fl.net) {
	            rs = uc_seek(op->fd,op->off,SEEK_SET) ;
		}
	    }
	    op->len = 0 ;
	    op->bp = op->dbuf ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_invalidate) */

int filer_flush(filer *op) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    if (op->fl.write && (op->len > 0)) {
	        rs = uc_writen(op->fd,op->dbuf,op->len) ;
	        len = rs ;
	        op->bp = op->dbuf ;
	        op->len = 0 ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (filer_flush) */

int filer_poll(filer *op,int mto) noex {
	cint		nfds = 1 ;
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    POLLFD	fds[nfds] = {} ;
	    fds[0].fd = op->fd ;
	    fds[0].events = 0 ;
	    fds[0].revents = 0 ;
	    rs = u_poll(fds,nfds,mto) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_poll) */

int filer_stat(filer *op,ustat *sbp) noex {
    	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = filer_magic(op,sbp)) >= 0) ylikely {
	    rs = uc_fstat(op->fd,sbp) ;
	} /* end if (magic) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (filer_stat) */

int filer_lockbegin(filer *op,int lt,int to) noex {
    	int		rs ;
	(void) to ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (lt >= 0) {
		cint cmd = (lt) ? F_RLOCK : F_WLOCK ;
		rs = uc_lockfile(op->fd,cmd,0z,0z,to) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_lockbegin) */

int filer_lockend(filer *op) noex {
    	int		rs ;
	if ((rs = filer_magic(op)) >= 0) ylikely {
	    rs = uc_lockf(op->fd,F_UNLOCK,0z) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_lockend) */

int filer_rewind(filer *op) noex {
	return filer_seek(op,0z,SEEK_SET) ;
} /* end subroutine (filer_rewind) */


/* private subroutines */

local int filer_adjbuf(filer *op,int bufsz) noex {
	int		rs ;
	DPRINTF("ent bufsz=%d\n",bufsz) ;
	if (ustat sb ; (rs = uc_fstat(op->fd,&sb)) >= 0) ylikely {
	    csize fsize = size_t(sb.st_size) ;
	    DPRINTF("uc_fstat() rs=%d fsize=%lu\n",rs,fsize) ;
	    op->dt = filetype(sb.st_mode) ;
	    if (bufsz <= 0) {
	        DPRINTF("need bufsz=%d\n",bufsz) ;
	        if (S_ISFIFO(sb.st_mode)) {
	            DPRINTF("-FIFO-yes\n") ;
	            bufsz = PIPEBUFLEN ;
	        } else {
	            DPRINTF("-FIFO-not\n") ;
		    if ((rs = pagesz) >= 0) ylikely {
			coff	ps = off_t(rs) ;
		        off_t	cs ;
	        	cint	of = op->of ;
	                DPRINTF("pagesz=%d\n",rs) ;
		        if ((of & O_ACCMODE) == O_RDONLY) {
	                    DPRINTF("RDONLY-yes\n") ;
		            csize fs = ((sb.st_size == 0) ? 1 : sb.st_size) ;
		            cs = BCEIL(fs,BLOCKBUFLEN) ;
	                    bufsz = (int) min(ps,cs) ;
	                } else {
	                    DPRINTF("RDONLY-not\n") ;
		            bufsz = intconv(ps) ;
		        } /* end if */
		    } /* end if (pagesz) */
	        } /* end if */
	    } /* end if (bufsz) */
	    op->dlen = bufsz ;
	    if (rs >= 0) {
		rs = op->dlen ;
	    } /* end if (ok) */
	} /* end if (uc_fstat) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (filer_adjbuf) */

local int filer_bufcpy(filer *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcopy(op->bp,abp,mlen) ;
	} else {
	    char	*bp = op->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    } /* end for */
	} /* end if */
	op->bp += mlen ;
	return mlen ;
} /* end subroutine (filer_bufcpy) */

local int debflags(int of) noex {
    	int		rs ;
	int		rs1 ;
	if (char *lbuf ; (rs = libmem.ml(&lbuf)) >= 0) {
	    if ((rs = snflagsopen(lbuf,rs,of)) >= 0) {
		DPRINTF("ofl %s\n",lbuf) ;
	    } /* end if (snflagsopen) */
	    rs1 = libmem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-d) */
	return rs ;
} /* end subroutine (debflags) */

local int debdesc(int fd) noex {
    	int		rs ;
	if ((rs = u_fgetfl(fd)) >= 0) {
	    debflags(rs) ;
	} /* end if (u_fgetfl) */
	return rs ;
} /* end subroutine (debdesc) */

local int debsize(int fd) noex {
    	int		rs ;
	if ((rs = u_fsize(fd)) >= 0) {
	    DPRINTF("fsz=%d\n",rs) ;
	} /* end if (u_fsize) */
	return rs ;
} /* end subroutine (debsize) */


