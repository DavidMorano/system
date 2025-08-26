/* langparse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Quoted-Printable (QP) decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

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

	Orientation note:
	Data that is encoded in the Quoted-Printable format is
	mostly found in MIME-type mail messages.  The data can be
	either in the mail-message header value fields (header keys
	still need to be normal -- so far) or the data can be in a
	section of the mail-message body.  But sometimes data encoded
	with the Quoted-Printable mechanism are also found outside
	of any mail-message context.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<six.h>
#include	<strwcpy.h>
#include	<digval.h>		/* |digvalhex(3uc)| */
#include	<obuf.hh>
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"langparse.h"

import libutil ;

/* local defines */


/* namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef obuf *		obufp ;


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

local int	langparse_loadspace(langparse *,cchar *,int) noex ;
local int	langparse_add(langparse *,cchar *,int) noex ;
local int	langparse_add(langparse *,int) noex ;
local int	langparse_cvt(langparse *) noex ;


/* local variables */

cint		nstage = LANGPARSE_NSTAGE ;


/* exported variables */


/* exported subroutines */

int langparse_start(langparse *op,int f_space) noex {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs ;
	if ((rs = langparse_ctor(op)) >= 0) {
	    op->fl.space = bool(f_space) ;
	    if (obuf *obp ; (obp = new(nt) obuf) != np) {
		if ((rs = obp->start) >= 0) {
	            op->outbuf = obp ;
	            op->magic = LANGPARSE_MAGIC ;
		}
		if (rs < 0) {
		    delete obp ;
		}
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if (new-obuf) */
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
	    if (op->outbuf) {
	        obuf 	*obp = obufp(op->outbuf) ;
		{
		    rs1 = obp->finish ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            delete obp ;
	            op->outbuf = nullptr ;
		}
	    }
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
	        if (obuf *obp ; (obp = obufp(op->outbuf)) != nullptr) {
	            if (op->fl.space) {
	                rs = langparse_loadspace(op,sp,sl) ;
	                c += rs ;
	            } else {
	                cint	nl = nstage ;
	                while ((rs >= 0) && (sl > 0)) {
	                    if (op->fl.esc) {
	                        cint	rl = op->rl ;
	                        int	ml = min(sl,(nl - op->rl)) ;
	                        char	*rb = op->rb ;
	                        strwcpy((rb + rl),sp,ml) ;
	                        op->rl += ml ;
	                        sp += ml ;
	                        sl -= ml ;
	                        if (rl == nl) {
	                            rs = langparse_cvt(op) ;
	                            c += rs ;
	                            op->rl = 0 ;
	                            op->fl.esc = false ;
	                        }
	                    } else {
	                        int		si ;
	                        while ((si = sichr(sp,sl,'=')) >= 0) {
	                            op->fl.esc = true ;
	                            if (si > 0) {
	                                rs = langparse_add(op,sp,si) ;
	                                c += rs ;
	                                sp += si ;
	                                sl -= si ;
	                            }
	                            sp += 1 ;
	                            sl -= 1 ;
	                            if ((rs >= 0) && (sl > 0)) {
	                                cint	rl = op->rl ;
	                                int	ml = min(sl,(nl - op->rl)) ;
	                                char	*rb = op->rb ;
	                                strwcpy((rb + rl),sp,ml) ;
	                                op->rl += ml ;
	                                sp += ml ;
	                                sl -= ml ;
	                                if (op->rl == nl) {
	                                    rs = langparse_cvt(op) ;
	                                    c += rs ;
	                                    op->rl = 0 ;
	                                    op->fl.esc = false ;
	                                }
	                            } /* end if */
			            if (rs < 0) break ;
	                        } /* end while */
	                        if ((rs >= 0) && (sl > 0)) {
	                            rs = langparse_add(op,sp,sl) ;
	                            c += rs ;
	                            sl = 0 ;
	                        } /* end if (remaining source) */
	                    } /* end if (escape or not) */
	                } /* end while */
	            } /* end if (space or not) */
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langparse_load) */

int langparse_read(langparse *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = langparse_magic(op,rbuf)) >= 0) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) {
	        int	ml ;
	        if (obuf *obp ; (obp = obufp(op->outbuf)) != nullptr) {
	            cint	len = obp->len ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; i < ml ; i += 1) {
			cint	ch = obp->at(i) ;
	                rbuf[i] = charconv(ch) ;
	            } /* end for */
	            rbuf[i] = '\0' ;
	            rs = obp->adv(i) ;
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (langparse_read) */


/* private subroutines */

local int langparse_loadspace(langparse *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && (sl-- > 0)) {
	    cint	ch = mkchar(*sp) ;
	    if (! CHAR_ISWHITE(ch)) {
	        if (op->fl.esc) {
	            op->rb[op->rl++] = charconv(ch) ;
	            if (op->rl == 2) {
	                rs = langparse_cvt(op) ;
	                c += rs ;
	                op->rl = 0 ;
	                op->fl.esc = false ;
	            }
	        } else if (ch == '=') {
	            op->fl.esc = true ;
	        } else {
	            rs = langparse_add(op,ch) ;
	            c += rs ;
	        }
	    } /* end if (not white-space) */
	    sp += 1 ;
	} /* end while */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langparse_readspace) */

local int langparse_add(langparse *op,cchar *vp,int vl = -1) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && vl-- && *vp) {
	    rs = langparse_add(op,*vp++) ;
	    c += rs ;
	}
	return c ;
}
/* end subrolutine (langparse_add) */

local int langparse_add(langparse *op,int v) noex {
    	int		rs ;
	obuf 		*obp = obufp(op->outbuf) ;
	if (op->fl.space && (v == '_')) {
	    v = ' ' ;
	}
	rs = obp->add(v) ;
	return (rs >= 0) ? 1 : rs ;
}
/* end subrolutine (langparse_add) */

local int langparse_cvt(langparse *op) noex {
	int		rs = SR_OK ;
	char		*rb = op->rb ;
	if (op->outbuf != nullptr) {
	    obuf 	*obp = obufp(op->outbuf) ;
	    cint	ch0 = mkchar(rb[0]) ;
	    cint	ch1 = mkchar(rb[1]) ;
	    int		v = 0 ;
	    if (ishexlatin(ch0) && ishexlatin(ch1)) {
		v |= (digvalhex(ch0)<<4) ;
		v |= (digvalhex(ch1)<<0) ;
	    } else {
	        v = '¿' ;
	    } 
	    rs = obp->add(v) ;
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (langparse_cvt) */


