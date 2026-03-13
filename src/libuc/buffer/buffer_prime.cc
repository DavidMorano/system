/* buffer_prime SUPPORT */
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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
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

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ulibvals ;			/* |maxlinelen(3u)| */

/* local defines */

#ifndef	CF_BUFSTART
#define	CF_BUFSTART	0
#endif

#ifndef	CF_FASTGROW
#define	CF_FASTGROW	0
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using buffer_ns::buffer_ext ;		/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int buffer_ctor(buffer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->dbuf = nullptr ;
	    op->startlen = 0 ;
	    op->clen = 0 ;
	    op->dlen = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end method (buffer_ctor) */


/* local variables */

static cint		maxlinelen = ulibval.maxline ;

constexpr bool		f_bufstart = CF_BUFSTART ;


/* exported variables */


/* exported subroutines */

int buffer_start(buffer *op,int startlen) noex {
	int		rs ;
	if ((rs = buffer_ctor(op)) >= 0) ylikely {
	    op->startlen = max(startlen,BUFFER_STARTLEN) ;
	    if_constexpr (f_bufstart) {
	        if ((rs = buffer_ext(op,-1)) >= 0) {
	            op->dbuf[0] = '\0' ;
	        }
	    } /* end if_constexpr (f_bufstart) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_start) */

int buffer_finish(buffer *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dbuf) {
	        rs1 = libmem.free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    len = op->clen ;
	    op->dlen = 0 ;
	    op->startlen = 0 ;
	    op->clen = 0 ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_finish) */

int buffer_reset(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = op->clen) > 0) ylikely {
	        op->clen = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_reset) */

int buffer_adv(buffer *op,int advlen) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = op->clen) >= 0) ylikely {
		rs = SR_INVALID ;
	        if (advlen >= 0) ylikely {
	            if ((rs = buffer_ext(op,advlen)) >= 0) ylikely {
	                op->clen += advlen ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? advlen : rs ;
}
/* end subroutine (buffer_adv) */

int buffer_strw(buffer *op,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && sp) ylikely {
	    if ((rs = op->clen) >= 0) ylikely {
		if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	            if ((rs = buffer_ext(op,sl)) >= 0) ylikely {
	                char	*bp = (op->dbuf + op->clen) ;
	                len = intconv(strwcpy(bp,sp,sl) - bp) ;
	                op->clen += len ;
	            } /* end if (buffer_ext) */
		} /* end if (getlenstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_strw) */

int buffer_chr(buffer *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = op->clen) >= 0) ylikely {
	        if ((rs = buffer_ext(op,1)) >= 0) ylikely {
	            op->dbuf[(op->clen)++] = char(ch) ;
	            op->dbuf[op->clen] = '\0' ;
	        } /* end if (buffer_ext) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (buffer_chr) */

int buffer_buf(buffer *op,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (op && sp) ylikely {
	    if ((rs = op->clen) >= 0) ylikely {
		if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	            if ((rs = buffer_ext(op,sl)) >= 0) ylikely {
	                char	*bp = (op->dbuf + op->clen) ;
	                memcopy(bp,sp,sl) ;
	                op->clen += sl ;
			rl = sl ;
	            } /* end if (buffer_ext) */
		} /* end if (getlenstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (buffer_buf) */

int buffer_vprintf(buffer *op,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	if (op && fmt) ylikely {
	    if ((rs = maxlinelen) >= 0) ylikely {
		cint	llen = rs ;
	        if (char *lbuf ; (rs = libmem.mall((llen + 1),&lbuf)) >= 0) {
	            if ((rs = fmtstr(lbuf,llen,0x01,fmt,ap)) >= 0) ylikely {
	                rs = buffer_strw(op,lbuf,rs) ;
			rl = rs ;
	            }
		    rs1 = libmem.free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxlinelen) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (buffer_vprintf) */

/* PRINTFLIKE2 */
int buffer_printf(buffer *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (op && fmt) ylikely {
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
	if (op && spp) ylikely {
	    len = op->clen ;
	    if (spp) {
	        if ((rs = buffer_ext(op,1)) >= 0) ylikely {
	            *spp = (rs >= 0) ? op->dbuf : nullptr ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_get) */

int buffer_getprev(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = op->clen) > 0) ylikely {
		cint bl = op->clen ;
		char *bp = op->dbuf ;
		rs = mkchar(bp[bl - 1]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buffer_getprev) */

int buffer_len(buffer *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->clen ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (buffer_len) */


/* private subroutines */

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


