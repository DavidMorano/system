/* usysflag SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* system flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998,2014 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Group:
	usysflag

	Description:
	This struct supplies some flags indicating what operating
	system the current program is on.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"usysflag.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */

const bool	usysflags::sunos 	= F_SUNOS ;
const bool	usysflags::darwin	= F_DARWIN ;
const bool	usysflags::linux	= F_LINUX ;

constexpr usysflags		usysflag ;


/* exported subroutines */


