/* filebuf */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */

#define	CF_NULTERM	0		/* NUL terminate read-string */

/* revision history:

	= 1998-07-09, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I/O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<usupport.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<intfloor.h>
#include	<format.h>
#include	<localmisc.h>

#include	"filebuf.h"


/* local defines */

#define	MEMCPYLEN	100

#define	PIPEBUFLEN	1024
#define	BLOCKBUFLEN	512

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* local namespaces */

using std::min ;
using std::max ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int filebuf_ctor(filebuf *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->off = 0 ;
	    op->buf = nullptr ;
	    op->bp = nullptr ;
	    op->f = {} ;
	    op->fd = 0 ;
	    op->of = 0 ;
	    op->dt = 0 ;
	    op->bufsize = 0 ;
	    op->len = 0 ;
	}
	return rs ;
}

static int	filebuf_adjbuf(filebuf *,int) noex ;
static int	filebuf_bufcpy(filebuf *,cchar *,int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static bufsizevar	maxlinelen(getbufsize_ml) ;


/* exported subroutines */

int filebuf_start(filebuf *op,int fd,off_t coff,int bufsize,int of) noex {
	int		rs ;
	if ((rs = filebuf_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (fd >= 0) {
		op->fd = fd ;
		op->of = of ;
		if ((rs = filebuf_adjbuf(op,bufsize)) >= 0) {
	            char	*p ;
	            if ((rs = uc_libvalloc(bufsize,&p)) >= 0) {
	                op->buf = p ;
	                op->bp = p ;
	                if (coff < 0) rs = u_tell(fd,&coff) ;
	                if (rs >= 0) {
		            op->off = coff ;
		            if (of & FILEBUF_ONET) op->f.net = true ;
	                }
	                if (rs < 0) {
		            uc_libfree(op->buf) ;
		            op->buf = NULL ;
	                }	
	            } /* end if (memory-allocation) */
		} /* end if (filebuf_adjbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? bufsize : rs ;
}
/* end subroutine (filebuf_start) */

int filebuf_finish(filebuf *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.write && (op->len > 0)) {
	        rs1 = uc_writen(op->fd,op->buf,op->len) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->buf) {
	        rs1 = uc_libfree(op->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->buf = NULL ;
	    }
	    op->len = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_finish) */

int filebuf_read(filebuf *op,void *rbuf,int rlen,int to) noex {
	int		rs = SR_FAULT ;
	int		tlen = 0 ;
	if (op && rbuf) {
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->f.net) ? FILEBUF_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*dbp = (char *) rbuf ;
	    char	*bp, *lastp ;
	    rs = SR_OK ;
	    while (tlen < rlen) {
	        int	mlen ;
	        while ((op->len == 0) && (rc-- > 0)) {
		    cint	fd = op->fd ;
		    cint	bufsize = op->bufsize ;
		    char	*buf = op->buf ;
	            op->bp = op->buf ;
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
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filebuf_read) */

#ifdef	COMMENT
int filebuf_readp(filebuf op,void *buf,int buflen,off_t off,int to) noex {
	int		rs = SR_NOSYS ;
	int		tlen = 0 ;

	return (rs >= 0) ? tlen : SR_NOSYS ;
}
/* end subroutine (filebuf_readp) */
#endif /* COMMENT */

int filebuf_readline(filebuf *op,char *rbuf,int rlen,int to) noex {
	int		rs = SR_FAULT ;
	int		tlen = 0 ;
	if (op) {
	    cint	fmo = FM_TIMED ;
	    int		rc = (op->f.net) ? FILEBUF_RCNET : 1 ;
	    bool	f_timedout = false ;
	    char	*rbp = rbuf ;
	    char	*bp, *lastp ;
	    rs = SR_OK ;
	    while ((rs >= 0) && (tlen < rlen)) {
	        int		mlen ;
	        while ((op->len == 0) && (rc-- > 0)) {
		        cint	fd = op->fd ;
		        cint	bufsize = op->bufsize ;
		        char	*buf = op->buf ;
	            op->bp = op->buf ;
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
	            i = bp - op->bp ;
	            op->bp += i ;
	            tlen += i ;
	            op->len -= i ;
	            if ((i > 0) && (rbp[-1] == '\n')) break ;
	        } /* end if (ok) */
	    } /* end while (trying to satisfy request) */
	    if (rs >= 0) {
#if	CF_NULTERM
	        *rbp = '\0' ;
#endif
	        op->off += tlen ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filebuf_readline) */

int filebuf_readlines(filebuf *fp,char *lbuf,int llen,int to,int *lcp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (fp && lbuf) {
	    int		lines = 0 ;
	    bool	f_cont = false ;
	    rs = SR_OK ;
	    lbuf[0] = '\0' ;
	    while ((lines == 0) || (f_cont = ISCONT(lbuf,i))) {
	        if (f_cont) i -= 2 ;
	        rs = filebuf_readline(fp,(lbuf + i),(llen - i),to) ;
	        if (rs <= 0) break ;
	        i += rs ;
	        lines += 1 ;
	    } /* end while */
	    if (lcp) *lcp = lines ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (filebuf_readlines) */

/* update a section of the buffer */
int filebuf_update(filebuf *op,off_t roff,cchar *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (op && rbuf) {
	    uint	boff, bext ;
	    uint	rext = roff + rlen ;
	    int		bdiff ;
	    bool	f_exit = false ;
	    rs = SR_OK ;
	    boff = op->off - (op->bp - op->buf) ;
	    bext = op->off + op->len ;
	    if (roff < boff) {
	        if (rext > boff) {
	            rbuf += (boff - roff) ;
	            rlen -= (boff - roff) ;
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
	        bdiff = roff - boff ;
	        memcpy((op->buf + bdiff),rbuf,rlen) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (filebuf_update) */

int filebuf_write(filebuf *op,const void *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (op && abuf) {
	    int		alenr ;
	    int		blenr ;
	    int		mlen ;
	    int		len ;
	    cchar	*abp = (cchar *) abuf ;
	    rs = SR_OK ;
	    op->f.write = true ;
	    if (alen < 0) alen = strlen(abp) ;
	    alenr = alen ;
	    while ((rs >= 0) && (alenr > 0)) {
	        if ((rs >= 0) && (op->len == 0) && (alenr >= op->bufsize)) {
	            mlen = ifloor(alenr,op->bufsize) ;
	            rs = uc_writen(op->fd,abp,mlen) ;
	            len = rs ;
	            if (rs >= 0) {
	                abp += len ;
	                alenr -= len ;
	            }
	        } /* end if */
	        if ((rs >= 0) && (alenr > 0)) {
	            blenr = op->bufsize - op->len ;
	            mlen = min(alenr,blenr) ;
	            filebuf_bufcpy(op,abp,mlen) ;
	            len = mlen ;
	            op->len += len ;
	            abp += len ;
	            alenr -= len ;
	            if (op->len == op->bufsize) {
	                rs = filebuf_flush(op) ;
		    }
	        } /* end if */
	    } /* end while */
	    if (rs >= 0) {
	        op->off += alen ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (filebuf_write) */

int filebuf_reserve(filebuf *op,int len) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.write && (len > 0)) {
	        if (len > (op->bufsize - op->len)) {
		    rs = filebuf_flush(op) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_reserve) */

int filebuf_print(filebuf *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && sp) {
	    int		reslen ;
	    bool	f_needeol ;
	    rs = SR_OK ;
	    sl = strnlen(sp,sl) ;
	    f_needeol = ((sl == 0) || (sp[sl-1] != '\n')) ;
	    reslen = (f_needeol) ? (sl+1) : sl ;
	    if (reslen > 1) {
	        rs = filebuf_reserve(op,reslen) ;
	    }
	    if ((rs >= 0) && (sl > 0)) {
	        rs = filebuf_write(op,sp,sl) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && f_needeol) {
	        char	buf[2] ;
	        buf[0] = '\n' ;
	        buf[1] = '\0' ;
	        rs = filebuf_write(op,buf,1) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_print) */

int filebuf_vprintf(filebuf *op,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (op && fmt) {
	    if ((rs = maxlinelen) >= 0) {
		cint	llen = rs ;
		char	*lbuf ;
		if ((rs = uc_libmalloc((llen+1),&lbuf)) >= 0) {
	    	    if ((rs = format(lbuf,llen,0,fmt,ap)) >= 0) {
	    	        rs = filebuf_write(op,lbuf,rs) ;
			wlen = rs ;
		    }
		    rs1 = uc_libfree(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxlinelen) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_vprintf) */

int filebuf_printf(filebuf *op,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && fmt) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = filebuf_vprintf(op,fmt,ap) ;
	    wlen = rs ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_printf) */

int filebuf_adv(filebuf *op,int inc) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (inc >= 0) {
		rs = SR_OK ;
	        if (inc > 0) {
	            if (op->f.write) {
	                rs = filebuf_flush(op) ;
	            } else {
	                int	ml = min(inc,op->len) ;
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_adv) */

int filebuf_seek(filebuf *op,off_t woff,int w) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTSEEK ;
	    if (! op->f.net) {
	        rs = SR_OK ;
	        if (op->f.write) { /* write */
	            if (op->len > 0) {
	                rs = filebuf_flush(op) ;
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
	            op->bp = op->buf ;
	            op->len = 0 ;
	        } /* end if (write or read)  */
	    } /* end if (seekable) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_seek) */

int filebuf_tell(filebuf *op,off_t *offp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (offp) *offp = op->off ;
	    rs = int(op->off & INT_MAX) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_tell) */

int filebuf_invalidate(filebuf *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if ((! op->f.write) && (op->len > 0)) {
	        if (! op->f.net) {
	            rs = u_seek(op->fd,op->off,SEEK_SET) ;
		}
	    }
	    op->len = 0 ;
	    op->bp = op->buf ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_invalidate) */

int filebuf_flush(filebuf *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.write && (op->len > 0)) {
	        rs = uc_writen(op->fd,op->buf,op->len) ;
	        len = rs ;
	        op->bp = op->buf ;
	        op->len = 0 ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (filebuf_flush) */

int filebuf_poll(filebuf *op,int mto) noex {
	cint		nfds = 1 ;
	int		rs = SR_FAULT ;
	if (op) {
	    POLLFD	fds[1] ;
	    cint	size = nfds * sizeof(POLLFD) ;
	    memset(fds,0,size) ;
	    fds[0].fd = op->fd ;
	    fds[0].events = 0 ;
	    fds[0].revents = 0 ;
	    rs = u_poll(fds,nfds,mto) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filebuf_poll) */


/* private subroutines */

static int filebuf_adjbuf(filebuf *op,int bufsize) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	    op->dt = filetype(sb.st_mode) ;
	    if (bufsize <= 0) {
	        if (S_ISFIFO(sb.st_mode)) {
	            bufsize = PIPEBUFLEN ;
	        } else {
		    if ((rs = pagesize) >= 0) {
			off_t	ps = off_t(rs) ;
		        off_t	cs ;
	        	cint	of = op->of ;
		        if ((of & O_ACCMODE) == O_RDONLY) {
		            off_t fs = ((sb.st_size == 0) ? 1 : sb.st_size) ;
		            cs = BCEIL(fs,BLOCKBUFLEN) ;
	                    bufsize = (int) min(ps,cs) ;
	                } else {
		            bufsize = ps ;
		        }
		    } /* end if (pagesize) */
	        } /* end if */
	    } /* end if (bufsize) */
	    op->bufsize = bufsize ;
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (filebuf_adjbuf) */

static int filebuf_bufcpy(filebuf *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcpy(op->bp,abp,mlen) ;
	} else {
	    char	*bp = op->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	} /* end if */
	op->bp += mlen ;
	return mlen ;
}
/* end subroutine (filebuf_bufcpy) */


