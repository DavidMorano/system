/* ass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* experimenal string manipulation object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ass.h"


/* local defines */

#define	ASS_ADDLEN	100


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ass_start(ass *asp) noex {
	int		rs = SR_FAULT ;
	if (asp) {
	    asp->rbuf = nullptr ;
	    asp->rlen = 0 ;
	    asp->ext = 0 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}

int ass_addchr(ass *asp,int ch) noex {
	int		rs = SR_FAULT ;
	int		sz ;
	int		len = 0 ;
	if (asp) {
	    caddr_t	p ; /* used-multiple */
	    if (asp->rbuf) {
	        asp->rlen = 0 ;
	        asp->ext = ASS_ADDLEN ;
	        sz = asp->ext ;
	        rs = uc_malloc(sz,&p) ;
	    } else if (asp->ext == asp->rlen) {
	        asp->ext += ASS_ADDLEN ;
	        sz = asp->ext ;
	        rs = uc_realloc(asp->rbuf,sz,&p) ;
	    }
	    if (rs >= 0) {
	        asp->rbuf = p ;
	        asp->rbuf[(asp->rlen)++] = char(ch) ;
	    }
	    len = asp->rlen ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ass_addchr) */

int ass_len(ass *asp) noex {
    	int		rs = SR_FAULT ;
	if (asp) {
	    rs = asp->rlen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ass_len) */

int ass_finish(ass *asp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->rbuf) {
	        rs1 = uc_free(asp->rbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        asp->rbuf = nullptr ;
	    }
	    asp->rlen = 0 ;
	    asp->ext = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ass_finish) */

int ass::addchr(int ch) noex {
	return ass_addchr(this,ch) ;
}

void ass::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("ass",rs,"fini-finish") ;
	}
} /* end method (ass::dtor) */

ass_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case assmem_start:
	        rs = ass_start(op) ;
	        break ;
	    case assmem_len:
	        rs = ass_len(op) ;
	        break ;
	    case assmem_finish:
	        rs = ass_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ass_co::operator) */


