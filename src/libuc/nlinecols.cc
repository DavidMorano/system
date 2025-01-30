/* nlinecols SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* calculate number of columns used by a line of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nlinecols

	Description:
	This subroutine calculates the number of columns that a
	line of characters takes up on a terminal, with tab stops
	set.

	Synopsis:
	int nlinecols(int ntab,int ccol,cchar *lbuf,int llen) noex

	Arguments:
	ntab		number of columns in a TAB character
	ccol		current column number
	lbuf		source string 
	llen		length of source string

	Returns:
	-		column number after line if used up

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int nlinecols(int ntab,int ccol,cchar *lbuf,int llen) noex {
	if (llen < 0) llen = strlen(lbuf) ;
	for (int i = 0 ; i < llen ; i += 1) {
	    ccol += charcols(ntab,ccol,lbuf[i]) ;
	} /* end for */
	return ccol ;
}
/* end subroutine (nlinecols) */


