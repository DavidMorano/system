/* getenver SUPPORT */
/* encoding=ISO8859-1 */
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
	getenver

	Description:
	This subroutine is like |getenv(3c)| except that it also
	takes an additional argument that specifies the length of
	the environment variable string to look up in the environment
	array.

	Synposis:
	cchar		*getenver(cchar *np,int nl) noex

	Arguments:
	np		pointer to environment name (key) to lookup
	nl		length of environment name (key) to lookup

	Returns:
	-		pointer to value associated with given name
	nullptr		name not found

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<matkeystr.h>

#include	"getenver.h"


/* local defines */


/* external subroutines */


/* external variables */

extern mainv	environ ;


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *getenver(cchar *kp,int kl) noex {
	cchar		*vp = nullptr ;
	if (kp) {
	    if (kl < 0) kl = strlen(kp) ;
	    if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
		kl = (tp-kp) ;
	    }
	    if (int ei ; (ei = matkeystr(environ,kp,kl)) >= 0) {
	        if ((vp = strchr(environ[ei],'=')) != nullptr) {
	            vp += 1 ;
		}
	    }
	} /* end if */
	return vp ;
}
/* end subroutine (getenver) */


