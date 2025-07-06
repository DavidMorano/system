/* sinon SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of the first character not something */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sinon{xxx}

	Description:
	This subroutine searchs for an a character that is not part
	of a certain character class.

	Synopsis:
	int sinon{xxx}(cchar *sp,int sl) noex

	Arguments:
	xxx	one of: oct, dec, hex
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of found character
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*isnon_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int sinon(cchar *sp,int sl,isnon_f isx) noex {
	int		i{} ; /* used-afterwards */
	bool		f = false ;
	for (i = 0 ; sl && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = (! isx(ch)) ;
	    if (f) break ;
	    sl -= 1 ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sinon) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sinonoct(cchar *sp,int sl) noex {
    	return sinon(sp,sl,isoctlatin) ;
}

int sinondec(cchar *sp,int sl) noex {
    	return sinon(sp,sl,isdeclatin) ;
}

int sinonhex(cchar *sp,int sl) noex {
    	return sinon(sp,sl,ishexlatin) ;
}


