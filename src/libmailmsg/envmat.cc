/* evmat SUPPORT (Environment-Match) */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This is part of our cleanup-compatibility effort.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/***************************************************************************

  	Name:
	envmat

	Description:
	Check for (try to "match") the introducer c-string of a
	mail-message "environment" c-string.

	Synopsis:
	int envmat(cchar *sp,int sl) noex

	Arguments:
	sp		c-string to search for match pointer
	sl		c-string to search for match length

	Returns:
	>0		the string contained an envelope
	==0		the string did not contain an evelope
	<0		error (system-return)

****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<matstr.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"envmat.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int envmat(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		f = false ;
	if (sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    rs = SR_OK ;
	    if ((sl > 5) && (sp[0] == 'F') && (sp[1] == 'r')) {
		f = (strncmp((sp+2),"om ",3) == 0) ;
	    } /* end if (possible match) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (envmat) */


