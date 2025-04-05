/* sncpyxc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}c

	Description:
	These subroutines construct a single string (into a
	caller-supplied counted buffer) from a single specificed
	c-string.  But a character-case conversion is available.
	Available character-case conversions are:
	+ none (straight through copy)
	+ convert to lower-case
	+ convert to upper-case
	+ convert to folded-case

	Synopsis:
	int sncpy{x}c(char *rbuf,int rlen,cchar *sp) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	sp		source c-string to copy-convert

	Returns:
	>=0		number of bytes copies (coverted)
	<0		error (system-return)[

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<toxc.h>

#include	"sncpyxc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references (local subroutine-templates) */

template<int (*toxc)(int)>
int sncpyxc(char *dbuf,int dlen,cchar *sp) noex {
    	int		rl = SR_OVERFLOW ;
	int		i = 0 ;
	char		*dp = dbuf ;
	if (dlen < 0) dlen = INT_MAX ;
	while ((i < dlen) && sp[i]) {
	    *dp++ = charconv(toxc(sp[i++])) ;
	}
	*dp = '\0' ;
	if (sp[i] == '\0') {
	    rl = intconv(dp - dbuf) ;
	}
	return rl ;
}
/* end subroutine-template (sncpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpybc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc<tobc>(dp,dl,s1) ;
}

int sncpylc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc<tolc>(dp,dl,s1) ;
}

int sncpyuc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc<touc>(dp,dl,s1) ;
}

int sncpyfc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc<tofc>(dp,dl,s1) ;
}


