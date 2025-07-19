/* wsnwcpynarrow SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a narrow source string to a wide-string recipient */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsnwcpynarrow

	Description:
	We copy a narrow source string to a wide-string destination.

	Synopsis:
	int wsnwcpynarrow(wchar_t *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp	wide-string result buffer pointer
	dl	wide-string result buffer length
	sp	the source narrow-string that is to be copied
	sl	the maximum length of the source string to be copied

	Returns:
	>=0	number of characters copied
	<0	error; buffer overflow (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| + |wchar_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* system-return codes */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"wsnx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int wsnwcpynarrow(wchar_t *rarr,int rlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if (rarr && sp) {
	    rs = SR_OK ;
	    for (c = 0 ; (c < rlen) && (c < sl) && sp[c] ; c += 1) {
	        cint	ch = mkchar(sp[c]) ;
	        rarr[c] = ch ;
	    }
	    rarr[c] = '\0' ;
	    if ((c < sl) && (sp[c] != '\0')) rs = SR_OVERFLOW ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (wsnwcpynarrow) */


