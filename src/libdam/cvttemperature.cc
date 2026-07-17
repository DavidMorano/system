/* cvttemperature SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert temperature */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"cvttemperature.h"


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

/* convert degrees Fahrenheit to Centigrade */
double cvtfahtocen(double fah) noex {
	return ((fah - 32.0) * 5.0 / 9.0) ;
} /* end subroutine (cvtfahtocen) */

/* convert degrees Centigrade to Fahrenheit */
double cvtcentofah(double cen) noex {
	return ((cen * 9.0 / 5.0) + 32.0) ;
} /* end subroutine (cvtcentofah) */


