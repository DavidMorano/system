/* b64decoder SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Base-64 (B64) decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	b64decoder

	Methods:
	b64decoder_start
	b64decoder_load
	b64decoder_read
	b64decoder_finish

	Description:
	This object facilitates Base-64 (B64) (coded input) decoding.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sfx.h>
#include	<intfloor.h>
#include	<strwcpy.h>
#include	<sfx.h>
#include	<base64.h>
#include	<obuf.hh>
#include	<localmisc.h>

#include	"b64decoder.h"

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
local inline int b64decoder_ctor(b64decoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->rl = 0 ;
	    op->outbuf = nullptr ;
	    op->rb[0] = '\0' ;
	}
	return rs ;
}

local inline int b64decoder_dtor(b64decoder *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (b64decoder_dtor) */

template<typename ... Args>
local inline int b64decoder_magic(b64decoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == B64DECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (b64decoder_magic) */

local int	b64decoder_cvt(b64decoder *,cchar *,int) noex ;


/* local variables */

cint		nstage = B64DECODER_NSTAGE ;


/* exported variables */


/* exported subroutines */

int b64decoder_start(b64decoder *op) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = b64decoder_ctor(op)) >= 0) {
	    if (obuf *obp ; (obp = new(nothrow) obuf) != np) {
		if ((rs = obp->start) >= 0) {
	            op->outbuf = obp ;
	            op->magic = B64DECODER_MAGIC ;
		}
		if (rs < 0) {
	            delete obp ;
		}
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if (new-obuf) */
	    if (rs < 0) {
		b64decoder_dtor(op) ;
	    }
	} /* end if (b64decoder_ctor) */
	return rs ;
}
/* end subroutine (b64decoder_start) */

int b64decoder_finish(b64decoder *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = b64decoder_magic(op)) >= 0) {
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
		rs1 = b64decoder_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (b64decoder_finish) */

int b64decoder_load(b64decoder *op,cchar *sp,int µsl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ;
	if ((rs = b64decoder_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
                if (obuf *obp ; (obp = obufp(op->outbuf)) != np) {
                    cchar       *cp ;
                    for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
                        sl -= intconv((cp + cl) - sp) ;
                        sp = (cp + cl) ;
                        if (op->rl > 0) {
                            cint        rl = op->rl ;
                            int         ml = min(cl,(nstage - op->rl)) ;
                            char        *rb = op->rb ;
                            strwcpy((rb+rl),cp,ml) ;
                            op->rl += ml ;
                            cp += ml ;
                            cl -= ml ;
                            if (op->rl == nstage) {
                                rs = b64decoder_cvt(op,op->rb,nstage) ;
                                c += rs ;
                                op->rl = 0 ;
                            }
                        } /* end if (positive residue) */
                        while ((rs >= 0) && (cl >= nstage)) {
                            cint        ml = ifloor(cl,nstage) ;
                            rs = b64decoder_cvt(op,cp,ml) ;
                            c += rs ;
                            cp += ml ;
                            cl -= ml ;
                            if (rs < 0) break ;
                        } /* end while */
                        if ((rs >= 0) && (cl > 0)) {
                            strwcpy(op->rb,cp,cl) ;
                            op->rl = cl ;
                        } /* end if (remaining source) */
                        if (rs < 0) break ;
                    } /* end for (sfnext) */
                } else {
                    rs = SR_BUGCHECK ;
                }
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (b64decoder_load) */

int b64decoder_read(b64decoder *op,char *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = b64decoder_magic(op,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
            rs = SR_INVALID ;
            if (rlen >= 0) {
                rs = SR_OK ;
                if (rlen > 0) {
		    int		ml ;
                    if (obuf *obp ; (obp = obufp(op->outbuf)) != np) {
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
                    } /* end if */
                } /* end if (positive) */
            } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (b64decoder_read) */


/* private subroutines */

local int b64decoder_cvt(b64decoder *op,cchar *cp,int cl) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->outbuf) {
	    obuf 	*obp = obufp(op->outbuf) ;
	    rs = SR_NOMEM ;
	    if (char *rbuf ; (rbuf = new(nothrow) char [cl + 1]) != np) {
	        if ((c = base64_d(cp,cl,rbuf)) > 0) {
	            rbuf[c] = '\0' ;
	            rs = obp->add(rbuf,c) ;
	        } else {
	            rs = SR_ILSEQ ;
		}
	        delete [] rbuf ;
	    } /* end if (new-char) */
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (b64decoder_cvt) */


