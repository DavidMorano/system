/* trailaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find trailing address */
/* version %I% last-modified %G% */


/* revision history:

	= 1997-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1997 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	trailaddr

	Description:
	This subroutine finds a trailing address in a "stacked"
	address.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

const char *trailaddr(cchar addr[],int addrlen) noex {
	int		al ;
	const char	*cp ;

	if (addr == NULL)
	    return NULL ;

	al = (addrlen >= 0) ? addrlen : strlen(addr) ;

/* remove trailing bangs */

	while ((al > 0) && (addr[al - 1] == '!'))
	    al -= 1 ;

	cp = addr + al ;
	while ((cp > addr) && (cp[-1] != '!'))
	    cp -= 1 ;

	if (cp == addr)
	    return cp ;

	if ((cp > addr) && (cp[-1] == '!'))
		cp -= 1 ;

	while ((cp > addr) && (cp[-1] != '!'))
	    cp -= 1 ;

	if (cp == addr)
	    return cp ;

	return cp ;
}
/* end subroutine (trailaddr) */


