/* snabbrnum SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the string repreentation of a signal number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snabbr{x}

	Description:
	We create the string repreentation of a {errno|signal}
	number.

	Synopsis:
	int snabbrerr(char *dbuf,int dlen,int n) noex
	int snabbrsig(char *dbuf,int dlen,int n) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	n		{errno|signal} number

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
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
#include	<strx.h>
#include	<localmisc.h>

#include	"snabbr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef cchar *(*strabbr_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int snabbrx(strabbr_f abbr,char *dbuf,int dlen,int n) noex {
	int		rs = SR_FAULT ;
	if (dbuf) ylikely {
	    if (cchar *s ; (s = abbr(n)) != nullptr) {
	        rs = sncpy(dbuf,dlen,s) ;
	    } else {
	        rs = ctdec(dbuf,dlen,n) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snabbrx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int snabbrerr(char *dbuf,int dlen,int n) noex {
    	return snabbrx(strabbrerr,dbuf,dlen,n) ;
}
/* end subroutine (snabbrerr) */

int snabbrsig(char *dbuf,int dlen,int n) noex {
    	return snabbrx(strabbrsig,dbuf,dlen,n) ;
}
/* end subroutine (snabbrsig) */


