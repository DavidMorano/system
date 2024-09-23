/* hexdecoder SUPPORT */
/* lang=C++98 */

/* HEX decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	hexdecoder_start
	hexdecoder_load
	hexdecoder_read
	hexdecoder_finish

	Description:
	We facilitate HEX (coded input) decoding.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<digval.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"hexdecoder.h"
#include	"obuf.hh"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int hexdecoder_magic(hexdecoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HEXDECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (hexdecoder_magic) */

static int	hexdecoder_cvt(hexdecoder *) noex ;


/* local variables */


/* exported subroutines */

int hexdecoder_start(hexdecoder *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	    if (obuf *obp ; (obp = new(nothrow) obuf) != nullptr) {
	        op->outbuf = (void *) obp ;
	        op->magic = HEXDECODER_MAGIC ;
	    } else {
	        rs = SR_NOMEM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hexdecoder_start) */

int hexdecoder_finish(hexdecoder *op) noex {
	int		rs ;
	if ((rs = hexdecoder_magic(op)) >= 0) {
	    if (op->outbuf) {
	        obuf *obp = (obuf *) op->outbuf ;
	        delete obp ;
	        op->outbuf = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hexdecoder_finish) */

int hexdecoder_load(hexdecoder *op,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = hexdecoder_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (obuf *obp ; (obp = ((obuf *) op->outbuf)) != nullptr) {
	        while ((rs >= 0) && (sl > 0) && *sp) {
		    cint	ch = mkchar(*sp) ;
		    if (ishexlatin(ch)) {
		        if (op->rl == 0) {
			    op->rb[0] = ch ;
			    op->rl = 1 ;
		        } else {
			    op->rb[1] = ch ;
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
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hexdecoder_load) */

int hexdecoder_read(hexdecoder *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = hexdecoder_magic(op,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if (obuf *obp ; (obp = ((obuf *) op->outbuf)) != nullptr) {
	        cint	len = obp->len() ;
	        int	ml ;
	        ml = min(len,rlen) ;
	        for (i = 0 ; i < ml ; i += 1) {
		    rbuf[i] = obp->at(i) ;
	        }
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

static int hexdecoder_cvt(hexdecoder *op) noex {
	int		rs = SR_OK ;
	cchar		*rb = op->rb ;
	if (op->outbuf) {
	    obuf *obp = (obuf *) op->outbuf ;
	    int	ch0 = mkchar(rb[0]) ;
	    int	ch1 = mkchar(rb[1]) ;
	    int	v = 0 ;
	    v |= (digvalhex(ch0)<<4) ;
	    v |= (digvalhex(ch1)<<0) ;
	    obp->add(v) ;
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (hexdecoder_cvt) */


