/* sncpyxc */
/* lang=C++20 */

/* copy a c-string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine constructs a single string from a single
	specificed string.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<clanguage.h>
#include	<toxc.h>

#include	"sncpyxc.h"


/* local defines */


/* external subroutines */


/* local subroutine-templates */

template<int (*toxc)(int)>
int sncpyxc(char *dbuf,int dlen,cchar *sp) noex {
	int		i = 0 ;
	char		*dp = dbuf ;
	if (dlen < 0) dlen = INT_MAX ;
	while ((i < dlen) && sp[i]) {
	    *dp++ = toxc(sp[i++]) ;
	}
	*dp = '\0' ;
	return (sp[i] == '\0') ? (dp - dbuf) : SR_OVERFLOW ;
}
/* end subroutine-template (sncpyxc) */


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


