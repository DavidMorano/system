/* snerrabbr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the string repreentation of a system-error return number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snerrabbr

	Description:
	We create the string repreentation of a system-error return
	number.

	Synopsis:
	int snerrabbr(char *dbuf,int dlen,int n) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	n		signal number

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sncpyx.h>
#include	<ctdec.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern cchar	*strerrabbr(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snerrabbr(char *dbuf,int dlen,int n) noex {
	int		rs = SR_FAULT ;
	if (dbuf) ylikely {
	    if (cchar *s ; (s = strerrabbr(n)) != nullptr) {
	        rs = sncpy(dbuf,dlen,s) ;
	    } else {
	        rs = ctdec(dbuf,dlen,n) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snerrabbr) */


