/* getpwx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the best approximation of the user's username */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	getpwx

	Description:
	Interface to PASSWORD caching.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<pwd.h>
#include	<usystem.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getax.h>

#include	"getpwx.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*getpwxname_f)(ucentpw *,char *,int,cchar *) noex ;
    typedef int (*getpwxuid_f)(ucentpw *,char *,int,uid_t) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr getpwxname_f	subnames[] = {
	ucpwcache_name,
	getpw_name
} ;

static constexpr getpwxuid_f	subuids[] = {
	ucpwcache_uid,
	getpw_uid
} ;

constexpr uid_t			uidend(-1) ;


/* exported variables */

int		getpwx_state = 1 ;


/* exported subroutines */

int getpwx_control(int ns) noex {
    	int		rs = SR_INVALID ;
	if ((ns >= 0) && (ns < 2)) {
	    getpwx_state = ns ;
	    rs = SR_OK ;
	}
	return rs ;
}

int getpwx_name(ucentpw *pwp,char *pwbuf,int pwlen,cchar *name) noex {
	int		rs = SR_FAULT ;
	if (pwp && pwbuf && name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		cint	w = getpwx_state ;
		rs = subnames[w](pwp,pwbuf,pwlen,name) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwx_name) */

int getpwx_uid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	if (pwp && pwbuf) {
	    rs = SR_INVALID ;
	    if (uid != uidend) {
		cint	w = getpwx_state ;
		rs = subuids[w](pwp,pwbuf,pwlen,uid) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwx_uid) */


