/* tmpx_getboottime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmpx_getboottime

	Description:
	This subroutine tries to fine the "boot-time" record in the
	UTMPX database.

	Synopsis:
	int tmpx_getboottime(tmpx *op,time_t *rp) noex

	Arguments:
	op		pointer to TMPX object
	rp		pointer to resulting time (time_t)

	Returns:
	>=0	database index of the "boot-time" record
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
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


/* exported variables */


/* exported subroutines */

int tmpx_getboottime(tmpx *op,time_t *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (op && rp) {
	    *rp = 0 ;
	    if (tmpx_cur uc ; (rs = tmpx_curbegin(op,&uc)) >= 0) {
	        tmpx_ent	ue ;
	        while (rs >= 0) {
	            rs1 = tmpx_curenum(op,&uc,&ue) ;
	            if (rs1 == SR_NOTFOUND) break ;
	            rs = rs1 ;
	            if ((rs >= 0) && (ue.ut_type == TMPX_TBOOTTIME)) {
	                *rp = ue.ut_tv.tv_sec ;
	                break ;
	            }
	            n += 1 ;
	        } /* end while */
	        rs1 = tmpx_curend(op,&uc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_getboottime) */


