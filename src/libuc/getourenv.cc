/* getourenv */
/* lang=C++20 */

/* get (retrieve) a local environment variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code is generalized from one of my old programs. Why
	the need for this? Because with the introduction of loadable
	commands in Korn Shell (KSH), we also needed a local source
	for an environment.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine is similar to the standard library |getenv(3c)|
	subroutine but uses a local environment array instead.

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
#include	<sys/types.h>
#include	<cstring>		/* <- |strchr(3c)| */
#include	<usystem.h>
#include	<matkeystr.h>
#include	<localmisc.h>

#include	"getourenv.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* exported subroutines */

cchar *getourenv(mainv envv,cchar *key) noex {
	cchar		*vp = nullptr ;
	if (envv && (key[0] != '\0')) {
	    if (int i ; (i = matkeystr(envv,key,-1)) >= 0) {
		if ((vp = strchr(envv[i],'=')) != nullptr) {
		    vp += 1 ;
		}
	    }
	}
	return vp ;
} 
/* end subroutine (getourenv) */


