/* mallocstuff SUPPORT */
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

	These subroutines allocate a suitable amount of memory and
	then copy the user supplied thing into it.

****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mallocstuff.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef char	*charp ;


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

char *mallocbuf(void *bp,int bl) noex {
	char		*rp = nullptr ;
	if (bp) {
	    int		rs ;
	    if (bl < 0) bl = strlen(charp(bp)) ;
	    if ((rs = uc_malloc((bl+1),&rp)) >= 0) {
		memcpy(rp,bp,bl) ;
		*rp = '\0' ;
	    } /* end if (uc_malloc) */
	    if (rs < 0) {
		ulogerror("mallocbuf",rs,"uc_malloc") ;
	    }
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (mallocbuf) */

char *mallocstrw(char *sp,int sl) noex {
	char		*rp = nullptr ;
	if (sp) {
	    int		rs ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = uc_malloc((sl+1),&rp)) >= 0) {
	        strwcpy(rp,sp,sl) ;
	    } /* end if (uc_malloc) */
	    if (rs < 0) {
		ulogerror("mallocstrw",rs,"uc_malloc") ;
	    }
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (mallocstrw) */

char *mallocstr(cchar *sp) noex {
	return mallocstrw(sp,-1) ;
}
/* end subroutine (mallocstr) */

char *mallocint(int v) noex {
	cint		len = sizeof(int) ;
	char		*rp = nullptr ;
	int		rs ;
	if ((rs = uc_malloc((len+1),&rp)) >= 0) {
	    char	*bp = charp(&v) ;
	    memcpy(rp,bp,len) ;
	    *rp = '\0' ;
	} /* end if (uc_malloc) */
	if (rs < 0) {
	    ulogerror("mallocint",rs,"uc_malloc") ;
	}
	return rp ;
}
/* end subroutine (mallocint) */


