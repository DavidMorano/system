/* proginfo3 MODULE (Module-Implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

	= 2026-10-20, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	proginfo

	Description:
	This module provides a very basic set of simple subroutines
	for basic program management.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strw.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<prognamevar.hh>	/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.base ;

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

statcexpr bool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int proginfo_progdname(PI *pip) noex {
	int		rs ;
	if (pip->progdname.valp == nullptr) {
	    if ((rs = proginfo_progename(pip)) >= 0) {
	        if (ccp enp = pip->progename.valp) {
	            ccharp	dp ; /* used-afterwards */
	            if (int dl ; (dl = sfdirname(enp,-1,&dp)) > 0) {
	                ccharpp vpp = &pip->progdname.valp ;
	                rs = pip->setent(vpp,dp,dl) ;
	            } /* end */
	        } /* end if (non-null) */
	    } /* end if (proginfo_progename) */
	} else {
	    rs = lenstr(pip->progdname.valp) ;
	} /* end */
	return rs ;
} /* end subroutine (proginfo_progdname) */


/* local subroutines */


