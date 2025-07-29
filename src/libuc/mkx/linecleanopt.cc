/* linecleanopt SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* line-cleaning options */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	linecleanopt

  	Description:
	This module contains various line-cleaning options.

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

#include	"linecleanopt.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cint	linecleanoptms::nonsub	= (1 << linecleanopto_nonsub) ;
cint	linecleanoptms::nulsub	= (1 << linecleanopto_nulsub) ;
cint	linecleanoptms::nulign	= (1 << linecleanopto_nulign) ;

const linecleanoptms	linecleanoptm ;


/* exported subroutines */


