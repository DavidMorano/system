/* tmpx_getuserlines SUPPORT */
/* lang=C++20 */

/* get all terminal lines where the given user is logged in on */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was originally written. It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmpx_getuserlines

	Description:
	This subroutine will find and return the names of all of
	the controlling terminal-lines for the specified username,
	if there are any.

	Synopsis:
	int tmpx_getuserlines(tmpx *op,vecstr *lp,cchar *username) noex

	Arguments:
	op		pointer to TMPX object
	lp		pointer to vecstr to receive terminal-line strings
	username	session ID to find controlling terminal for

	Returns:
	>=0		number of entries returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strncmp(3c)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<tmpx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int tmpx_getuserlines(tmpx *op,vecstr *lp,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (op && lp && un) {
	    rs = SR_INVALID ;
	    if (un[0]) {
	        tmpx_ent	ue ;
	        tmpx_cur	cur ;
	        if ((rs = tmpx_curbegin(op,&cur)) >= 0) {
	            cint	llen = TMPX_LLINE ;
	            bool	f ;
	            while (rs >= 0) {
		        rs1 = tmpx_fetchuser(op,&cur,&ue,un) ;
		        if (rs1 == SR_NOTFOUND) break ;
		        rs = rs1 ;
		        if (rs < 0) break ;
		        f = true ;
	                f = f && (ue.ut_type == TMPX_TPROCUSER) ;
	                f = f && (ue.ut_line[0] != '\0') ;
#ifdef	COMMENT
	                f = f && (strncmp(un,ue.ut_user,TMPX_LUSER) == 0) ;
#endif
		        if (f) {
		            n += 1 ;
	                    rs = vecstr_add(lp,ue.ut_line,llen) ;
		        }
	            } /* end while (looping through entries) */
	            rs1 = tmpx_curend(op,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_getuserlines) */


