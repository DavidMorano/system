/* buffer_main SUPPORT */
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

	This module can be used to construct strings or messages
	in buffers WITHOUT using the 'sprint' subroutine.

	This module is useful when the user does NOT supply a buffer
	to be used as the working store.  Instead, a dynamically
	grown and managed buffer is maintained within the object.

	This module uses an object, that must be initialized and
	eventually freed, to track the state of the dynamically
	used internal buffer.  An exponential growth is used for
	increasing the buffer size as needed.

	Arguments:
	- bop		pointer to the buffer object
	- <others>

	Returns:
	>=0		the total length of the filled up buffer so far!
	<0		error

	Note:
	This module is not completely protected against calling
	methods when the object has not been initialized already.
	This is a little hit for "performance reasons" but the
	benefits are really questionable given how cheap it is to
	check for an uninitialized object!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<stdintx.h>
#include	<format.h>
#include	<strwcpy.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<libmallocxx.h>
#include	<localmisc.h>

#include	"buffer.h"


/* local defines */

#define	BUFFER_STARTLEN	50		/* starting buffer length */

#ifndef	CF_BUFSTART
#define	CF_BUFSTART	0
#endif

#ifndef	CF_FASTGROW
#define	CF_FASTGROW	0
#endif


/* imported namespaces */

using std::min ;
using std::max ;


/* local typedefs */


/* external subroutines */


/* forward references */

template<typename ... Args>
static inline int buffer_ctor(buffer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->buf = nullptr ;
	    op->startlen = 0 ;
	    op->len = 0 ;
	    op->e = 0 ;
	}
	return rs ;
}

static int	buffer_ext(buffer *,int) noex ;


/* local subroutine-templates */

