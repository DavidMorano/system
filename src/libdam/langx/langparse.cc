/* langparse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Language-Parse (replacement for LANGSTATE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	several others that I have made like it in the past.  The
	problem that this code is *supposed* to solve has been
	handled (poorly) for a long time now.

	= 2018-12-14, David A-D- Morano
	Made a small enhancement for error handling.

*/

/* Copyright @ 2016,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	langparse

	Names:
	langparse_start
	langparse_load
	langparse_read
	langparse_finish

	Description:
	This object facilitates the parsing of C and C++ language
	source text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<six.h>			/* |sichr(2uc)| */
#include	<shortq.h>
#include	<strwcpy.h>
#include	<digval.h>		/* |digvalhex(3uc)| */
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"langparse.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef shortq *	obufp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int langparse_ctor(langparse *op,Args ... args) noex {
	LANGPARSE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langparse_ctor) */

local int langparse_dtor(langparse *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langparse_dtor) */

template<typename ... Args>
local inline int langparse_magic(langparse *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LANGPARSE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (langparse_magic) */

local int	langparse_proc(langparse *,int) noex ;
local int	langparse_add(langparse *,cchar *,int) noex ;
local int	langparse_add(langparse *,int) noex ;


/* local variables */

cint		nstage = LANGPARSE_NSTAGE ;


/* exported variables */


/* exported subroutines */

int langparse_start(langparse *op,int f_space) noex {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs ;
	if ((rs = langparse_ctor(op)) >= 0) {
	    if (shortq *obp ; (obp = new(nt) shortq) != np) {
		if ((rs = obp->start) >= 0) {
	            op->outbuf = obp ;
	            op->magic = LANGPARSE_MAGIC ;
		}
		if (rs < 0) {
		    delete obp ;
		} /* end if (error) */
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if (new-shortq) */
	    if (rs < 0) {
		langparse_dtor(op) ;
	    }
	} /* end if (langparse_ctor) */
	return rs ;
}
/* end subroutine (langparse_start) */

int langparse_finish(langparse *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = langparse_magic(op)) >= 0) {
	    if (shortq *obp = obufp(op->outbuf) ; obp) {
		{
		    rs1 = obp->finish ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            delete obp ;
	            op->outbuf = nullptr ;
		}
	    } /* end if (outbuf) */
	    {
		rs1 = langparse_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (langparse_finish) */

int langparse_load(langparse *op,cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = langparse_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        while (sl-- && *sp) {
		    cint ch = mkchar(*sp) ;
		    rs = langparse_proc(op,ch) ;
		    c += 1 ;
		    if (rs < 0) break ;
	        } /* end while */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langparse_load) */

int langparse_read(langparse *op,short *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = langparse_magic(op,rbuf)) >= 0) {
	    rs = SR_INVALID ;
	    rbuf[0] = 0 ;
	    if (rlen >= 0) {
	        int	ml ;
	        if (shortq *obp ; (obp = obufp(op->outbuf)) != np) {
	            cint	len = obp->len ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; i < ml ; i += 1) {
			if (short code ; (rs = obp->rem(&code)) >= 0) {
	                    rbuf[i] = code ;
			} else if (rs == SR_NOTFOUND) {
			    rs = SR_OK ;
			    break ;
			}
	            } /* end for */
	            rbuf[i] = 0 ;
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (langparse_read) */


/* private subroutines */

local int langparse_proc(langparse *op,int ch) noex {
	int		rs ;
	int		f = false ; /* return-value */
	if ((rs = langparse_magic(op)) >= 0) {
	    f = op->fl.clear ;
	    if (op->fl.comment) {
	        if ((op->pch == CH_STAR) && (ch == CH_SLASH)) {
		    op->fl.comment = false ;
		    op->fl.clear = true ;
	        }
	    } else if (op->fl.quote) {
	        if (op->fl.skip) {
		    op->fl.skip = false ;
	        } else {
		    if (ch == CH_BSLASH) {
		        op->fl.skip = true ;
	            } else if (ch == CH_DQUOTE) {
		        op->fl.quote = false ;
		        op->fl.clear = true ;
		    }
	        }
	    } else if (op->fl.literal) {
	        if (op->fl.skip) {
		    op->fl.skip = false ;
	        } else {
		    if (ch == CH_BSLASH) {
		        op->fl.skip = true ;
	            } else if (ch == CH_SQUOTE) {
		        op->fl.literal = false ;
		        op->fl.clear = true ;
		    }
	        }
	    } else {
	        switch (ch) {
	        case CH_STAR:
		    if (op->pch == CH_SLASH) {
		        op->fl.comment = true ;
		        op->fl.clear = false ;
		        f = false ;
		    }
		    break ;
	        case CH_DQUOTE:
		    if ((op->pch != CH_BSLASH) && (op->pch != CH_SQUOTE)) {
		        op->fl.quote = true ;
		        op->fl.clear = false ;
		        f = false ;
		    }
		    break ;
	        case CH_SQUOTE:
		    if (op->pch != CH_BSLASH) {
		        op->fl.literal = true ;
		        op->fl.clear = false ;
		        f = false ;
		    }
		    break ;
	        } /* end switch */
	    } /* end if */
	    if (f) {
		if (shortq *obp = sbufp(op->outbuf) ; obp) {
		    cshort code = shortconv(ch) ;
		    rs = obp->ins(code) ;
		}
	    }
	    op->pch = ch ;
	} /* end if (langparse_magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (langparse_proc) */

local int langparse_add(langparse *op,cchar *vp,int vl = -1) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && vl-- && *vp) {
	    rs = langparse_add(op,*vp++) ;
	    c += rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subrolutine (langparse_add) */

local int langparse_add(langparse *op,int v) noex {
    	int		rs = SR_BUGCHECK ;
	if (shortq *obp = obufp(op->outbuf) ; obp) {
	    cshort code = shortconv(v) ;
	    rs = obp->ins(code) ;
	}
	return (rs >= 0) ? 1 : rs ;
}
/* end subrolutine (langparse_add) */


