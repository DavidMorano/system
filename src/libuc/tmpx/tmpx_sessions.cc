/* tmpx_getsessions SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get all of the terminals where the given user is logged in */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This code was originally written.  It was prompted by the
	failure of other terminal message programs from finding the
	proper controlling terminal.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This subroutine will find and return the names of all of
	the controlling terminals for the specified username, if
	there are any.

	Synopsis:
	int tmpx_getsessions(tmpx *op,vecint *vip,cchar *username) noex

	Arguments:
	op		pointer to TMPX object
	vip		pointer to vecint to receive sessions
	username	username to find sessions

	Returns:
	>=0		number of entries returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<tmpx.h>
#include	<vecint.h>
#include	<localmisc.h>

#include	"tmpx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int tmpx_getsessions(tmpx *op,vecint *vip,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = tmpx_magic(op,vip,un)) >= 0) {
	    rs = SR_INVALID ;
	    if (un[0]) {
	        if (tmpx_cur cur{} ; (rs = tmpx_curbegin(op,&cur)) >= 0) {
	            tmpx_ent	ue{} ;
	            bool	f ;
	            while (rs >= 0) {
	                rs1 = tmpx_fetchuser(op,&cur,&ue,un) ;
	                if (rs1 == SR_NOTFOUND) break ;
	                rs = rs1 ;
	                if (rs < 0) break ;
	                f = true ;
	                f = f && (ue.ut_type == TMPX_TPROCUSER) ;
	                f = f && (ue.ut_line[0] != '\0') ;
	                if (f) {
	                    int	v = ue.ut_pid ;
	                    n += 1 ;
	                    rs = vecint_add(vip,v) ;
	                } /* end if (got one) */
	            } /* end while (looping through entries) */
	            rs1 = tmpx_curend(op,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_getsessions) */


