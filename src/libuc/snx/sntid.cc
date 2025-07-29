/* sntid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a string representation of a Pthread Thread-ID */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sntid

	Description:
	We convert a Pthread Thread-ID into a string representation.

	Synopsis:
	int sntid(char *rbuf,int rlen,pthread_t tid) noex

	Arguments:
	rbuf		result buffer
	rlen		length of supplied result buffer
	tid		pthread thread-ID to convert

	Returns:
	>=0		length of create string in result buffer
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

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>		/* |pthread_t| */
#include	<climits>		/* UINT_MAX */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |uintptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ctdec.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sntid(char *dp,int dl,pthread_t tid) noex {
	uintptr_t	up = uintptr_t(tid) ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    uint	uv = cast_static<uint>(up & UINT_MAX) ;
	    rs = ctdecui(dp,dl,uv) ;
	}
	return rs ;
}
/* end subroutine (sntid) */


