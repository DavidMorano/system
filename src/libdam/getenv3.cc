/* getenv3 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written from scratch.  There are (or
	may) be some other standard ones floating around like it
	but I could not find one that was exactly what I needed
	(sigh).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getenv3

	Description:
	This subroutine is like |getenv(3c)| except that it also
	takes an argument that specifies the length of the environment
	variable string to look up in the environment array.  This
	subroutine also returns a pointer to the entire string (key
	and value pair) as it is found (if found) in the process
	environment array.

	Synopsis:
	int getenv3(cchar *namp,int naml,cchar **epp) noex

	Arguments:
	namp		key-name pointer
	naml		key-name length
	epp		pointer to hold result, if found

	Returns:
	>=0		OK, variable found
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getenv3.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::rmochr ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */

namespace {
    struct vars {
	mainv	envv ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int getenv3(cchar *namp,int µnaml,cchar **epp) noex {
    	int		rs = SR_FAULT ;
	int		el = 0 ; /* return-value */
	if (int naml ; namp && ((naml = getlenstr(namp,µnaml)) >= 0)) {
	    rs = SR_NOTFOUND ;
	    if (cint keyl = rmochr(namp,naml,'=') ; keyl > 0) {
		if (static cint rsv = var ; (rs = rsv) >= 0) {
		    rs = SR_NOTFOUND ;
	            for (mainv p = var.envv ; *p ; p += 1) {
		        int	i ; /* used-afterwards */
	                cchar	*sp = namp ;
	                cchar	*cp = *p ;
	                for (i = keyl ; i && *cp ; i -= 1) {
	                    if (*cp++ != *sp++) break ;
	                } /* end for */
	                if ((i == 0) && (*cp++ == '=')) {
			    el = lenstr(cp) ;
	                    if (epp) *epp = cp ;
			    rs = SR_OK ;
		            break ;
	                } /* end if */
			if (rs >= 0) break ;
	            } /* end for */
		} /* end if (vars) */
	    } /* end if (getkeylen) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? el : rs ;
} /* end subroutine (getenv3) */


/* local subroutines */

vars::operator int () noex {
    	return u_getenviron(&envv) ;
} /* end method (vars::operator) */


