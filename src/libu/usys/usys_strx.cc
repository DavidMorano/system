/* usys_strx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_strx

  	Description:
	I provide some missing system facilities in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strpbrk(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"usys_strx.h"


char *strbrk	(cchar *sp,cchar *sc)	noex {
    	return strpbrk(sp,sc) ;
} /* end */


