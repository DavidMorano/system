/* mailvalues SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* various mail (message) values */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailvalues

	Description:
	This object contains various mail (message) values.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"mailvalues.hh"


/* local defines */

#define	HOSTNAMEMULT		2	/* host-name multiplier */
#define	NODENAMEMULT		2	/* node-name multiplier */


/* exported variables */

const int	mailvalues::hostnamemult =	HOSTNAMEMULT ;
const int	mailvalues::nodenamemult =	NODENAMEMULT ;


