/* sbuf_main SUPPORT */
/* lang=C++20 */

/* storage buffer (SBuf) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

	= 2016-11-06, David A­D­ Morano
	I did some optimization for the method |sbuf_decX()| and
	|sbuf_hexX()| to form the digits in the object buffer
	directly rather than in a separate buffer and then copying
	the data into the object buffer.

	= 2017-11-06, David A­D­ Morano
	I enhanced the |sbuf_hexXX()| methods using the property
	that the length (in bytes) of the result is known ahead of
	time. We cannot do this (really quickly) with decimal
	conversions.

*/

/* Copyright © 1998,2016,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sbuf

	Description:
	This module can be used to construct strings or messages
	in buffers WITHOUT using the "snwprint(3c)| subroutine or
	something similar.  This module is useful when the user
	SUPPLIES a buffer of a specified length to the object at
	object initialization.  This module uses an object, that
	must be initialized and eventually freed, to track the state
	of the user supplied buffer.

	Arguments:
	bop		pointer to the buffer object
	<others>	depending on the call made

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<uvariables.hh>		/* |sysword(3u)| */
#include	<format.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sbuf.h"


/* local defines */

#define	SBUF_RBUF	(sbp->rbuf)
#define	SBUF_RLEN	(sbp->rlen)
#define	SBUF_INDEX	(sbp->index)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = strlen(sysword.w_blanks) ;
	cchar	*p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
}


/* forward references */

static int	sbuf_addstrw(sbuf *,cchar *,int) noex ;

template<typename T>
int sbuf_xxxx(sbuf *sbp,int (*ctxxx)(char *,int,T),T v) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        int		bl = (SBUF_RLEN-SBUF_INDEX) ;
	        if (bl >= dlen) {
	            char	*bp = (SBUF_RBUF+SBUF_INDEX) ;
	            if ((rs = ctxxx(bp,bl,v)) >= 0) {
		        SBUF_INDEX += rs ;
		        len = rs ;
	            }
	        } else {
	            char	dbuf[dlen+1] ;
	            if ((rs = ctxxx(dbuf,dlen,v)) >= 0) {
	                len = rs ;
	                rs = sbuf_addstrw(sbp,dbuf,len) ;
	            }
	        } /* end if */
	    } /* end if (not in error mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine-template (sbuf_xxxx) */

template<typename T>
int sbuf_binx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,ctbin,v) ;
}
/* end subroutine-template (sbuf_binx) */

template<typename T>
int sbuf_octx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,ctoct,v) ;
}
/* end subroutine-template (sbuf_octx) */

template<typename T>
int sbuf_decx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,ctdec,v) ;
}
/* end subroutine-template (sbuf_decx) */

template<typename T>
int sbuf_hexx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,cthex,v) ;
}
/* end subroutine-template (sbuf_hexx) */


/* local variables */

static blanker			bo ;	/* so-valled "blank" object */


/* exported variables */


/* exported subroutines */

