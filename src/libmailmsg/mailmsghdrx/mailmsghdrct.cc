/* mailmsghdrct SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage content-types with header field */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsghdrct

	Description:
	This object module parses a "content-type" header specification.
	The parsed results are separated into three types of items:
	the 'type', the 'sub-type', and 'parameters'.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"mailmsghdrct.h"


/* local defines */

#define	MMHCT		mailmsghdrct
#define	MMHCT_PAR	mailmsghdrct_par


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int mailmsghdrct_ctor(mailmsghdrct *op,Args ... args) noex {
    	MAILMSGHDRCT	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrct_ctor) */

static int mailmsghdrct_dtor(mailmsghdrct *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrct_dtor) */

template<typename ... Args>
static inline int mailmsghdrct_magic(mailmsghdrct *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGHDRCT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsghdrct_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsghdrct_start(MMHCT *op,cchar *hp,int hl) noex {
	int		rs ;
	int		nparams = 0 ;
	if ((rs = mailmsghdrct_ctor(op,hp)) >= 0) {
	    (void) hl ;
	    rs = SR_NOSYS ;
	    if (rs < 0) {
		mailmsghdrct_dtor(op) ;
	    }
	} /* end if (mailmsghdrct_ctor) */
	return (rs >= 0) ? nparams : rs ;
}
/* end subroutine (mailmsghdrct_start) */

int mailmsghdrct_finish(MMHCT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsghdrct_magic(op)) >= 0) {
	    op->mtp = nullptr ;
	    op->stp = nullptr ;
	    op->mtl = 0 ;
	    op->stl = 0 ;
	    {
		rs1 = mailmsghdrct_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsghdrct_finish) */

int mailmsghdrct_paramget(MMHCT *op,int i,MMHCT_PAR *rp) noex {
	int		rs ;
	if ((rs = mailmsghdrct_magic(op,rp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((i >= 0) && (i < MAILMSGHDRCT_NPARAMS)) {
		rs = op->p[i].kl ;
		*rp = op->p[i] ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsghdrct_paramget) */

int mailmsghdrct_paramfind(MMHCT *op,cchar *key,MMHCT *rp) noex {
	int		rs ;
	if ((rs = mailmsghdrct_magic(op,key,rp)) >= 0) {
	    rs = SR_NOSYS ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsghdrct_paramfind) */


