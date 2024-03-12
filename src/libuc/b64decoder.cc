/* b64decoder SUPPORT */
/* lang=C++98 */

/* Base-64 (B64) decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We facilitate Base-64 (B64) (coded input) decoding.

		b64decoder_start
		b64decoder_load
		b64decoder_read
		b64decoder_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstring>
#include	<vector>
#include	<new>
#include	<algorithm>
#include	<usystem.h>
#include	<sfx.h>
#include	<intfloor.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"b64decoder.h"
#include	"base64.h"
#include	"obuf.hh"


/* local defines */


/* namespaces */

using std::min ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int b64decoder_ctor(b64decoder *op,Args ... args) noex {
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

static inline int b64decoder_dtor(b64decoder *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}

static int	b64decoder_cvt(B64DECODER *,cchar *,int) noex ;


/* local variables */


/* exported subroutines */

int b64decoder_start(B64DECODER *op) noex {
	int		rs ;
	if ((rs = b64decoder_ctor(op)) >= 0) {
	    obuf	*obp ;
	    rs = SR_NOMEM ;
	    if ((obp = new(nothrow) obuf) != nullptr) {
	        op->outbuf = (void *) obp ;
	        op->magic = B64DECODER_MAGIC ;
	        rs = SR_OK ;
	    } /* end if (new) */
	    if (rs < 0) {
		b64decoder_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (b64decoder_start) */

int b64decoder_finish(B64DECODER *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == B64DECODER_MAGIC) {
	        if (op->outbuf) {
	            obuf 	*obp = (obuf *) op->outbuf ;
	            delete obp ;
	            op->outbuf = nullptr ;
	        }
	        {
		    rs1 = b64decoder_dtor(op) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        op->magic = 0 ;
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (b64decoder_finish) */

int b64decoder_load(B64DECODER *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && sp) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == B64DECODER_MAGIC) {
	        obuf		*obp ;
		rs = SR_OK ;
	        if (sl < 0) sl = strlen(sp) ;
	        if ((obp = ((obuf *) op->outbuf)) != nullptr) {
	            int		cl ;
	            cchar	*cp ;
	            while ((cl = nextfield(sp,sl,&cp)) > 0) {
	                sl -= ((cp+cl)-sp) ;
	                sp = (cp+cl) ;
	                if (op->rl > 0) {
	                    cint	rl = op->rl ;
	                    int		ml = min(cl,(4-op->rl)) ;
	                    char	*rb = op->rb ;
	                    strwcpy((rb+rl),cp,ml) ;
	                    op->rl += ml ;
	                    cp += ml ;
	                    cl -= ml ;
	                    if (op->rl == 4) {
	                        rs = b64decoder_cvt(op,op->rb,4) ;
	                        c += rs ;
	                        op->rl = 0 ;
	                    }
	                } /* end if (positive residue) */
	                while ((rs >= 0) && (cl >= 4)) {
		            cint	ml = ifloor(cl,4) ;
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
	            } /* end while (nextfield) */
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (b64decoder_load) */

int b64decoder_read(B64DECODER *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && rbuf) {
	    rs = SR_NOTOPEN ;
	    rbuf[0] = '\0' ;
	    if (op->magic == B64DECODER_MAGIC) {
		rs = SR_INVALID ;
	        if (rlen >= 0) {
	            obuf	*obp ;
		    rs = SR_OK ;
	            if (rlen > 0) {
	                if ((obp = ((obuf *) op->outbuf)) != nullptr) {
	                    cint	len = obp->len() ;
	                    int		ml ;
	                    ml = min(len,rlen) ;
	                    for (i = 0 ; i < ml ; i += 1) {
		                rbuf[i] = obp->at(i) ;
	                    }
	                    rbuf[i] = '\0' ;
	                    rs = obp->adv(i) ;
	                } else {
	                    rs = SR_BUGCHECK ;
	                }
	            } /* end if (positive) */
		} /* end if (valid) */
	    } /* end if (real money) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (b64decoder_read) */


/* private subroutines */

static int b64decoder_cvt(B64DECODER *op,cchar *cp,int cl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->outbuf) {
	    obuf 	*obp = (obuf *) op->outbuf ;
	    char	*rbuf ;
	    if ((rbuf = new(nothrow) char [cl+1]) != nullptr) {
	        if ((c = base64_d(cp,cl,rbuf)) > 0) {
	            rbuf[c] = '\0' ;
	            obp->add(rbuf,c) ;
	        } else {
	            rs = SR_ILSEQ ;
		}
	        delete [] rbuf ;
	    } else { /* memory allocation */
	        rs = SR_NOMEM ;
	    }
	} else {
	    rs = SR_BUGCHECK ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (b64decoder_cvt) */


