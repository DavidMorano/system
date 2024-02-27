/* isobjspecial SUPPORT */
/* lang=C++20 */

/* determine if a shared-object handle is special or not */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isobjspecial

	Description:
	This subroutine determines if a library file is a special
	object or not.

	Synopsis:
	int isobjspecial(void *sop) noex

	Arguments:
	sop		shared-object handle

	Returns:
	TRUE		shared-object is special
	FALSE		shared-object is regular

	Notes:
	MacOS (Darwin) and possibly other OSes do not have the handle
	of RTLD_PROBE.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<dlfcn.h>
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ismisc.h"


/* local defines */

#ifndef	RTLD_PROBE
#define	RTLD_PROBE		voidp(1)	/* real vals are neg */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static const void	*objs[] = {
	RTLD_DEFAULT,
	RTLD_NEXT,
	RTLD_SELF,
	RTLD_PROBE,
	nullptr
} ;


/* exported subroutines */

bool isobjspecial(void *sop) noex {
	bool		f = false ;
	for (int i = 0 ; objs[i] ; i += 1) {
	    f = (sop == objs[i]) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isobjspecial) */


