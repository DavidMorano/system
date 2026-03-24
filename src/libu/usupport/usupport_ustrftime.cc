/* usupport_ustrftime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ustrftime

	Description:
	This is a standard POSIX® subroutine that formats time elements
	as given in a POSIX® TM object into a string representation
	and stored in the supplied destination buffer.

	Synopsis:
    	sysret_t ustrftime(char *dbuf,int dlen,cchar *fmt,CTM *tmp) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	fmt		format string
	tmp		pointer to TM object

	Returns:
	>=0		OK, number of characters stored in destination buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"usupport_ustrftime.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    sysret_t ustrftime(char *dbuf,int dlen,cchar *fmt,CTM *tmp) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt && tmp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        csize	dsize = size_t(dlen + 1) ;
		errno = 0 ;
	        if (size_t rsize ; (rsize = strftime(dbuf,dsize,fmt,tmp)) > 0) {
	            rs = intsat(rsize) ;
	        } else if (rsize == 0) {
	            dbuf[0] = '\0' ;
	            rs = SR_OVERFLOW ;
	        } else if (errno) {
		    rs = (- errno) ;
		} else {
		    rs = SR_NOMSG ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ustrftime) */
} /* end namespace (libu) */


/* local subroutines */


