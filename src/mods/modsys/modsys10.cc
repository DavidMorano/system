/* modsys10 MODYLE (implementation) SUPPORT */
/* charset=ISO8859-1 */
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

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |MAILALIASLEN| */

#include	"modsys.hh"

module modsys ;

/* local defines */

#define	NODENAMEMULT		2	/* node-name multiplier */
#define	HOSTNAMEMULT		2	/* host-name multiplier */

#ifndef	MAILALIASLEN
#define	MAILALIASLEN		(1 * 1024)
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

int	mailvalues::nodenamemult =	NODENAMEMULT ;
int	mailvalues::hostnamemult =	HOSTNAMEMULT ;
int	mailvalues::mailaliaslen =	MAILALIASLEN ;

mailvalues		mailvalue ;


/* exported subroutines */


