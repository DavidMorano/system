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

	These subroutines allocate a fixed amount of memory and
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


/* local namespaces */


/* local typedefs */

typedef char	*charp ;


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

char *mallocbuf(void *bp,int bl) noex {
	char		*rp = nullptr ;
	if (bp) {
	    if (bl < 0) bl = strlen(charp(bp)) ;
	    if (uc_malloc((bl+1),&rp) >= 0) {
		memcpy(rp,bp,bl) ;
	    }
	}
	return rp ;
}
/* end subroutine (mallocbuf) */

char *mallocstrw(char *sp,int sl) noex {
	char		*rp = nullptr ;
	if (sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (uc_malloc((sl+1),&rp) >= 0) {
	        strwcpy(rp,sp,sl) ;
	    }
	} /* end if */
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
	if (uc_malloc((len+1),&rp) >= 0) {
	    char	*bp = charp(&v) ;
	    memcpy(rp,bp,len) ;
	}
	return rp ;
}
/* end subroutine (mallocint) */


