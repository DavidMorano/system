/* mallocstuff SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscellaneous |malloc(3c)| related */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-11, David A­D­ Morano
	These subroutines were was originally written (inspired by
	others).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Group:
	mallocstuff

	Description:
	These subroutines allocate a suitable amount of memory and
	then copy the user supplied thing into it.

****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>		/* |ulogeerror(3u)| */
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mallocstuff.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::mem ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *mallocbuf(void *bp,int µbl) noex {
	char		*rp = nullptr ;
	if (bp) {
	    int		rs ;
	    if (int bl ; (bl = getlenstr(charp(bp),µbl)) >= 0) {
	        if ((rs = mem.malloc((bl+1),&rp)) >= 0) {
		    memcopy(rp,bp,bl) ;
		    *rp = '\0' ;
	        } /* end if (memory-allocation) */
	        if (rs < 0) {
		    ulogerror("mallocbuf",rs,"uc_malloc") ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (mallocbuf) */

char *mallocstrw(char *sp,int µsl) noex {
	char		*rp = nullptr ;
	if (sp) {
	    int		rs ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if ((rs = mem.malloc((sl + 1),&rp)) >= 0) {
	            strwcpy(rp,sp,sl) ;
	        } /* end if (uc_malloc) */
	        if (rs < 0) {
		    ulogerror("mallocstrw",rs,"uc_malloc") ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (mallocstrw) */

char *mallocstr(cchar *sp) noex {
	return mallocstrw(sp,-1) ;
}
/* end subroutine (mallocstr) */

char *mallocint(int v) noex {
	int		rs ;
	cint		len = sizeof(int) ;
	char		*rp = nullptr ;
	if ((rs = mem.malloc((len + 1),&rp)) >= 0) {
	    char	*bp = charp(&v) ;
	    memcopy(rp,bp,len) ;
	    *rp = '\0' ;
	} /* end if (uc_malloc) */
	if (rs < 0) {
	    ulogerror("mallocint",rs,"uc_malloc") ;
	}
	return rp ;
}
/* end subroutine (mallocint) */


