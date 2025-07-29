/* snadd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* append a c-string to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snaddx

	Description:
	Add some c-string to the end of an existing string.  Return the
	incremental amount added.

	Synopsis:
	int snaddx(char *rbuf,int rlen,int rl,int n,...) noex

	Arguments:
	rbuf		result c-string buffer pointer
	rlen		result c-string buffer length
	rl		length of result buffer already filled
	sp		new c-string pointer to be added
	sl		new c-string length to be added

	Returns:
	>=0		the amount of additional string added to
			the existing c-string buffer
	<0		error (system-return)

	Notes:
	This subroutine should be the same as (the older)
	|storebuf_str(3uc)|.

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
#include	<cstdarg>		/* |va_list(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"snadd.h"		/* |snadd(3uc)| */


/* local defines */


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
	if (rbuf) ylikely {
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


