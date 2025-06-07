/* ureserve1 MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	ureserve

	Description:
	This module provides some subroutines are used in certain
	circumstances where the LIBUC library is not availble (for
	whatever reasons).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* <- the money shot! */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

module ureserve ;

/* local defines */


/* imported namespaces */


/* local typedefs */


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
	SR_PERM,			/* Apple-Darwin from |stat(2)| */
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

constexpr int	nlib[] = {
	SR_NOENT,
	SR_ACCESS,
	SR_LIBACC,
	SR_NOTDIR,
	SR_STALE,
	0	
} ;

constexpr int	rnoterm[] = {
	SR_BADF,
	SR_BADFD,
	SR_NOTTY,
	SR_ACCESS,
	0
} ;

constexpr int	rnosupport[] = {
	SR_NOSYS,
	SR_NOTSUP,
	SR_PROTONOSUPPORT,
	SR_SOCKTNOSUPPORT,
	SR_OPNOTSUPP,
	SR_PFNOSUPPORT,
	SR_AFNOSUPPORT,
	0
} ;


/* exported variables */


/* exported subroutines */

bool isoneof(cint *a,int rs) noex {
	bool		f = false ;
	for (int i = 0 ; a[i] != 0 ; i += 1) {
	    if ((f = (rs == a[i]))) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isoneof) */

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
	return isOneOf(rnoterm,rs) ;
}

bool isNotSupport(int rs) noex {
	return isOneOf(rnosupport,rs) ;
}


/* local subroutines */


