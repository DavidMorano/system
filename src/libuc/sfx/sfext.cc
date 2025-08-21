/* sfext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-find a file-name extension */
/* version %I% last-modified %G% */

#define	CF_SIRCHR	1		/* used |sirchr(3uc)| */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfext

	Description:
	This subroutine will look for and return the file extension
	(file suffix) of a given file-name (given as a counted
	c-string), if one exits.

	Synopsis:
	int sfext(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		base source c-string pointer
	sl		base source c-string length
	rpp		pointer to result pointer to store found pointer

	Returns:
	>=0		length of found suffix c-string
	<0		no suffix was found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<six.h>			/* |sirchr(3uc)| */
#include	<ascii.h>		/* |CH_DOT| + |CH_SLASH| */
#include	<localmisc.h>

#include	"sfx.h"
#include	"sfext.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/chrset.ccm"

import libutil ;			/* |getlenstr(3u)| */
import chrset ;

/* local defines */

#ifndef	CF_SIRCHR
#define	CF_SIRCHR	1		/* used |sirchr(3uc)| */
#endif


/* external subroutines */

extern "C++" {
    int siobrk(cchar *,int,const chrset &) noex ;
    int sirbrk(cchar *,int,const chrset &) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct brkinit {
        chrset  brks ;
        constexpr brkinit() noex {
            brks.set(CH_SLASH) ;
            brks.set(CH_DOT) ;
        } ; /* end ctor */
    } ; /* end struct (brkinit) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr brkinit       bi ;

cbool			f_sirchr = CF_SIRCHR ;


/* exported variables */


/* exported subroutines */

int sfext(cchar *sp,int µsl,cchar **rpp) noex {
	int		el = -1 ; /* return-value */
	if (sp && rpp) ylikely {
	    cchar	*ep = nullptr ;
	    if_constexpr (f_sirchr) {
		if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
		    while ((sl > 0) && (sp[sl - 1] == CH_SLASH)) {
		        sl -= 1 ;
		    }
		    if (int si ; (si = sirbrk(sp,sl,bi.brks)) >= 0) {
                        if (sp[si] == CH_DOT) {
                            si += 1 ;
		            el = (sl - si) ; /* <- signal FOUND */
		            ep = (sp + si) ;
                        } /* end if (got) */
                    } /* end if (hit something) */
		} /* end if (gelenstr) */
	    } else {
	        cchar	*bp ;
	        if (int bl ; (bl = sfbasename(sp,µsl,&bp)) > 0) {
		    if (int si ; (si = sirchr(bp,bl,CH_DOT)) > 0) {
			si += 1 ;
		        el = (bl - si) ; /* <- signal FOUND */
		        ep = (bp + si) ;
		    }
	        } /* end if (sibasename) */
	    } /* end if_constexpr (f_sirchr) */
	    *rpp = ep ;
	} /* end if (non-null) */
	return el ;
}
/* end subroutine (sfext) */


