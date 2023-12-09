/* isNotAccess */

/* determine if a file is not present from its return-status */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine determines if a file is not present from its
	return-status.

	Synopsis:

	int isNotAccess(int rs)

	Arguments:

	rs		return-status from a file access

	Returns:

	1		condition found
	0		condition not found


*******************************************************************************/


#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	isOneOf(const int *,int) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static const int	nofiles[] = {
	SR_NOENT,
	SR_ACCESS,
	0	
} ;


/* exported subroutines */


int isNotAccess(int rs)
{
	return isOneOf(nofiles,rs) ;
}
/* end subroutine (isNotAccess) */


