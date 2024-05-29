/* isnot SUPPORT */
/* lang=C++20 */

/* determine if a file is not present from its return-status */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	isNotPresent
	isNotAcces
	isNotValid
	isNotLib

	Description:
	This subroutine determines if a file is not present from its
	return-status.

	Synopsis:
	int isNot{X}(int rs) noex

	Arguments:
	rs		return-status from a file access

	Returns:
	TRUE		file is *not* present (true)
	FALSE		file is present (false)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<usysrets.h>

#include	"isoneof.h"
#include	"isnot.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	npresent[] = {
	SR_NOENT,
	SR_NETUNREACH,
	SR_NETDOWN,
	SR_HOSTUNREACH,
	SR_HOSTDOWN,
	SR_TIMEDOUT,
	SR_CONNREFUSED,
	SR_LIBACC,			/* libs can be "files" also! */
	SR_STALE,
	0	
} ;

constexpr int	naccess[] = {
	SR_ACCESS,
	SR_PERM,
	0	
} ;

constexpr int	nvalid[] = {
	SR_INVALID,
	SR_DOM,
	SR_RANGE,
	SR_NOENT,
	SR_NOTSUP,
	0	
} ;

static constexpr int	nlib[] = {
	SR_NOENT,
	SR_ACCESS,
	SR_LIBACC,
	SR_NOTDIR,
	SR_STALE,
	0	
} ;

constexpr int		rsnoterm[] = {
	SR_BADF,
	SR_BADFD,
	SR_NOTTY,
	SR_ACCESS,
	0
} ;


/* exported variables */


/* exported subroutines */

bool isNotPresent(int rs) noex {
	return isOneOf(npresent,rs) ;
}
/* end subroutine (isNotPresent) */

bool isNotAccess(int rs) noex {
	int		f = false ;
	f = f || isOneOf(naccess,rs) ;
	f = f || isOneOf(npresent,rs) ;
	return f ;
}
/* end subroutine (isNotAccess) */

bool isNotValid(int rs) noex {
	return isOneOf(nvalid,rs) ;
}
/* end subroutine (isNotValid) */

bool isNotLib(int rs) noex {
	return isOneOf(nlib,rs) ;
}
/* end subroutine (isNotLib) */

bool isNotTerm(int rs) noex {
	return isOneOf(rsnoterm,rs) ;
}


