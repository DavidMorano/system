/* uc_getauid */
/* charset=ISO8859-1 */

/* interface component for UNIX® library-3c */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getauid

	Description:
	This subroutine retrieves from the system the Audit-UID of
	the current process.

	Synopsis:
	int uc_getauid()

	Arguments:
	*node*

	Returns:
	>=0		UID of process (as an integer)
	<0		error (system-return)

*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>

#if	SYSHAS_AUDIT
#include	<bsm/audit.h>
#endif

#include	<usystem.h>


/* local defines */


/* external subroutines */


/* local variables */


/* forward references */


/* exported subroutines */


int uc_getauid()
{
	int		rs ;
	uid_t		auid = 0 ;

#if	SYSHAS_AUDIT
	rs = SR_OK ;
	if (getauid(&auid) < 0) rs = (- errno) ;
	else rs = (int) (auid & INT_MAX) ;
#else
	rs = SR_NOSYS ;
#endif /* SYSHAS_AUDIT */

	return rs ;
}
/* end subroutine (uc_getauid) */


