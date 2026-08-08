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
	snabbrname(3uc)
	snabbrerr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucodenames.h>		/* LIBU */
#include	<snwcpy.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"snabbr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*ucode_f)(int,ccharpp) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int snabbrx(ucode_f get,char *dbuf,int dlen,int n) noex {
	int		rs = SR_FAULT ;
	if (dbuf) ylikely {
	    if (cchar *rp ; (rs = get(n,&rp)) >= 0) {
	        rs = snwcpy(dbuf,dlen,rp,rs) ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = ctdec(dbuf,dlen,n) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snabbrx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int snabbrerr(char *dbuf,int dlen,int n) noex {
    	return snabbrx(ucodename_sr,dbuf,dlen,n) ;
} /* end subroutine (snabbrerr) */

int snabbrsig(char *dbuf,int dlen,int n) noex {
    	return snabbrx(ucodename_sig,dbuf,dlen,n) ;
} /* end subroutine (snabbrsig) */


