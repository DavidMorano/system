/* snsigabbr SUPPORT */
/* lang=C++20 */

/* make the string repreentation of a signal number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snsigabbr

	Description:
	We create the string repreentation of a signal number.

	Synopsis:
	int snsigabbr(char *dbuf,int dlen,int n) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	n		signal number

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<ctdec.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern cchar	*strsigabbr(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snsigabbr(char *dbuf,int dlen,uint n) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    if (cchar *s ; (s = strsigabbr(n)) != nullptr) {
	        rs = sncpy1(dbuf,dlen,s) ;
	    } else {
	        rs = ctdecui(dbuf,dlen,n) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snsigabbr) */


