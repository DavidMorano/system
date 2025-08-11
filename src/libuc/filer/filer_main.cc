/* filer_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


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
#include	<sys/stat.h>
#include	<unistd.h>		/* |SEEK_{xx}| */
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<intfloor.h>
#include	<fmtstr.h>
#include	<localmisc.h>

#include	"filer.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |memcopy(3u)| */

/* local defines */

#define	MEMCPYLEN	100

#define	PIPEBUFLEN	1024
#define	BLOCKBUFLEN	512

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* imported namespaces */

using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int filer_ctor(filer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
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
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filer_dtor) */

local int	filer_adjbuf(filer *,int) noex ;
local int	filer_bufcpy(filer *,cchar *,int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static bufsizevar	maxlinelen(getbufsize_ml) ;


/* exported variables */


/* exported subroutines */

int filer_start(filer *op,int fd,off_t foff,int bsz,int of) noex {
	int		rs ;
	if ((rs = filer_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (fd >= 0) {
		op->fd = fd ;
		op->of = of ;
		if ((rs = filer_adjbuf(op,bsz)) >= 0) {
	            if (char *p ; (rs = uc_libvalloc(bsz,&p)) >= 0) {
	                op->dbuf = p ;
	                op->bp = p ;
	                if (foff < 0) {
			    rs = u_tell(fd,&foff) ;
			}
	                if (rs >= 0) {
		            op->off = foff ;
		            if (of & FILER_ONET) op->fl.net = true ;
			    op->magic = FILER_MAGIC ;
	                } /* end if (ok) */
	                if (rs < 0) {
		            uc_libfree(op->dbuf) ;
		            op->dbuf = nullptr ;
	                }	
	            } /* end if (memory-allocation) */
		} /* end if (filer_adjbuf) */
	    } /* end if (valid) */
	    if (rs < 0) {
		filer_dtor(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? bsz : rs ;
}
/* end subroutine (filer_start) */

int filer_finish(filer *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = filer_magic(op)) >= 0) {
	    if (op->fl.write && (op->len > 0)) {
	        rs1 = uc_writen(op->fd,op->dbuf,op->len) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbuf) {
	        rs1 = uc_libfree(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    {
	        rs1 = filer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->len = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_finish) */

int filer_read(filer *op,void *rbuf,int rlen,int to) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = filer_magic(op,rbuf)) >= 0) {
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->fl.net) ? FILER_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*dbp = charp(rbuf) ;
	    char	*bp ;
	    char	*lastp ;
	    while (tlen < rlen) {
	        int	mlen ;
	        while ((op->len == 0) && (rc-- > 0)) {
		    cint	fd = op->fd ;
		    cint	bufsize = op->dlen ;
		    char	*buf = op->dbuf ;
	            op->bp = op->dbuf ;
		    if (to >= 0) {
	                rs = uc_reade(fd,buf,bufsize,to,fmo) ;
		    } else {
	                rs = u_read(fd,buf,bufsize) ;
		    }
	            if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	                f_timedout = true ;
	                rs = SR_OK ;
	                break ;
	            }
	            if (rs < 0) break ;
	            op->len = rs ;
	        } /* end while (refill) */
	        if ((op->len == 0) || f_timedout) break ;
	        mlen = min(op->len,(rlen - tlen)) ;
	        bp = op->bp ;
	        lastp = op->bp + mlen ;
	        while (bp < lastp) {
	            *dbp++ = *bp++ ;
	        }
	        op->bp += mlen ;
	        tlen += mlen ;
	        op->len -= mlen ;
	    } /* end while */
	    if (rs >= 0) op->off += tlen ;
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filer_read) */

int filer_readp(filer *op,void *rbuf,int rlen,off_t off,int to) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = filer_magic(op,rbuf)) >= 0) {
	    (void) rlen ;
	    (void) off ;
	    (void) to ;
	    rs = SR_NOSYS ;
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filer_readp) */

int filer_readln(filer *op,char *rbuf,int rlen,int to) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = filer_magic(op,rbuf)) >= 0) {
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
		    cint	bufsize = op->dlen ;
		    char	*buf = op->dbuf ;
	            op->bp = op->dbuf ;
		    if (to >= 0) {
	                rs = uc_reade(fd,buf,bufsize,to,fmo) ;
		    } else {
	                rs = u_read(fd,buf,bufsize) ;
		    }
	            if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	                f_timedout = true ;
	                rs = SR_OK ;
	                break ;
	            }
	            if (rs < 0) break ;
	            op->len = rs ;
	        } /* end while (refilling up buffer) */
	        if (rs >= 0) {
		    int		i ;
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
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filer_readln) */

int filer_readlns(filer *op,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = filer_magic(op,lbuf)) >= 0) {
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
}
/* end subroutine (filer_readlns) */

/* update a section of the buffer */
int filer_update(filer *op,off_t roff,cchar *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = filer_magic(op,rbuf)) >= 0) {
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
	} /* end if (magic) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (filer_update) */

int filer_write(filer *op,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = filer_magic(op,abuf)) >= 0) {
	    int		alenr ;
	    int		blenr ;
	    int		mlen ;
	    int		len ;
	    cchar	*abp = charp(abuf) ;
	    op->fl.write = true ;
	    if (alen < 0) alen = lenstr(abp) ;
	    alenr = alen ;
	    while ((rs >= 0) && (alenr > 0)) {
	        if ((rs >= 0) && (op->len == 0) && (alenr >= op->dlen)) {
	            mlen = ifloor(alenr,op->dlen) ;
	            rs = uc_writen(op->fd,abp,mlen) ;
	            len = rs ;
	            if (rs >= 0) {
	                abp += len ;
	                alenr -= len ;
	            }
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
		    }
	        } /* end if */
	    } /* end while */
	    if (rs >= 0) {
	        op->off += alen ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (filer_write) */

int filer_reserve(filer *op,int len) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    if (op->fl.write && (len > 0)) {
	        if (len > (op->dlen - op->len)) {
		    rs = filer_flush(op) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_reserve) */

int filer_println(filer *op,cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,sp)) >= 0) {
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
	    }
	    if ((rs >= 0) && feol) {
	        char	buf[2] = { '\n' } ;
	        rs = filer_write(op,buf,1) ;
	        wlen += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_println) */

