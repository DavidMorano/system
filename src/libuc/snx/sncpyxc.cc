/* sncpyxc SUPPORT */
/* charset=ISO8859-1 */
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

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* system-returns */
#include	<toxc.h>
#include	<localmisc.h>

#include	"sncpyxc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references (local subroutine-templates) */

template<int (*toxc)(int)>
int sncpyxc(char *dbuf,int dlen,cchar *sp) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dbuf && sp) ylikely {
	    int		i = 0 ; /* used-afterwards */
	    char	*dp = dbuf ;
	    rs = SR_OK ;
	    if (dlen < 0) dlen = INT_MAX ;
	    while ((i < dlen) && sp[i]) {
	        *dp++ = charconv(toxc(sp[i++])) ;
	    }
	    *dp = '\0' ;
	    if (sp[i] == '\0') {
	        rl = intconv(dp - dbuf) ;
	    } else {
		rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
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


