/* bufprintf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to process a format-string into a buffer */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	bufprintf

	Description:
	This subroutine is used by 'printf' type routines to format
	an output "buffer" from a format specification.  This routine
	has no support for floating point conversion since floating
	point formats are not general enough for the most portable
	applications.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<usystem.h>
#include	<fmtstr.h>
#include	<localmisc.h>

#include	"bufprintf.h"


/* local defines */


/* external references */


/* local structures */


/* forward references */


/* local variables */

cint		mo = 0 ;		/* mode-options */


/* exported variables */


/* exported subroutines */

int bufprintf(char *dbuf,int dlen,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dbuf && fmt) {
	    va_begin(ap,fmt) ;
	    rs = fmtstr(dbuf,dlen,mo,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bufprintf) */

int bufvprintf(char *dbuf,int dlen,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt && ap) {
	    rs = fmtstr(dbuf,dlen,mo,fmt,ap) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bufvprintf) */


