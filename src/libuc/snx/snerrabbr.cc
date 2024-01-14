/* snerrabbr SUPPORT */
/* lang=C++20 */

/* make the string repreentation of a system-error return number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snerrabbr

	Description:
	We create the string repreentation of a system-error return
	number.

	Synopsis:
	int snerrabbr(char *dbuf,int dlen,int n) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	n		signal number

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<ctdec.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern cchar	*strerrabbr(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int snerrabbr(char *dbuf,int dlen,int n) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    if (cchar *s ; (s = strerrabbr(n)) != nullptr) {
	        rs = sncpy1(dbuf,dlen,s) ;
	    } else {
	        rs = ctdeci(dbuf,dlen,n) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snerrabbr) */


