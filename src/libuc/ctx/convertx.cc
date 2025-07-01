/* convertx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a double floating point value to various representations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Names:
	converte
	convertf
	convertg

	Description:
	These subroutines are similar to the |ecvt(3c)| |fcvt(3c)|,
	and |gcvt(3c)| POSIX® subroutines, but are reentrant.  They
	are modeled after similar (almost indetical) subroutines
	that were originally part of the Solaris® operating system.
	But those Solaris® subroutines were proprietary and
	non-portable.

	Synopsis:
	int converte(double v,int prec,int *dpp,int *fsign,char *buf) noex
	int convertf(double v,int prec,int *dpp,int *fsign,char *buf) noex
	int convertg(double v,int prec,int trailing,char *buf) noex

	Arguments:
	v		value (of type |double|) to be converted
	prec		precision (places after the decimal point)
	dpp		pointer to |int| to receive position of decimal point
	fsign		pointer to |int| to receive sign of result
	trailing	??
	buf		buffer to receive result

	Returns:
	>=0		length of fill result buffer
	<0		error (system-return)

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

#include	"convertx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int converte(double v,int prec,int *dpp,int *fsign,char *buf) noex {
	(void) v ;
	(void) prec ;
	(void) dpp ;
	(void) fsign ;
	(void) buf ;
	return 0 ;
}

int convertf(double v,int prec,int *dpp,int *fsign,char *buf) noex {
	(void) v ;
	(void) prec ;
	(void) dpp ;
	(void) fsign ;
	(void) buf ;
	return 0 ;
}

int convertg(double v,int prec,int trailing,char *buf) noex {
	(void) v ;
	(void) prec ;
	(void) trailing ;
	(void) buf ;
	return 0 ;
}


