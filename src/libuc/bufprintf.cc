/* bufprintf SUPPORT */
/* lang=C++20 */

/* subroutine to process a format-string into a buffer */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used by 'printf' type routines to format
	an output "buffer" from a format specification. This routine
	has no support for floating point conversion since floating
	point formats are not general enough for the most portable
	applications.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdarg>
#include	<usystem.h>
#include	<format.h>
#include	<localmisc.h>

#include	"bufprintf.h"


/* local defines */


/* external references */


/* local structures */


/* forward references */


/* exported variables */


/* exported subroutines */

int bufprintf(char *dbuf,int dlen,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt) {
	    va_list	ap ;
	    cint	m = 0 ;
	    va_begin(ap,fmt) ;
	    rs = format(dbuf,dlen,m,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bufprintf) */

int vbufprintf(char *dbuf,int dlen,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt && ap) {
	    cint	m = 0 ;
	    rs = format(dbuf,dlen,m,fmt,ap) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vbufprintf) */


