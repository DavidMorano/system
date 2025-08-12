/* ema_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ema.h"


/* local defines */


/* imported namespaces */

using namespace		ema_ns ;	/* |ema_starter()| */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int ema::parse(cchar *sp,int sl) noex {
	return ema_parse(this,sp,sl) ;
}

int ema::addent(ema_ent *ep) noex {
    	return ema_addent(this,ep) ;
}

int ema::addents(ema *oop) noex {
    	return ema_addents(this,oop) ;
}

int ema::get(int i,ema_ent **epp) noex {
	return ema_get(this,i,epp) ;
}

int ema::getbestaddr(int i,cchar **rpp) noex {
    	return ema_getbestaddr(this,i,rpp) ;
}

int ema::haveaddr(cchar *sp,int sl) noex {
    	return ema_haveaddr(this,sp,sl) ;
}

int ema::first(cchar **rpp) noex {
    	return ema_first(this,rpp) ;
}

void ema::dtor() noex {
    if (cint rs = finish ; rs < 0) {
	ulogerror("ema",rs,"dtor-finish") ;
    }
}

int ema_st::operator () (cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case emamem_start:
	        rs = ema_starter(op,sp,sl) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ema_st::operator) */

ema_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case emamem_count:
	        rs = ema_count(op) ;
	        break ;
	    case emamem_finish:
	        rs = ema_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ema_co::operator) */


