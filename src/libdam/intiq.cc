/* intiq SUPPORT */
/* lang=C++20 */

/* Integer-Interlocked Queue management */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-11-24, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	intiq

	Description:
	This object manages interlocked FIFO-integer operations.

	Notes:
	+ thread-safe

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<usystem.h>
#include	<localmisc.h>

#include	"intiq.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int intiq_start(intiq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = ptm_create(&op->m,nullptr)) >= 0) {
	        rs = fifoitem_start(&op->q) ;
	        if (rs < 0) {
		    ptm_destroy(&op->m) ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (intiq_start) */

int intiq_finish(intiq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = fifoitem_finish(&op->q) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (intiq_finish) */

int intiq_ins(intiq *op,int ch) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            cint	esize = sizeof(int) ;
	            rs = fifoitem_ins(&op->q,&ch,esize) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_ins) */

int intiq_rem(intiq *op,int *chp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
		    cint	esize = sizeof(int) ;
	            rs = fifoitem_rem(&op->q,chp,esize) ;
	            c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_rem) */

int intiq_count(intiq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        {
	            rs = fifoitem_count(&op->q) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_count) */


