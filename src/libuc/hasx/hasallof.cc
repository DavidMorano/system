/* hasallof SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* does the given string have all of the given characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	hasallof

	Description:
	Determine if the given string has all of the specified
	characters within it.

	Synopsis:
	int hasallof(cchar *sp,int sl,cchar *tstr)

	Arguments:
	sp		source string
	sl		length of source string
	tstr		string of characters to test against

	Returns:
	>=0		yes, the given c-string has all the characters
	==0		failed, did not have all characters specified
	>0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<six.h>			/* |sichr(3uc)| */
#include	<dupstr.h>
#include	<localmisc.h>

#include	"hasallof.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hasallof(cchar *sp,int sl,cchar *tstr) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = true ;
	if (sp && tstr) {
	    rs = SR_INVALID ;
	    if (tstr[0]) {
	        char	*bp ;
	        f = false ;
	        if (dupstr sd ; (rs = dupstr_start(&sd,tstr,-1,&bp)) >= 0) {
	            int	bl = rs ;
	            while (sl && *sp) {
	                if (int si ; (si = sichr(bp,bl,sp[0])) >= 0) {
			    if (bl-- > 1) {
			        if (si < bl) bp[si] = bp[bl] ;
			    }
		            f = (bl == 0) ;
	                    if (f) break ;
		        }
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end while */
	            rs1 = dupstr_finish(&sd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (dupstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (hasallof) */


