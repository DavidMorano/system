/* usysbasic3 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isOneOf
	ifNotPresent

	Description:

	Synopsis:
	bool isOneOf(cint *rarr,int rs) noex
	bool isNotPresent(int rs) noex

	Arguments:
	rarr		return-status array
	rs		return-status

	Returns:
	false		not found
	true		yes found

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module usysbasic ;

/* local defines */


/* imported namespaces */


/* local typenames */


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

bool isonebad(cint *a,int rs) noex {
	bool		f = false ;
	if (rs < 0) ylikely {
	    f = isoneof(a,rs) ;
	} /* end if (is-bad) */
	return f ;
} /* end subroutine (isonebad) */

bool isNotPresent(int rs) noex {
	return isonebad(npresent,rs) ;
} /* end subroutine (isNotPresent) */

bool isNotAccess(int rs) noex {
	int	f = false ;
	f = f || isonebad(naccess,rs) ;
	f = f || isonebad(npresent,rs) ;
	return f ;
} /* end subroutine (isNotAccess) */

bool isNotValid(int rs) noex {
	return isonebad(nvalid,rs) ;
} /* end subroutine (isNotValid) */

bool isNotLib(int rs) noex {
	return isonebad(nlib,rs) ;
} /* end subroutine (isNotLib) */

bool isNotTerm(int rs) noex {
	return isonebad(rnoterm,rs) ;
} /* end subroutine */

bool isNotSupport(int rs) noex {
	return isonebad(rnosupport,rs) ;
} /* end subroutine */