int sbuf_start(sbuf *sbp,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (sbp && dbuf) {
	    rs = SR_INVALID ;
	    if (dlen > 0) {
	        rs = SR_OK ;
	        dbuf[0] = '\0' ;
	        SBUF_RBUF = dbuf ;
	        SBUF_RLEN = dlen ;
	        SBUF_INDEX = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_start) */

int sbuf_finish(sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        SBUF_RBUF = nullptr ;
	        SBUF_RLEN = 0 ;
	        SBUF_INDEX = SR_NOTOPEN ;
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_finish) */

int sbuf_reset(sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    char	*bp = SBUF_RBUF ;
	    rs = SR_OK ;
	    SBUF_INDEX = 0 ;
	    *bp = '\0' ;
	}
	return rs ;
}
/* end subroutine (sbuf_reset) */

int sbuf_buf(sbuf *sbp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        char	*bp = (SBUF_RBUF + SBUF_INDEX) ;
	        if (SBUF_RLEN < 0) {
	            if (sl < 0) {
	                while (*sp) {
	                    *bp++ = *sp++ ;
		        }
	            } else {
	                while (sl > 0) {
	                    *bp++ = *sp++ ;
	                    sl -= 1 ;
	                }
	            } /* end if */
	        } else {
	            if (sl < 0) {
	                while (*sp && (bp < (SBUF_RBUF + SBUF_RLEN))) {
	                    *bp++ = *sp++ ;
		        }
	            } else {
	                while ((sl > 0) && (bp < (SBUF_RBUF + SBUF_RLEN))) {
	                    *bp++ = *sp++ ;
	                    sl -= 1 ;
	                }
	            } /* end if */
	            if (*sp && (sl > 0)) rs = SR_OVERFLOW ;
	        } /* end if */
	        *bp = '\0' ;
	        len = (bp - (SBUF_RBUF + SBUF_INDEX)) ;
	        SBUF_INDEX = (rs >= 0) ? (bp - SBUF_RBUF) : rs ;
	    } /* end if (not in error-mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_buf) */

int sbuf_strw(sbuf *sbp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sbp && sp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		rs = sbuf_addstrw(sbp,sp,sl) ;
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_strw) */

int sbuf_strs(sbuf *bp,int sch,cchar **sv) noex {
	int		rs = SR_FAULT ;
	if (bp && sv) {
	    rs = SR_OK ;
	    for (int i = 0 ; sv[i] ; i += 1) {
	        cchar	*sp = sv[i] ;
		if (sp[0]) {
	            if (sch && (i > 0)) rs = sbuf_chr(bp,sch) ;
	            if (rs >= 0) rs = sbuf_strw(bp,sp,-1) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null vector) */
	return rs ;
}
/* end subroutine (sbuf_strs) */

int sbuf_bini(sbuf *sbp,int v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_bini) */

int sbuf_binl(sbuf *sbp,long v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_binl) */

int sbuf_binll(sbuf *sbp,longlong v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_binll) */

int sbuf_binui(sbuf *sbp,uint v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_binui) */

int sbuf_binul(sbuf *sbp,ulong v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_binul) */

int sbuf_binull(sbuf *sbp,ulonglong v) noex {
	return sbuf_binx(sbp,v) ;
}
/* end subroutine (sbuf_binull) */

int sbuf_octi(sbuf *sbp,int v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octi) */

int sbuf_octl(sbuf *sbp,long v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octl) */

int sbuf_octll(sbuf *sbp,longlong v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octll) */

int sbuf_octui(sbuf *sbp,uint v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octui) */

int sbuf_octul(sbuf *sbp,ulong v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octul) */

int sbuf_octull(sbuf *sbp,ulonglong v) noex {
	return sbuf_octx(sbp,v) ;
}
/* end subroutine (sbuf_octull) */

int sbuf_deci(sbuf *sbp,int v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_deci) */

int sbuf_decl(sbuf *sbp,long v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_decl) */

int sbuf_decll(sbuf *sbp,longlong v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_decll) */

int sbuf_decui(sbuf *sbp,uint v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_decui) */

int sbuf_decul(sbuf *sbp,ulong v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_decul) */

int sbuf_decull(sbuf *sbp,ulonglong v) noex {
	return sbuf_decx(sbp,v) ;
}
/* end subroutine (sbuf_decull) */

int sbuf_hexc(sbuf *sbp,char v) noex {
	uint		uv = uint(v) ;
	return sbuf_hexuc(sbp,uv) ;
}
/* end subroutine (sbuf_hexc) */

int sbuf_hexi(sbuf *sbp,int v) noex {
	uint		uv = uint(v) ;
	return sbuf_hexx(sbp,uv) ;
}
/* end subroutine (sbuf_hexi) */

int sbuf_hexl(sbuf *sbp,long v) noex {
	ulong		uv = ulong(v) ;
	return sbuf_hexx(sbp,uv) ;
}
/* end subroutine (sbuf_hexl) */

int sbuf_hexll(sbuf *sbp,longlong v) noex {
	ulonglong	uv = ulonglong(v) ;
	return sbuf_hexx(sbp,uv) ;
}
/* end subroutine (sbuf_hexll) */

int sbuf_hexuc(sbuf *sbp,uchar v) noex {
	cint		hlen = (2*sizeof(uchar)) ; /* unsigned character */
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		int	bl = (SBUF_RLEN-SBUF_INDEX) ;
		rs = SR_OVERFLOW ;
	        if (bl >= hlen) {
	            char	*bp = (SBUF_RBUF+SBUF_INDEX) ;
	            if ((rs = cthexuc(bp,bl,v)) >= 0) {
		        SBUF_INDEX += rs ;
		        len = rs ;
	            }
	        }
	    } /* end if (not in error mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_hexuc) */

int sbuf_hexui(sbuf *sbp,uint v) noex {
	return sbuf_hexx(sbp,v) ;
}
/* end subroutine (sbuf_hexui) */

int sbuf_hexul(sbuf *sbp,ulong v) noex {
	return sbuf_hexx(sbp,v) ;
}
/* end subroutine (sbuf_hexul) */

int sbuf_hexull(sbuf *sbp,ulonglong v) noex {
	return sbuf_hexx(sbp,v) ;
}
/* end subroutine (sbuf_hexull) */

int sbuf_chr(sbuf *sbp,int ch) noex {
	cint		len = 1 ;
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        int	bl = (SBUF_RLEN-SBUF_INDEX) ;
	        char	*bp = (SBUF_RBUF + SBUF_INDEX) ;
		rs = SR_OVERFLOW ;
	        if (bl >= len) {
	            *bp++ = ch ;
	            *bp = '\0' ;
	            SBUF_INDEX += len ;
	            rs = SR_OK ;
	        } else {
	            SBUF_INDEX = rs ;
	        }
	    } /* end if (not in error mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_chr) */

/* store a character (n-times) */
int sbuf_chrs(sbuf *sbp,int ch,int len) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		rs = SR_INVALID ;
		if (len >= 0) {
		    int		bl = (SBUF_RLEN - SBUF_INDEX) ;
		    char	*bp = (SBUF_RBUF + SBUF_INDEX) ;
		    rs = SR_OVERFLOW ;
	            if (bl >= len) {
	                for (int i = 0 ; i < len ; i += 1) {
		            *bp++ = ch ;
	                }
	                SBUF_INDEX += len ;
	                rs = SR_OK ;
	                *bp = '\0' ;
	            } else {
	                SBUF_INDEX = rs ;
	            }
		} /* end if (valid) */
	   } /* end if (not in error-mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_chrs) */

int sbuf_blanks(sbuf *sbp,int n) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		if (n >= 0) {
	            while ((rs >= 0) && (len < n)) {
	                cint	ml = min((n - len),bo.l) ;
	                rs = sbuf_addstrw(sbp,bo.p,ml) ;
	                len += rs ;
	            } /* end while */
		} /* end if (valid) */
	    } /* end if (not in error-mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_blanks) */

int sbuf_vprintf(sbuf *sbp,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp && fmt && ap) {
	    rs = SR_INVALID ;
	    if (fmt[0]) {
	        if ((rs = SBUF_INDEX) >= 0) {
	            cint	fm = 0x01 ; /* *will* error out on overflow! */
	            cint	dl = (SBUF_RLEN - SBUF_INDEX) ;
	            char	*dp = (SBUF_RBUF + SBUF_INDEX) ;
	            if ((rs = format(dp,dl,fm,fmt,ap)) >= 0) {
	                len = rs ;
		        SBUF_INDEX += len ;
	            } else {
		        SBUF_INDEX = rs ;
	            }
	        } /* end if (not error mode) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_vprintf) */

/* PRINTFLIKE2 */
int sbuf_printf(sbuf *op,cchar *fmt,...) noex {
	int		rs ;
	va_list		ap ;
	{
	    va_begin(ap,fmt) ;
	    rs = sbuf_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (sbuf_printf) */

int sbuf_adv(sbuf *sbp,int adv,char **dpp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if (dpp) *dpp = nullptr ;
	    if ((rs = SBUF_INDEX) >= 0) {
	        if ((SBUF_RLEN - SBUF_INDEX) >= adv) {
		    if (dpp != nullptr) *dpp = (SBUF_RBUF + SBUF_INDEX) ;
		    SBUF_INDEX += adv ;
	        } else {
		    rs = SR_TOOBIG ;
	        }
	    } /* end if (not error mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? adv : rs ;
}
/* end subroutine (sbuf_adv) */

/* get the remaining length in the buffer */
int sbuf_rem(sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        rs = (SBUF_RLEN-SBUF_INDEX) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sbuf_rem) */

/* get the length filled so far */
int sbuf_getlen(sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    rs = SBUF_INDEX ;
	}
	return rs ;
}
/* end subroutine (sbuf_getlen) */

/* get the length filled so far */
int sbuf_getbuf(sbuf *sbp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if (rpp) *rpp = sbp->rbuf ;
	    rs = SBUF_INDEX ;
	}
	return rs ;
}
/* end subroutine (sbuf_getbuf) */

/* get the pointer in the buffer to the next character */
int sbuf_getpoint(sbuf *sbp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    rs = SBUF_INDEX ;
	    if (rpp) {
	        *rpp = (SBUF_RBUF + SBUF_INDEX) ;
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_getpoint) */

/* get (retrieve) the previous character (if there is one) */
int sbuf_getprev(sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        rs = mkchar(SBUF_RBUF[SBUF_INDEX - 1]) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (sbuf_getprev) */

int sbuf::start(char *rp,int rl) noex {
	return sbuf_start(this,rp,rl) ;
}
/* end subroutine (sbuf::start) */

int sbuf::chrs(int ch,int nc) noex {
	return sbuf_chrs(this,ch,nc) ;
}
/* end subroutine (sbuf::chrs) */

void sbuf::dtor() noex {
	cint		rs = sbuf_finish(this) ;
	if (rs < 0) {
	   ulogerror("sbuf",rs,"dtor-finish") ;
	}
}
/* end subroutine (sbuf::dtor) */

int sbuf::hexp(uint64_t v,int n) noex {
    	return sbuf_hexp(this,v,n) ;
}

int sbuf::decl(long v) noex {
    	return sbuf_decl(this,v) ;
}


/* private subroutines */

static int sbuf_addstrw(sbuf *sbp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*bp = (SBUF_RBUF + SBUF_INDEX) ;
	if (SBUF_RLEN < 0) {
	    if (sl < 0) {
	        while (*sp) {
	            *bp++ = *sp++ ;
		}
	    } else {
	        while (sl && *sp) {
	            *bp++ = *sp++ ;
	            sl -= 1 ;
	        }
	    } /* end if */
	} else {
	    if (sl < 0) {
	        while (*sp && (bp < (SBUF_RBUF + SBUF_RLEN))) {
	            *bp++ = *sp++ ;
		}
	        if (*sp) rs = SR_OVERFLOW ;
	    } else {
	        while (sl && *sp && (bp < (SBUF_RBUF + SBUF_RLEN))) {
	            *bp++ = *sp++ ;
	            sl -= 1 ;
	        }
	        if (*sp && (sl > 0)) rs = SR_OVERFLOW ;
	    } /* end if */
	} /* end if */
	*bp = '\0' ;
	len = (bp - (SBUF_RBUF + SBUF_INDEX)) ;
	SBUF_INDEX = (rs >= 0) ? (bp - SBUF_RBUF) : rs ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_addstrw) */

int sbuf_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case sbufmem_deci:
		rs = sbuf_deci(op,a) ;
		break ;
	    case sbufmem_hexc:
		rs = sbuf_hexc(op,a) ;
		break ;
	    case sbufmem_hexi:
		rs = sbuf_hexi(op,a) ;
		break ;
	    case sbufmem_chr:
		rs = sbuf_chr(op,a) ;
		break ;
	    case sbufmem_blanks:
		rs = sbuf_blanks(op,a) ;
		break ;
	    case sbufmem_rem:
		rs = sbuf_rem(op) ;
		break ;
	    case sbufmem_getlen:
		rs = sbuf_getlen(op) ;
		break ;
	    case sbufmem_getprev:
		rs = sbuf_getprev(op) ;
		break ;
	    case sbufmem_reset:
		rs = sbuf_reset(op) ;
		break ;
	    case sbufmem_finish:
		rs = sbuf_finish(op) ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (sbuf_co::operator) */


