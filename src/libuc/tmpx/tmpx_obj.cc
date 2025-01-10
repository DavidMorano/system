/* tmpx_obj SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage reading or writing of the [UW]TMP database */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed similar sorts of functions.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	tmpx

	Description:
	This code is used to access the Utmpx (or WTMPX) database.
	Those databases constitute the connect-time accounting
	information for the system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"tmpx.h"


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

int tmpx::read(int ei,tmpx_ent *ep) noex {
	return tmpx_read(this,ei,ep) ;
}

int tmpx::write(int ei,tmpx_ent *ep) noex {
	return tmpx_write(this,ei,ep) ;
}

int tmpx::check(time_t dt) noex {
	return tmpx_check(this,dt) ;
}

int tmpx::curbegin(tmpx_cur *curp) noex {
	return tmpx_curbegin(this,curp) ;
}

int tmpx::curend(tmpx_cur *curp) noex {
	return tmpx_curend(this,curp) ;
}

int tmpx::curenum(tmpx_cur *curp,tmpx_ent *ep) noex {
	return tmpx_curenum(this,curp,ep) ;
}

int tmpx::fetchpid(tmpx_ent *ep,pid_t pid) noex {
	return tmpx_fetchpid(this,ep,pid) ;
}

int tmpx::fetchuser(tmpx_cur *curp,tmpx_ent *ep,cchar *un) noex {
	return tmpx_fetchuser(this,curp,ep,un) ;
}

void tmpx::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("tmpx",rs,"fini-close") ;
	}
}

int tmpx_op::operator () (cchar *fn,int of) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case tmpxmem_open:
		rs = tmpx_open(op,fn,of) ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (tmpx_op::operator) */

tmpx_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case tmpxmem_getrunlevel:
	        rs = tmpx_getrunlevel(op) ;
	        break ;
	    case tmpxmem_nusers:
	        rs = tmpx_nusers(op) ;
	        break ;
	    case tmpxmem_close:
	        rs = tmpx_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (tmpx_co::operator) */


