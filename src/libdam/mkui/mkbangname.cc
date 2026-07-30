/* mkbangname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

#define	CF_FULLNAME	0		/* use full-name */
#define	CF_MAILNAME	1		/* use mail-name */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkbangname

	Description:
	This subroutine create a name that combines the nodename
	and the username into a "bangname."

	Synopsis:
	int mkbangname(char *nbuf,int nlen,userinfo *uip) noex

	Arguments:
	nbuf		buffer to receive resulting name
	nlen		length of supplied buffer
	uip		pointer to USERINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<userinfo.h>		/* LIBUC */
#include	<mkui.h>		/* LIBUC |mkuibang(3dam)| */
#include	<localmisc.h>		/* LIBU */

#include	"mkbangname.h"

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkbangname(char *rbuf,int rlen,userinfo *uip) noex {
    	return mkuibang(rbuf,rlen,uip) ;
} /* end subroutine (mkbangname) */


