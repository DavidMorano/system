/* gettmpdname SUPPORT */
/* lang=C++20 */

/* get the name of the TMPDIR (however one can get it) */
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

	Name:
	gettmpdname

	Description:
	Retrieve the name of the current TMPDIR.

	Synopsis:
	cchar *gettmpdname(mainv) noex

	Arguments:
	envv		environment array

	Returns:
	-		pointer to value
	nullptr		key not found in environment database

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>		/* <- |getenv(3c)| */
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<getourenv.h>
#include	<localmisc.h>

#include	"gettmpdname.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported variables */


/* exported subroutines */

cchar *gettmpdnamex(mainv envv) noex {
	cchar		*vn = varname.tmpdir ;
	cchar		*vp = nullptr ;
	if (envv) {
	    vp = getourenv(envv,vn) ;
	}
	if ((vp == nullptr) || (vn[0] == '\0')) {
	    vp = getenv(vn) ;
	}
	if ((vp == nullptr) || (vn[0] == '\0')) {
	    vp = sysword.w_tmpdir ;
	}
	return vp ;
} 
/* end subroutine (gettmpdnamex) */


