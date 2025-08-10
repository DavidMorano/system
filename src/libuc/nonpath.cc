/* nonpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if the given string represent a non-path filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nonpath

	Description:
	I test a c-string to see if it is a floating or non-path
	filename.

	Synopsis:
	int nonpath(cchar *pp,int pl) noex

	Arguments:
	- pp	pointer to path string
	- pl	length of given path string

	Returns:
	true
	false

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<six.h>			/* |siochr(3uc)| */
#include	<ascii.h>		/* |CH_{xx}| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"nonpath.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/chrset.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */
import chrset ;				/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C++" {
    int		siobrk(cchar *,int,const chrset &) noex ;
    int		sirbrk(cchar *,int,const chrset &) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct nons {
	cchar		*nonpaths = "¥§~" ;
	chrset		ss ;
	constexpr nons() noex {
	    for (int ch, i = 0 ; ((ch = nonpaths[i])) ; i += 1) {
		ss.set(ch) ;
	    } /* end for */
	} ; /* end ctor (nons) */
    } ; /* end struct (nons) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr nons		bs ;

cint			chx_user1 = mkchar('~') ;
cint			chx_user2 = mkchar('µ') ;
cint			chx_cd = mkchar('¬') ;


/* exported variables */


/* exported subroutines */

int nonpath(cchar *fp,int µfl) noex {
	int		rs = SR_FAULT ;
	int		t = 0 ; /* return-value (type-of-nonpath) */
	if (fp) ylikely {
	    rs = SR_INVALID ;
	    if (int fl ; (fl = getlenstr(fp,µfl)) > 0) {
		cint ch = mkchar(fp[0]) ;
		rs = SR_OK ;
	        if (ch != CH_SLASH) {
		    t = nonpath_user ;
		    if ((ch != chx_user1) && (ch != chx_user2)) {
			t = nonpath_cd ;
			if (ch != chx_cd) {
			    t = nonpath_reg ;
	                    if (int si ; (si = siobrk(fp,fl,bs.ss)) >= 0) {
			        t = (si + nonpath_dialer) ;
			    }
			}
	            } /* end if (nonpath_user) */
	        } /* end if (absolute path) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? t : rs ;
}
/* end subroutine (nonpath) */