int filer_vprintf(filer *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,fmt,ap)) >= 0) {
	    if ((rs = maxlinelen) >= 0) {
		cint	llen = rs ;
		if (char *lbuf{} ; (rs = uc_libmalloc((llen+1),&lbuf)) >= 0) {
	    	    if ((rs = fmtstr(lbuf,llen,0,fmt,ap)) >= 0) {
	    	        rs = filer_write(op,lbuf,rs) ;
			wlen = rs ;
		    }
		    rs1 = uc_libfree(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxlinelen) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_vprintf) */

int filer_printf(filer *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && fmt) {
	    va_begin(ap,fmt) ;
	    rs = filer_vprintf(op,fmt,ap) ;
	    wlen = rs ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_printf) */

int filer_adv(filer *op,int inc) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (inc >= 0) {
		rs = SR_OK ;
	        if (inc > 0) {
	            if (op->fl.write) {
	                rs = filer_flush(op) ;
	            } else {
	                cint	ml = min(inc,op->len) ;
	                if (ml > 0) {
	                    inc -= ml ;
	                    op->len -= ml ;
	                    op->bp += ml ;
	                    op->off += ml ;
	                }
	            } /* end if (reading) */
	            if ((rs >= 0) && (inc > 0)) {
		        op->off += inc ;
		        rs = u_seek(op->fd,inc,SEEK_CUR) ;
	            } /* end if */
	        } /* end if (positive) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_adv) */

int filer_seek(filer *op,off_t woff,int w) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    rs = SR_NOTSEEK ;
	    if (! op->fl.net) {
	        rs = SR_OK ;
	        if (op->fl.write) { /* write */
	            if (op->len > 0) {
	                rs = filer_flush(op) ;
	            }
	            if (rs >= 0) {
			off_t	noff ;
	                rs = u_seeko(op->fd,woff,w,&noff) ;
	                op->off = noff ;
	            }
	        } else { /* read */
	            off_t	aoff = 0 ;
	            switch (w) {
	            case SEEK_CUR:
	                aoff = (- op->len) ;
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
	                op->off = noff ;
	            }
	            op->bp = op->dbuf ;
	            op->len = 0 ;
	        } /* end if (write or read)  */
	    } /* end if (seekable) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_seek) */

int filer_tell(filer *op,off_t *offp) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    if (offp) *offp = op->off ;
	    rs = int(op->off & INT_MAX) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_tell) */

int filer_invalidate(filer *op) noex {
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    if ((! op->fl.write) && (op->len > 0)) {
	        if (! op->fl.net) {
	            rs = u_seek(op->fd,op->off,SEEK_SET) ;
		}
	    }
	    op->len = 0 ;
	    op->bp = op->dbuf ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_invalidate) */

int filer_flush(filer *op) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = filer_magic(op)) >= 0) {
	    if (op->fl.write && (op->len > 0)) {
	        rs = uc_writen(op->fd,op->dbuf,op->len) ;
	        len = rs ;
	        op->bp = op->dbuf ;
	        op->len = 0 ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (filer_flush) */

int filer_poll(filer *op,int mto) noex {
	cint		nfds = 1 ;
	int		rs ;
	if ((rs = filer_magic(op)) >= 0) {
	    POLLFD	fds[nfds] = {} ;
	    fds[0].fd = op->fd ;
	    fds[0].events = 0 ;
	    fds[0].revents = 0 ;
	    rs = u_poll(fds,nfds,mto) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filer_poll) */

int filer_stat(filer *op,ustat *sbp) noex {
    	int		rs ;
	if ((rs = filer_magic(op,sbp)) >= 0) {
	    rs = uc_fstat(op->fd,sbp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_stat) */

int filer_lockbegin(filer *op,int lt,int to) noex {
    	int		rs ;
	(void) to ;
	if ((rs = filer_magic(op)) >= 0) {
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
	if ((rs = filer_magic(op)) >= 0) {
	    rs = uc_lockf(op->fd,F_UNLOCK,0z) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filer_lockend) */


/* private subroutines */

local int filer_adjbuf(filer *op,int bufsz) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	    op->dt = filetype(sb.st_mode) ;
	    if (bufsz <= 0) {
	        if (S_ISFIFO(sb.st_mode)) {
	            bufsz = PIPEBUFLEN ;
	        } else {
		    if ((rs = pagesize) >= 0) {
			coff	ps = off_t(rs) ;
		        off_t	cs ;
	        	cint	of = op->of ;
		        if ((of & O_ACCMODE) == O_RDONLY) {
		            csize fs = ((sb.st_size == 0) ? 1 : sb.st_size) ;
		            cs = BCEIL(fs,BLOCKBUFLEN) ;
	                    bufsz = (int) min(ps,cs) ;
	                } else {
		            bufsz = intconv(ps) ;
		        }
		    } /* end if (pagesize) */
	        } /* end if */
	    } /* end if (bufsz) */
	    op->dlen = bufsz ;
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (filer_adjbuf) */

local int filer_bufcpy(filer *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcopy(op->bp,abp,mlen) ;
	} else {
	    char	*bp = op->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	} /* end if */
	op->bp += mlen ;
	return mlen ;
}
/* end subroutine (filer_bufcpy) */


