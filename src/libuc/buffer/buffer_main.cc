/* buffer_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */

#define	CF_BUFSTART	0		/* allcate buffer from start */
#define	CF_FASTGROW	1		/* grow (extend) faster */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	buffer

	Description:
	This module can be used to construct strings or messages
	in buffers WITHOUT using the 'sprint' subroutine.  This
	module is useful when the user does NOT supply a buffer to
	be used as the working store.  Instead, a dynamically grown
	and managed buffer is maintained within the object.  This
	module uses an object, that must be initialized and eventually
	freed, to track the state of the dynamically used internal
	buffer.  An exponential growth is used for increasing the
	buffer size as needed.

	Arguments:
	- bop		pointer to the buffer object
	- <others>

	Returns:
	>=0		the total length of the filled up buffer so far!
	<0		error (system-return)

	Note:
	This module is not completely protected against calling
	methods when the object has not been initialized already.
	This is a little hit for "performance reasons" but the
	benefits are really questionable given how cheap it is to
	check for an uninitialized object!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<stdintx.h>
#include	<fmtstr.h>
#include	<strwcpy.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"buffer.h"

import libutil ;

/* local defines */

#define	BUFFER_STARTLEN	50		/* starting buffer length */

#ifndef	CF_BUFSTART
#define	CF_BUFSTART	0
#endif

