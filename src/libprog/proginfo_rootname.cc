/* proginfo_rootname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information (extracting the distribution root-name) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Here I simply extract the root-name of the software
	distribution from our program-root directory.  This is
	really just grabbing the "base-name" of a directory path.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sfx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import proginfo ;

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

int proginfo_rootname(PI *pip) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pip) ylikely {
	    rs = SR_BADFMT ;
	    if (pip->pr) {
	        if (pip->rootname == nullptr) {
	            cchar	*cp ;
	            if (int cl ; (cl = sfbasename(pip->pr,-1,&cp)) > 0) {
		        ccharpp	vpp = &pip->rootname ;
		        rs = pip->setent(vpp,cp,cl) ;
		        len = rs ;
	            } /* end */
	        } else {
	            len = lenstr(pip->rootname) ;
	        } /* end */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (proginfo_rootname) */


