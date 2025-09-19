/* chariq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Character-Interlocked Queue management */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	chariq

	Description:
	This object manages interlocked FIFO-character operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"chariq.h"


/* local defines */

#define	DEFLEN		10


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int chariq_start(chariq *op,int sz) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (sz <= 0) sz = DEFLEN ;
	    if ((rs = ptm_create(&op->m,nullptr)) >= 0) {
	        rs = charq_start(&op->q,sz) ;
	        if (rs < 0) {
		    ptm_destroy(&op->m) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (chariq_start) */

int chariq_finish(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = charq_finish(&op->q) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (chariq_finish) */

int chariq_ins(chariq *op,int ch) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            rs = charq_ins(&op->q,ch) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_ins) */

int chariq_rem(chariq *op,char *chp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            rs = charq_rem(&op->q,chp) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_rem) */

int chariq_size(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            rs = charq_size(&op->q) ;
		    rv = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (chariq_size) */

int chariq_count(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            rs = charq_count(&op->q) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_count) */


