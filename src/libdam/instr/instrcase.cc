/* instrcase SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* change the character case in a c-string in-place */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	instrcaselower
	instrcaseupper
	instrcasefold

	Description:
	These subroutines perform in-place c-string case conversions.

	Synopsis:
	int instrcaselower(char *rp,int rl) noex
	int instrcaseupper(char *rp,int rl) noex
	int instrcasefold(char *rp,int rl) noex

	Arguments:
	rp		source and destination buffer
	rl		length of source string

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<char.h>		/* |char_isuc(3uc)| + ... */
#include	<localmisc.h>

#include	"instrcase.h"


/* local defines */


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef bool	(*chis_f)(int) noex ;
    typedef uchar	(*chto_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct casex {
    	chis_f	chis ;
	chto_f	chto ;
	casex(chis_f i,chto_f t) noex : chis(i), chto(t) { } ;
	int operator () (char *rp,int rl) noex {
	    int		rs = SR_FAULT ;
	    int		i = 0 ; /* return-value */
	    if (rp) ylikely {
	        rs = SR_OK ;
	        if (rl < 0) rl = INT_MAX ;
	        for (i = 0 ; (i < rl) && *rp ; i += 1) {
		    cint ch = mkchar(rp[i]) ;
	            if (! chis(ch)) {
			rp[i] = chto(ch) ;
		    }
	        } /* end for */
	    } /* end if (non-null) */
	    return (rs >= 0) ? i : rs ;
        } /* end method (operator) */
    } ; /* end struct (casex) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int instrcaselower(char *rp,int rl) noex {
    	casex cx(char_islc,char_tolc) ;
	return cx(rp,rl) ;
}

int instrcaseupper(char *rp,int rl) noex {
    	casex cx(char_isuc,char_touc) ;
	return cx(rp,rl) ;
}

int instrcasefold(char *rp,int rl) noex {
    	casex cx(char_isfc,char_tofc) ;
	return cx(rp,rl) ;
}


