/* proginfo_rootname SUPPORT */
/* lang=C++20 */

/* program information (extracting the distribution root-name) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Here we simply extract the root-name of the software
	distribution from our program-root directory.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"proginfo.h"


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

int proginfo_rootname(PROGINFO *pip) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pip) {
	    rs = SR_BADFMT ;
	    if (pip->pr) {
	        if (pip->rootname == nullptr) {
	            cchar	*cp ;
	            if (int cl ; (cl = sfbasename(pip->pr,-1,&cp)) > 0) {
		        cchar	**vpp = &pip->rootname ;
		        rs = proginfo_setentry(pip,vpp,cp,cl) ;
		        len = rs ;
	            }
	        } else {
	            len = strlen(pip->rootname) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (proginfo_rootname) */


