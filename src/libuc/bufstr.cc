/* bufstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufstr

	Description:
	This module can be used to construct strings or messages
	in buffers WITHOUT using the |snsprint(3c)| subroutine.  This
	module is useful when the user does NOT supply a buffer to
	be used as the working store.  Rather, an internally,
	dynamically grown and managed buffer is maintained.  This
	module uses an object, that must be initialized and eventually
	freed, to track the state of the dynamically used internal
	buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"bufstr.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bufstr_extend(bufstr *,int,char ** = nullptr) noex ;

template<typename T>
static inline int bufstr_xxxx(bufstr *op,int (*ctxxx)(char *,int,T),T v) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    if ((rs = bufstr_extend(op,dlen)) >= 0) ylikely {
	        char	*bp = (op->dbuf + op->len) ;
	        rs = ctxxx(bp,dlen,v) ;
	        op->len += rs ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine-template (bufstr_xxxx) */

template<typename T>
static inline int bufstr_binx(bufstr *sbp,T v) noex {
	return bufstr_xxxx(sbp,ctbin,v) ;
}
/* end subroutine-template (bufstr_binx) */

template<typename T>
static inline int bufstr_octx(bufstr *sbp,T v) noex {
	return bufstr_xxxx(sbp,ctoct,v) ;
}
/* end subroutine-template (bufstr_octx) */

template<typename T>
static inline int bufstr_decx(bufstr *sbp,T v) noex {
	return bufstr_xxxx(sbp,ctdec,v) ;
}
/* end subroutine-template (bufstr_decx) */

template<typename T>
static inline int bufstr_hexx(bufstr *sbp,T v) noex {
	return bufstr_xxxx(sbp,cthex,v) ;
}
/* end subroutine-template (bufstr_hexx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int bufstr_start(bufstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->len = 0 ;
	    op->dlen = 0 ;
	    op->dbuf = nullptr ;
	    op->sbuf[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (bufstr_start) */

int bufstr_finish(bufstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dbuf) {
	        rs1 = uc_free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    len = op->len ;
	    op->dlen = 0 ;
	    op->len = 0 ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_finish) */

int bufstr_strw(bufstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (char *bp ; (rs = bufstr_extend(op,sl,&bp)) >= 0) ylikely {
	        strwcpy(bp,sp,sl) ;
	        op->len += sl ;
	        len = op->len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_strw) */

int bufstr_chr(bufstr *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    char	buf[2] = { char(ch) } ;
	    rs = bufstr_strw(op,buf,1) ;
	}
	return rs ;
}
/* end subroutine (bufstr_chr) */

int bufstr_get(bufstr *op,cchar **spp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (spp) {
	        *spp = (op->dbuf) ? op->dbuf : op->sbuf ;
	    }
	    len = op->len ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bufstr_get) */

int bufstr_bini(bufstr *sbp,int v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_bini) */

int bufstr_binl(bufstr *sbp,long v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_binl) */

int bufstr_binll(bufstr *sbp,longlong v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_binll) */

int bufstr_binui(bufstr *sbp,uint v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_binui) */

int bufstr_binul(bufstr *sbp,ulong v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_binul) */

int bufstr_binull(bufstr *sbp,ulonglong v) noex {
	return bufstr_binx(sbp,v) ;
}
/* end subroutine (bufstr_binull) */

int bufstr_octi(bufstr *sbp,int v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octi) */

int bufstr_octl(bufstr *sbp,long v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octl) */

int bufstr_octll(bufstr *sbp,longlong v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octll) */

int bufstr_octui(bufstr *sbp,uint v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octui) */

int bufstr_octul(bufstr *sbp,ulong v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octul) */

int bufstr_octull(bufstr *sbp,ulonglong v) noex {
	return bufstr_octx(sbp,v) ;
}
/* end subroutine (bufstr_octull) */

int bufstr_deci(bufstr *sbp,int v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_deci) */

int bufstr_decl(bufstr *sbp,long v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_decl) */

int bufstr_decll(bufstr *sbp,longlong v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_decll) */

int bufstr_decui(bufstr *sbp,uint v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_decui) */

int bufstr_decul(bufstr *sbp,ulong v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_decul) */

int bufstr_decull(bufstr *sbp,ulonglong v) noex {
	return bufstr_decx(sbp,v) ;
}
/* end subroutine (bufstr_decull) */

int bufstr_hexi(bufstr *sbp,int v) noex {
	uint		uv = uint(v) ;
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexi) */

int bufstr_hexl(bufstr *sbp,long v) noex {
	ulong		uv = ulong(v) ;
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexl) */

int bufstr_hexll(bufstr *sbp,longlong v) noex {
	ulonglong	uv = ulonglong(v) ;
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexll) */

int bufstr_hexui(bufstr *sbp,uint uv) noex {
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexui) */

int bufstr_hexul(bufstr *sbp,ulong uv) noex {
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexul) */

int bufstr_hexull(bufstr *sbp,ulonglong uv) noex {
	return bufstr_hexx(sbp,uv) ;
}
/* end subroutine (bufstr_hexull) */


/* private subroutines */

static int bufstr_extend(bufstr *op,int nlen,char **rpp) noex {
	cint		slen = BUFSTR_LEN ;
	int		rs = SR_OK ;
	char		*dp{} ;
	if (op->dbuf) {
	    cint	rlen = (slen-op->len) ;
	    if (nlen > rlen) {
	    	cint	dlen = max((slen + nlen),(slen * 2)) ;
		if ((rs = uc_malloc((dlen+1),&dp)) >= 0) {
		    op->dlen = dlen ;
		    op->dbuf = dp ;
		    dp = strwcpy(dp,op->sbuf,op->len) ;
		}
	    } else {
		dp = (op->sbuf + op->len) ;
	    }
	} else {
	    cint	rlen = (op->dlen - op->len) ;
	    if (nlen > rlen) {
		cint	dlen = (op->dlen + nlen+slen) ;
		if ((rs = uc_realloc(op->dbuf,(dlen+1),&dp)) >= 0) {
		    op->dbuf = dp ;
		    op->dlen = dlen ;
		}
	    } else {
		dp = (op->dbuf + op->len) ;
	    }
	} /* end if (extension needed) */
	if (rpp) {
	    *rpp = (rs >= 0) ? dp : nullptr ;
	}
	return rs ;
}
/* end subroutine (bufstr_extend) */

int bufstr::strw(cchar *sp,int sl) noex {
	return bufstr_strw(this,sp,sl) ;
}

int bufstr::chr(int ch) noex {
	return bufstr_chr(this,ch) ;
}

int bufstr::get(cchar **rpp) noex {
	return bufstr_get(this,rpp) ;
}

void bufstr::dtor() noex {
	if (cint rs = bufstr_finish(this) ; rs < 0) {
	    ulogerror("bufstr",rs,"fini-finish") ;
	}
}

bufstr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case bufstrmem_start:
	        rs = bufstr_start(op) ;
	        break ;
	    case bufstrmem_strsize:
	        rs = bufstr_get(op,nullptr) ;
	        break ;
	    case bufstrmem_finish:
	        rs = bufstr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (bufstr_co::operator) */


