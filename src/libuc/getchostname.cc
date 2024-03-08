/* getchostname SUPPORT */
/* lang=C++20 */

/* subroutine to get a canonical hostname */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to get a canonical INET hostname
	for a supplied name. Note carefully that the returned
	hostname, if any, may NOT be a name that can be translated
	into a good INET address. In other words, this subroutine
	uses its own definition of a "canonical" name and that
	definition does NOT necessarily include the fact that the
	resulting name can be translated into a good INET address.
	If you want a name that is guaranteed to be translatable
	into a valid INET address, then you want to investigate the
	subroutine GETEHOSTNAME (Get Entry HostName).

	Having said that the resuling name is not guaranteed to be
	translatable, a good translation facility will generally
	figure out that the given name is something that can be
	translated given the existing host information.

	Synopsis:
	int getchostname(cchar *name,char *hostname) noex

	Arguments:
	name		name to lookup
	hostname	if not NULL, a buffer to hold the resulting hostname

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>

#include	"getchostname.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int getchostname(cchar *name,char *hostname) noex {
	return getcname(name,hostname) ;
}
/* end if (getchostname) */


