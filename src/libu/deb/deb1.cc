/* deb1 MODULE (module-implemetation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-02-23, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	debon
	debfd
	debprintf

	Description:
	These subroutines are a hack for deb-printfs where the normal
	debugging facilities are not available (for whatever reason).

	Synopsis:
	int debfd(int dfd) noex
	int debprintf(cchar *func,cchar *fmt,...) noex

	Arguments:
	dfd		file-description to use for debug output
	func		c-string function-name pointer
	fmt		c-string formatting specification pointer
	...		|printf|-like arguments

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |sncpy(3u)| */
#include	<usysutility.hh>	/* LIBU |snvprintf(3u)| */
#include	<localmisc.h>		/* LIBU */

module deb ;


/* local defines */

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	(4 * 1024)
#endif


/* imported namespaces */

using libu::sncpy ;			/* subroutine */
using libu::snprintf ;			/* subroutine */
using libu::snvprintf ;			/* subroutine */


/* external subroutines */


/* local variables */

constexpr int	flen = MAXPATHLEN ;

int		dfd = -1 ;


/* exported variables */

debon_mgr	debon ;


/* exported subroutines */

debon_mgr::operator int () noex {
	return (dfd >= 0) ;
} /* end method */

int debfd(int fd) noex {
    	dfd = fd ;
	return SR_OK ;
} /* end subroutine (debfd) */

int debprintf(cchar *func,cchar *fmt,...) noex {
    	va_list		ap ;
	cnothrow	nt{} ;
	cnullptr	np{} ;
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (func && fmt) {
	    rs = SR_OK ;
	    if (dfd >= 0) {
	        va_begin(ap,fmt) ;
    	        rs = SR_NOMEM ;
	        if (char *fbuf ; (fbuf = new(nt) char[flen + 1]) != np) {
		    if ((rs = sncpy(fbuf,flen,func,": ")) >= 0) {
		        cint	bl = (flen - rs) ;
		        char	*bp = (fbuf + rs) ;
		        len += rs ;
	                if ((rs = snvprintf(bp,bl,fmt,ap)) >= 0) {
			    len += rs ;
		            rs = u_write(dfd,fbuf,len) ;
	                } /* end if (snvprintf) */
		    } /* end if (sncpy) */
		    delete [] fbuf ;
	        } /* end if (m-a-f) */
	        va_end(ap) ;
	    } /* end if (enabled) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (debprintf) */


