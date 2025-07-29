/* snaddslash SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* append a slash character ('/') to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snaddslashx

	Description:
	Add a shash character ('/') to the end of an existing c-string.
	Return the incremental amount added.

	Synopsis:
	int snaddslash(char *rbuf,int rlen,int rl) noex

	Arguments:
	rbuf		result c-string buffer pointer
	rlen		result c-string buffer length
	rl		length of result buffer already filled

	Returns:
	>=0		the amount of additional string added to
			the existing c-string buffer
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
#include	<localmisc.h>

#include	"snaddslash.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snaddslash(char *rbuf,int rlen,int rl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && (rl >= 0)) ylikely {
	        rs = SR_OK ;
		if ((rl == 0) || (rbuf[rl - 1] != '/')) {
		    rbuf[rl] = '/' ;
		    len += 1 ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snaddslash) */


