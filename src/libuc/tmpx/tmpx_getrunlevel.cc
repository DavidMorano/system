/* tmpx_getrunlevel SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmpx_getrunlevel

	Description:
	This subroutine will try to find a "run-level" record in
	the UTMPX database.

	Synopsis:
	int tmpx_getrunlevel(tmpx *op) noex

	Arguments:
	op		pointer to TMPX object

	Returns:
	>=0	run-level (including '0' meaning no record found)
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<vecstr.h>
#include	<tmpx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int tmpx_getrunlevel(tmpx *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		runlevel = 0 ;
	if (op) {
	    if_constexpr (f_darwin) {
		runlevel = 3 ;
	    } else {
	        if (tmpx_cur cur ; (rs = tmpx_curbegin(op,&cur)) >= 0) {
	            tmpx_ent	ue{} ;
	            while (rs >= 0) {
		        rs1 = tmpx_curenum(op,&cur,&ue) ;
		        if (rs1 == SR_NOTFOUND) break ;
		        rs = rs1 ;
		        if (rs < 0) break ;
	                if (ue.ut_type == TMPX_TRUNLEVEL) {
#if	F_DARWIN
		            runlevel = 3 ;
#else
		            runlevel = ue.ut_exit.e_termination & UCHAR_MAX ;
#endif /* F_DARWIN */
		            break ;
		        } /* end if (got run-level) */
	            } /* end while (looping through entries) */
	            rs1 = tmpx_curend(op,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (tmpx-cursor) */
	    } /* end if_constexpr (f_darwin) */
	} /* end if (non-null) */
	return (rs >= 0) ? runlevel : rs ;
}
/* end subroutine (tmpx_getrunlevel) */


