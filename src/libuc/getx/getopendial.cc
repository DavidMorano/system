/* getopendial SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the OPENDIAL code for a dialer specification (string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getopendial

	Description:
	Get the open-dial code (an integer value) for an open-dial
	name string.

	Synopsis:
	int getopendial(cchar *dialspec) noex

	Arguments:
	dialspec	the dialer specification string

	Returns:
	>=0		the open-dialer code for the specified dialer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<matstr.h>		/* LIBUC */
#include	<opendials.h>		/* LIBUC |opendialer(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"getopendial.h"


/* local defines */

#define	names	opendial_data.name


/* imported namespaces */

using libuc::opendial_data ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getopendial(cchar *name) noex {
    	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		rs = SR_NOTFOUND ;
		if ((v = matstr(names,name,-1)) >= 0) {
		    rs = SR_OK ;
		} else {
	            if (strcmp(name,"unix") == 0) {
			rs = SR_OK ;
		        v = opendial_uss ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
} /* end subroutine (getopendial) */


