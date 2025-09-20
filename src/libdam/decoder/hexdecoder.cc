/* hexdecoder SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* HEX decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hexdecoder

	Names:
	hexdecoder_start
	hexdecoder_load
	hexdecoder_read
	hexdecoder_finish

	Description:
	This object facilitates HEX (coded input) decoding.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<digval.h>
#include	<obuf.hh>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"hexdecoder.h"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

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
local int hexdecoder_ctor(hexdecoder *op,Args ... args) noex {
	HEXDECODER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hexdecoder_ctor) */

local int hexdecoder_dtor(hexdecoder *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hexdecoder_dtor) */

template<typename ... Args>
local inline int hexdecoder_magic(hexdecoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HEXDECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (hexdecoder_magic) */

local int	hexdecoder_cvt(hexdecoder *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int hexdecoder_start(hexdecoder *op) noex {
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = hexdecoder_ctor(op)) >= 0) {
	    if (obuf *obp ; (obp = new(nt) obuf) != np) {
		if ((rs = obp->start) >= 0) {
	            op->outbuf = obp ;
	            op->magic = HEXDECODER_MAGIC ;
		}
		if (rs < 0) {
		    delete obp ;
		}
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if (new-obuf) */
	    if (rs < 0) {
		hexdecoder_dtor(op) ;
	    }
	} /* end if (hexdecoder_ctor) */
	return rs ;
}
/* end subroutine (hexdecoder_start) */

int hexdecoder_finish(hexdecoder *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hexdecoder_magic(op)) >= 0) {
	    if (op->outbuf) {
	        obuf	*obp = obufp(op->outbuf) ;
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
		rs1 = hexdecoder_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hexdecoder_finish) */

int hexdecoder_load(hexdecoder *op,cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = hexdecoder_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if (obuf *obp ; (obp = obufp(op->outbuf)) != nullptr) {
	            while ((rs >= 0) && (sl > 0) && *sp) {
		        cint	ch = mkchar(*sp) ;
		        if (ishexlatin(ch)) {
		            if (op->rl == 0) {
			        op->rb[0] = charconv(ch) ;
			        op->rl = 1 ;
		            } else {
			        op->rb[1] = charconv(ch) ;
			        rs = hexdecoder_cvt(op) ;
			        c += rs ;
			        op->rl = 0 ;
		            }
		        } /* end if (ishexlatin) */
		        sp += 1 ;
		        sl -= 1 ;
	            } /* end while */
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hexdecoder_load) */

int hexdecoder_read(hexdecoder *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = hexdecoder_magic(op,rbuf)) >= 0) {
	    int		ml ;
	    rbuf[0] = '\0' ;
	    if (obuf *obp ; (obp = obufp(op->outbuf)) != nullptr) {
	        cint	len = obp->len ; /* <- read-coerce */
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
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (hexdecoder_read) */


/* private subroutines */

local int hexdecoder_cvt(hexdecoder *op) noex {
	int		rs = SR_OK ;
	cchar		*rb = op->rb ;
	if (obuf *obp ; (obp = obufp(op->outbuf)) != nullptr) {
	    cint	ch0 = mkchar(rb[0]) ;
	    cint	ch1 = mkchar(rb[1]) ;
	    int	v = 0 ;
	    v |= (digvalhex(ch0)<<4) ;
	    v |= (digvalhex(ch1)<<0) ;
	    rs = obp->add(v) ;
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (hexdecoder_cvt) */