#ifndef	CF_FASTGROW
#define	CF_FASTGROW	0
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int buffer_ctor(buffer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->dbuf = nullptr ;
	    op->startlen = 0 ;
	    op->clen = 0 ;
	    op->dlen = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end method (buffer_ctor) */

static int	buffer_ext(buffer *,int) noex ;

template<typename T>
int buffer_xxxx(buffer *op,int (*ctxxx)(char *,int,T),T v) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    if ((rs = buffer_ext(op,dlen)) >= 0) {
	        char	*bp = (op->dbuf + op->clen) ;
	        rs = ctxxx(bp,dlen,v) ;
	        op->clen += rs ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine-template (buffer_xxxx) */

template<typename T>
int buffer_decx(buffer *op,T v) noex {
	return buffer_xxxx(op,ctdec,v) ;
}
/* end subroutine-template (buffer_decx) */

template<typename T>
int buffer_hexx(buffer *op,T v) noex {
	return buffer_xxxx(op,cthex,v) ;
}
/* end subroutine-template (buffer_hexx) */


/* local variables */

constexpr bool		f_bufstart = CF_BUFSTART ;
constexpr bool		f_fastgrow = CF_FASTGROW ;


/* exported variables */


/* exported subroutines */

int buffer_start(buffer *op,int startlen) noex {
	int		rs ;
	if ((rs = buffer_ctor(op)) >= 0) {
	    op->startlen = max(startlen,BUFFER_STARTLEN) ;
	    if_constexpr (f_bufstart) {
	        if ((rs = buffer_ext(op,-1)) >= 0) {
	            op->dbuf[0] = '\0' ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_start) */

int buffer_finish(buffer *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbuf) {
	        rs1 = uc_libfree(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    len = op->clen ;
	    op->dlen = 0 ;
	    op->startlen = 0 ;
	    op->clen = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_finish) */

int buffer_reset(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->clen) >= 0) {
	        op->clen = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (buffer_reset) */

int buffer_adv(buffer *op,int advlen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->clen) >= 0) {
		rs = SR_INVALID ;
	        if (advlen >= 0) {
	            if ((rs = buffer_ext(op,advlen)) >= 0) {
	                op->clen += advlen ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? advlen : rs ;
}
/* end subroutine (buffer_adv) */

int buffer_strw(buffer *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && sbuf) {
	    if ((rs = op->clen) >= 0) {
	        if (slen < 0) slen = lenstr(sbuf) ;
	        if ((rs = buffer_ext(op,slen)) >= 0) {
	            char	*bp = (op->dbuf + op->clen) ;
	            len = intconv(strwcpy(bp,sbuf,slen) - bp) ;
	            op->clen += len ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strw) */

int buffer_chr(buffer *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->clen) >= 0) {
	        if ((rs = buffer_ext(op,1)) >= 0) {
	            op->dbuf[(op->clen)++] = char(ch) ;
	            op->dbuf[op->clen] = '\0' ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (buffer_chr) */

int buffer_buf(buffer *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (op && sbuf) {
	    if ((rs = op->clen) >= 0) {
	        if (slen < 0) slen = lenstr(sbuf) ;
	        if ((rs = buffer_ext(op,slen)) >= 0) {
	            char	*bp = (op->dbuf + op->clen) ;
	            memcpy(bp,sbuf,slen) ;
	            op->clen += slen ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? slen : rs ;
}
/* end subroutine (buffer_buf) */

int buffer_vprintf(buffer *op,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && fmt) {
	    if (char *lbuf ; (rs = libmalloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if ((rs = fmtstr(lbuf,llen,0x01,fmt,ap)) >= 0) {
	            rs = buffer_strw(op,lbuf,rs) ;
	        }
		rs1 = uc_libfree(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_vprintf) */

/* PRINTFLIKE2 */
int buffer_printf(buffer *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (op && fmt) {
	    va_begin(ap,fmt) ;
	    rs = buffer_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (buffer_printf) */

/* get the address of the byte array from the buffer object */
int buffer_get(buffer *op,cchar **spp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && spp) {
	    len = op->clen ;
	    if (spp) {
	        if ((rs = buffer_ext(op,1)) >= 0) {
	            *spp = (rs >= 0) ? op->dbuf : nullptr ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_get) */

int buffer_getprev(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->clen) > 0) {
		cint bl = op->clen ;
		char *bp = op->dbuf ;
		rs = mkchar(bp[bl - 1]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_getprev) */

int buffer_deci(buffer *op,int v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_deci) */

int buffer_decl(buffer *op,long v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decl) */

int buffer_decll(buffer *op,longlong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decll) */

int buffer_decui(buffer *op,uint v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decui) */

int buffer_decul(buffer *op,ulong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decul) */

int buffer_decull(buffer *op,ulonglong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decull) */

int buffer_hexc(buffer *op,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexuc(op,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexi(buffer *op,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexi) */

int buffer_hexl(buffer *op,long v) noex {
	ulong		uv = ulong(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexl) */

int buffer_hexll(buffer *op,longlong v) noex {
	ulonglong	uv = ulonglong(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexll) */

int buffer_hexuc(buffer *op,int uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexui(buffer *op,uint uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexui) */

int buffer_hexul(buffer *op,ulong uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexul) */

int buffer_hexull(buffer *op,ulonglong uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexull) */

int buffer_len(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->clen ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (buffer_len) */


/* private subroutines */

static int buffer_ext(buffer *op,int req) noex {
	int		rs = SR_OK ;
	int		need ; /* used-below */
	if (req < 0) req = op->startlen ;
	need = ((op->clen + (req + 1)) - op->dlen) ;
	if (need > 0) {
	    int		nlen ; /* used-multiple */
	    char	*nbuf{} ; /* used-multiple */
	    if (op->dbuf) {
	        nlen = max(op->startlen,need) ;
	        if ((rs = uc_libmalloc((nlen + 1),&nbuf)) >= 0) {
	            op->dbuf = nbuf ;
		    op->dlen = nlen ;
	        } else {
	            op->clen = rs ;
	        }
	    } else {
		nlen = op->dlen ;
	        while ((op->clen + (req + 1)) > nlen) {
		    if_constexpr (f_fastgrow) {
	                nlen = ((nlen + 1) * 2) ;
		    } else {
	                nlen = (nlen + BUFFER_STARTLEN) ;
		    }
	        } /* end while */
	        if ((rs = uc_librealloc(op->dbuf,nlen,&nbuf)) >= 0) {
	            op->dbuf = nbuf ;
		    op->dlen = nlen ;
	        } else {
	            op->clen = rs ;
		}
	    } /* end if */
	} /* end if (extension needed) */
	return rs ;
}
/* end subroutine (buffer_ext) */

int buffer::adv(int v) noex {
	return buffer_adv(this,v) ;
}

int buffer::strw(cchar *sp,int sl) noex {
	return buffer_strw(this,sp,sl) ;
}

int buffer::chr(int ch) noex {
	return buffer_chr(this,ch) ;
}

int buffer::buf(cchar *sbuf,int slen) noex {
	return buffer_buf(this,sbuf,slen) ;
}

int buffer::get(cchar **rpp) noex {
	return buffer_get(this,rpp) ;
}

void buffer::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("buffer",rs,"fini-finish") ;
	}
} /* end method (buffer::dtor) */

int buffer_co::operator () (int v) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case buffermem_start:
	        rs = buffer_start(op,v) ;
	        break ;
	    case buffermem_strsize:
	        rs = buffer_get(op,nullptr) ;
	        break ;
	    case buffermem_reset:
	        rs = buffer_reset(op) ;
	        break ;
	    case buffermem_len:
	        rs = buffer_len(op) ;
	        break ;
	    case buffermem_getprev:
	        rs = buffer_getprev(op) ;
	        break ;
	    case buffermem_finish:
	        rs = buffer_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (buffer_co::operator) */