template<typename T>
int buffer_xxxx(buffer *op,int (*ctxxx)(char *,int,T),T v) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    if ((rs = buffer_ext(op,dlen)) >= 0) {
	        char	*bp = (op->buf + op->len) ;
	        rs = ctxxx(bp,dlen,v) ;
	        op->len += rs ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine-template (buffer_xxxx) */

template<typename T>
int buffer_decx(buffer *sbp,T v) noex {
	return buffer_xxxx(sbp,ctdec,v) ;
}
/* end subroutine-template (buffer_decx) */

template<typename T>
int buffer_hexx(buffer *sbp,T v) noex {
	return buffer_xxxx(sbp,cthex,v) ;
}
/* end subroutine-template (buffer_hexx) */


/* local variables */

constexpr bool		f_bufstart = CF_BUFSTART ;
constexpr bool		f_fastgrow = CF_FASTGROW ;


/* exported subroutines */

int buffer_start(buffer *op,int startlen) noex {
	int		rs ;
	if ((rs = buffer_ctor(op)) >= 0) {
	    op->startlen = max(startlen,BUFFER_STARTLEN) ;
	    if constexpr (f_bufstart) {
	        if ((rs = buffer_ext(op,-1)) >= 0) {
	            op->buf[0] = '\0' ;
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
	    if (op->buf) {
	        rs1 = uc_libfree(op->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->buf = nullptr ;
	    }
	    len = op->len ;
	    op->e = 0 ;
	    op->startlen = 0 ;
	    op->len = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_finish) */

int buffer_reset(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->len) >= 0) {
	        op->len = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (buffer_reset) */

int buffer_adv(buffer *op,int advlen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->len) >= 0) {
		rs = SR_INVALID ;
	        if (advlen >= 0) {
	            if ((rs = buffer_ext(op,advlen)) >= 0) {
	                op->len += advlen ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? advlen : rs ;
}
/* end subroutine (buffer_adv) */

int buffer_char(buffer *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->len) >= 0) {
	        if ((rs = buffer_ext(op,1)) >= 0) {
	            op->buf[(op->len)++] = ch ;
	            op->buf[op->len] = '\0' ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (buffer_char) */

int buffer_buf(buffer *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (op && sbuf) {
	    if ((rs = op->len) >= 0) {
	        if (slen < 0) slen = strlen(sbuf) ;
	        if ((rs = buffer_ext(op,slen)) >= 0) {
	            char	*bp = (op->buf + op->len) ;
	            memcpy(bp,sbuf,slen) ;
	            op->len += slen ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? slen : rs ;
}
/* end subroutine (buffer_buf) */

int buffer_strw(buffer *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && sbuf) {
	    if ((rs = op->len) >= 0) {
	        if (slen < 0) slen = strlen(sbuf) ;
	        if ((rs = buffer_ext(op,slen)) >= 0) {
	            char	*bp = (op->buf + op->len) ;
	            len = strwcpy(bp,sbuf,slen) - bp ;
	            op->len += len ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strw) */

int buffer_vprintf(buffer *op,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && fmt) {
	    char	*lbuf ;
	    if ((rs = libmalloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if ((rs = format(lbuf,llen,0x01,fmt,ap)) >= 0) {
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
	int		rs = SR_FAULT ;
	if (op && fmt) {
	    va_list	ap ;
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
	    len = op->len ;
	    if (spp) {
	        if ((rs = buffer_ext(op,1)) >= 0) {
	            *spp = (rs >= 0) ? op->buf : nullptr ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_get) */

int buffer_getprev(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = op->len) > 0) {
	        if ((rs = buffer_ext(op,1)) >= 0) {
	            rs = mkchar(op->buf[op->len-1]) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_getprev) */

int buffer_deci(buffer *sbp,int v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_deci) */

int buffer_decl(buffer *sbp,long v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_decl) */

int buffer_decll(buffer *sbp,longlong v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_decll) */

int buffer_decui(buffer *sbp,uint v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_decui) */

int buffer_decul(buffer *sbp,ulong v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_decul) */

int buffer_decull(buffer *sbp,ulonglong v) noex {
	return buffer_decx(sbp,v) ;
}
/* end subroutine (buffer_decull) */

int buffer_hexc(buffer *sbp,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexuc(sbp,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexi(buffer *sbp,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexi) */

int buffer_hexl(buffer *sbp,long v) noex {
	ulong		uv = ulong(v) ;
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexl) */

int buffer_hexll(buffer *sbp,longlong v) noex {
	ulonglong	uv = ulonglong(v) ;
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexll) */

int buffer_hexuc(buffer *sbp,int uv) noex {
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexui(buffer *sbp,uint uv) noex {
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexui) */

int buffer_hexul(buffer *sbp,ulong uv) noex {
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexul) */

int buffer_hexull(buffer *sbp,ulonglong uv) noex {
	return buffer_hexx(sbp,uv) ;
}
/* end subroutine (buffer_hexull) */


/* private subroutines */

static int buffer_ext(buffer *op,int req) noex {
	int		rs = SR_OK ;
	int		need ;
	int		ne ;
	if (req < 0) req = op->startlen ;
	need = ((op->len + (req+1)) - op->e) ;
	if (need > 0) {
	    char	*buf{} ;
	    if (op->buf) {
	        ne = max(op->startlen,need) ;
	        if ((rs = uc_libmalloc(ne,&buf)) >= 0) {
	            op->buf = buf ;
		    op->e = ne ;
	        } else {
	            op->len = rs ;
	        }
	    } else {
		ne = op->e ;
	        while ((op->len + (req+1)) > ne) {
		    if constexpr (f_fastgrow) {
	                ne = ((ne + 1) * 2) ;
		    } else {
	                ne = (ne + BUFFER_STARTLEN) ;
		    }
	        } /* end while */
	        if ((rs = uc_librealloc(op->buf,ne,&buf)) >= 0) {
	            op->buf = buf ;
		    op->e = ne ;
	        } else {
	            op->len = rs ;
		}
	    } /* end if */
	} /* end if (extension needed) */
	return rs ;
}
/* end subroutine (buffer_ext) */


