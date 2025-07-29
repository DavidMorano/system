/* mkunique SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* test whether a string consists of all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkunique

	Description:
	Create a new c-string (in the given result buffer) that
	duplicates the source c-string but with all duplicated
	character removed.

	Synopsis:
	int mkunique(char *rbuf,int rlen,cchar *sp,int sl) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	sp		string to test
	sl		length of string to test

	Returns:
	>=0		length of given string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* |bitset(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strmgr.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkunique.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

using std::nullptr_t ;			/* type */
using std::bitset ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		nchars = (UCHAR_MAX + 1) ;


/* exported variables */


/* exported subroutines */

int mkunique(char *rbuf,int rlen,cchar *sp,int bl) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        if (bl < 0) bl = lenstr(sp) ;
	        rbuf[0] = '\0' ;
	        if (bl > 0) ylikely {
		    if (strmgr m ; (rs = m.start(rbuf,rlen)) >= 0) {
	                if (bl > 1) ylikely {
	                    bitset<nchars>	seen ;
	                    while ((rs >= 0) && bl-- && *sp) {
	                        cint	ch = mkchar(*sp++) ;
		                if (! seen[ch]) {
		                    rs = m.chr(ch) ;
	                            seen[ch] = true ;
		                }
	                    } /* end while */
	                } else {
			    rs = m.chr(*sp++) ;
	                } /* end if */
		        rl = m.finish ;
		        if (rs >= 0) rs = rl ;
	            } /* end if (strmgr) */
	        } /* end if (non-zero positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkunique) */


