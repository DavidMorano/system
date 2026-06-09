/* getourenv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get (retrieve) the value of an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code is generalized from one of my old programs.  Why
	the need for this?  Because with the introduction of loadable
	commands in Korn Shell (KSH), we also needed a local source
	for an environment since the global one is indeterminate
	(missing or incorrect).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getourenv

	Description:
	This subroutine is similar to the standard library |getenv(3c)|
	subroutine but uses a caller-supplied environment array
	instead.

	Synopsis:
	cchar *getourenv(cchar *const *envv,cchar *key) noex

	Arguments:
	envv		environment array
	key		key-string to lookup

	Returns:
	-		pointer to value for given key
	nullptr		key not found in environment database

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getourenv.h"


/* local defines */


/* imported namespaces */

using libu::matkeystr ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *getourenv(con mainv envv,cchar *key) noex {
	cchar		*vp = nullptr ; /* return-value */
	if (envv && key && key[0]) {
	    if (int ei ; (ei = matkeystr(envv,key,-1)) >= 0) {
		if ((vp = strchr(envv[ei],'=')) != nullptr) {
		    vp += 1 ;
		}
	    }
	} /* end if */
	return vp ;
} 
/* end subroutine (getourenv) */


