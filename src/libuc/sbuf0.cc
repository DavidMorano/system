/* sbuf SUPPORT */
/* lang=C++20 */

/* storage buffer (SBuf) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

	= 2016-11-06, David A­D­ Morano
        I did some optimization for the method |sbuf_decX()| and |sbuf_hexX()|
        to form the digits in the object buffer directly rather than in a
        separate buffer and then copying the data into the object buffer.

	= 2017-11-06, David A­D­ Morano
        I enhanced the |sbuf_hexXX()| methods using the property that the length
        (in bytes) of the result is known ahead of time. We cannot do this
	(really quickly) with decimal conversions.

*/

/* Copyright © 1998,2016,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sbuf

	Description:
        This module can be used to construct strings or messages in buffers
        WITHOUT using the 'sprint(3c)' subroutine or something similar.

        This module is useful when the user SUPPLIES a buffer of a specified
        length to the object at object initialization.

        This module uses an object, that must be initialized and eventually
        freed, to track the state of the user supplied buffer.

	Arguments:
	bop		pointer to the buffer object
	<others>	depending on the call made

	Returns:
	>=0		amount of new space used by the newly stored item
			(not including any possible trailing NUL characters)
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<cstdarg>
#include	<algorithm>
#include	<usystem.h>
#include	<format.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<localmisc.h>

#include	"sbuf.h"


/* local defines */

#define	SBUF_RBUF	(sbp->rbuf)
#define	SBUF_RLEN	(sbp->rlen)
#define	SBUF_INDEX	(sbp->index)

#ifndef	MKCHAR
#define	MKCHAR(ch)	((ch) & UCHAR_MAX)
#endif


/* local namespaces */

using std::min ;


/* external subroutines */


/* local structures */


/* forward references */

int		sbuf_buf(sbuf *,const char *,int) noex ;
int		sbuf_vprintf(sbuf *,const char *,va_list) noex ;

static int	sbuf_addstrw(sbuf *,const char *,int) noex ;


/* local variables */

static cchar	blanks[] = "        " ;


/* local subroutine-templates */

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
int sbuf_decx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,ctdec,v) ;
}
/* end subroutine-template (sbuf_decx) */

template<typename T>
int sbuf_hexx(sbuf *sbp,T v) noex {
	return sbuf_xxxx(sbp,cthex,v) ;
}
/* end subroutine-template (sbuf_hexx) */


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
	        SBUF_RBUF = NULL ;
	        SBUF_RLEN = 0 ;
	        SBUF_INDEX = SR_NOTOPEN ;
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_finish) */

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
	            if (sch && (i > 0)) rs = sbuf_char(bp,sch) ;
	            if (rs >= 0) rs = sbuf_strw(bp,sp,-1) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null vector) */
	return rs ;
}
/* end subroutine (sbuf_strs) */

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

int sbuf_hexc(sbuf *sbp,int v) noex {
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

int sbuf_hexuc(sbuf *sbp,uint v) noex {
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

int sbuf_char(sbuf *sbp,int ch) noex {
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
/* end subroutine (sbuf_char) */

/* store a character (n-times) */
int sbuf_nchar(sbuf *sbp,int len,int ch) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		rs = SR_INVALID ;
		if (len >= 0) {
		    int		bl = (SBUF_RLEN-SBUF_INDEX) ;
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
/* end subroutine (sbuf_nchar) */

int sbuf_blanks(sbuf *sbp,int n) noex {
	cint		nblank = sizeof(blanks) ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp) {
	    if ((rs = SBUF_INDEX) >= 0) {
		if (n >= 0) {
	            while ((rs >= 0) && (len < n)) {
	                cint	m = min(nblank,(n-len)) ;
	                rs = sbuf_addstrw(sbp,blanks,m) ;
	                len += m ;
	            } /* end while */
		} /* end if (valid) */
	    } /* end if (not in error-mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_blanks) */

/* vprintf-like thing */
int sbuf_vprintf(sbuf *sbp,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (sbp && fmt) {
	    if ((rs = SBUF_INDEX) >= 0) {
	        cint	fm = 0x01 ; /* *will* error out on overflow! */
	        int	dl = (SBUF_RLEN - SBUF_INDEX) ;
	        char	*dp = (SBUF_RBUF + SBUF_INDEX) ;
	        if ((rs = format(dp,dl,fm,fmt,ap)) >= 0) {
	            len = rs ;
		    SBUF_INDEX += len ;
	        } else {
		    SBUF_INDEX = rs ;
	        }
	    } /* end if (not error mode) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sbuf_vprintf) */

/* PRINTFLIKE2 */
int sbuf_printf(sbuf *op,const char *fmt,...) noex {
	int		rs ;
	{
	    va_list	ap ;
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
	    if (dpp) *dpp = NULL ;
	    if ((rs = SBUF_INDEX) >= 0) {
	        if ((SBUF_RLEN - SBUF_INDEX) >= adv) {
		    if (dpp != NULL) *dpp = (SBUF_RBUF + SBUF_INDEX) ;
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
	        rs = MKCHAR(SBUF_RBUF[SBUF_INDEX - 1]) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (sbuf_getprev) */


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


