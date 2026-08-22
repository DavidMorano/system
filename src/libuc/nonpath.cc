/* nonpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if the given string represents a non-path filename */
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
	>0	type nonpath found
	0	regualr file-name
	<0	error (system-return)

	Notes:
	1. Non-paths are among the following:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bitset>		/* C++STD <- currently unused */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<six.h>			/* LIBUC |siochr(3uc)| */
#include	<ascii.h>		/* LIBU |CH_{xx}| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"nonpath.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/chrset.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */
import chrset ;				/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::bitset ;			/* type (currently unused) */


/* local typedefs */


/* external subroutines */

extern "C++" {
    int		siobrk(cchar *,int,con chrset *) noex ;
    int		sirbrk(cchar *,int,con chrset *) noex ;
} /* end extern (C++) */


/* external variables */

cchar		nonpathchs[nonpath_overlast + 1] = " ~¬¥§" ;


/* local structures */

namespace {
    constexpr int	chlen = (UCHAR_MAX + 1) ;
    struct nons {
	chrset		ss ;
	nons() noex {
	    for (int ch, i = 0 ; ((ch = nonpathchs[i])) ; i += 1) {
		ss.set(ch) ;
	    } /* end for */
	} ; /* end ctor (nons) */
    } ; /* end struct (nons) */
} /* end namespace */


/* forward references */


/* local variables */

static const nons	bs ;

constexpr int		chx_user1	= mkchar('~') ;
constexpr int		chx_user2	= mkchar('µ') ;
constexpr int		chx_var		= mkchar('¬') ;


/* exported variables */


/* exported subroutines */

int nonpath(cchar *fp,int µfl) noex {
	int		rs = SR_FAULT ;
	int		t = 0 ; /* return-value (type-of-nonpath) */
	if (fp) ylikely {
	    rs = SR_INVALID ;
	    if (int fl ; (fl = getlenstr(fp,µfl)) > 0) {
		rs = SR_OK ;
		if (cint ch = mkchar(fp[0]) ; ch != CH_SLASH) {
		    t = nonpath_user ;
		    if ((ch != chx_user1) && (ch != chx_user2)) {
			t = nonpath_var ;
			if (ch != chx_var) {
			    t = nonpath_reg ;
	                    if (int si ; (si = siobrk(fp,fl,&bs.ss)) >= 0) {
			        t = (si + nonpath_dialer) ;
			    }
			} /* end if */
	            } /* end if (nonpath_user) */
	        } /* end if (relative path) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? t : rs ;
} /* end subroutine (nonpath) */


