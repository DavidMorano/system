/* ureserve1 MODULE (isnot - implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces - isnot */
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
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bitset>		/* C++STD <- the money shot! */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

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
} ; /* end array */

constexpr int	naccess[] = {
	SR_ACCESS,
	SR_PERM,			
	0	
} ; /* end array */

constexpr int	nvalid[] = {
	SR_INVALID,
	SR_DOM,
	SR_RANGE,
	SR_NOENT,
	SR_NOTSUP,
	0	
} ; /* end array */

constexpr int	nlib[] = {
	SR_NOENT,
	SR_ACCESS,
	SR_LIBACC,
	SR_NOTDIR,
	SR_STALE,
	0	
} ; /* end array */

constexpr int	rnoterm[] = {
	SR_BADF,
	SR_BADFD,
	SR_NOTTY,
	SR_ACCESS,
	0
} ; /* end array */

constexpr int	rnosupport[] = {
	SR_NOSYS,
	SR_NOTSUP,
	SR_PROTONOSUPPORT,
	SR_SOCKTNOSUPPORT,
	SR_OPNOTSUPP,
	SR_PFNOSUPPORT,
	SR_AFNOSUPPORT,
	0
} ; /* end array */


/* exported variables */


/* exported subroutines */

bool isoneof(cint *a,int rs) noex {
	bool		f = false ;
	for (int i = 0 ; a[i] != 0 ; i += 1) {
	    if ((f = (rs == a[i]))) break ;
	} /* end if */
	return f ;
} /* end subroutine (isoneof) */

bool isnotaccess(int rs) noex {
	int		f = false ;
	f = f || isoneof(naccess,rs) ;
	f = f || isoneof(npresent,rs) ;
	return f ;
} /* end subroutine (isnotaccess) */

bool isnotlib(int rs) noex {
	return isoneof(nlib,rs) ;
} /* end subroutine */

bool isnotpresent(int rs) noex {
	return isoneof(npresent,rs) ;
} /* end subroutine */

bool isnotsupport(int rs) noex {
	return isoneof(rnosupport,rs) ;
} /* end subroutine */

bool isnotterm(int rs) noex {
	return isoneof(rnoterm,rs) ;
} /* end subroutine */

bool isnotvalid(int rs) noex {
	return isoneof(nvalid,rs) ;
} /* end subroutine */


/* local subroutines */


