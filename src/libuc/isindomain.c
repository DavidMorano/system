/* isindomain */
/* lang=C20 */

/* subroutine to determine if the name is in the given domain */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isindomain

	Description:
	This subroutine determines if a given hostname is in the
	specified domain or not.

	Synopsis:

	int isindomain(cchar *nodename,cchar *domainname) noex

	Arguments:
	nodename	if not NULL, a buffer to hold the resulting hostname
	domainname	if not NULL, the domain name to check against

	Returns:
	TRUE		it is an INET address
	FALSE		it is not

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<stdlib.h>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<strxcmp.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported subroutines */

int isindomain(cchar *name,cchar *domainname) noex {
	int		f = true ;
	char		*tp ;
	if ((tp = strchr(name,'.')) != nullptr) {
	    f = false ;
	    if (tp[1]) {
		f = (strcasecmp((tp + 1),domainname) == 0) ;
	    } /* end if */
	} /* end if */
	return f ;
}
/* end subroutine (isindomain) */


