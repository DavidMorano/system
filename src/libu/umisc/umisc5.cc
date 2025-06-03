/* umisc5 MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Module:
	ureserve

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

module umisc ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snaddx(char *rbuf,int rlen,int rl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
		cchar	*sp = (char *) va_arg(ap,char *) ;
		rs = snadd(rbuf,rlen,rl,sp) ;
		len += rs ;
		rl += rs ;
	    } /* end for */
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snaddx) */


