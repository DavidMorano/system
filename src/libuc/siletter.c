/* siletter */
/* lang=C20 */

/* is the ... something ... a "leader" (whatever that is)? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We check for some condition.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"
#include	"siletter.h"


/* local defines */


/* external subroutines */

extern int	sfshrink(cchar *,int,cchar **) noex ;
extern int	hasalluc(cchar *,int) noex ;

extern char	*strnchr(cchar *,int,int) noex ;


/* exported subroutines */

int siletter(SILETTER *lsp,cchar *sp,int sl) noex {
	int		si = 0 ;
	memset(lsp,0,sizeof(SILETTER)) ;
	if (sl > 0) {
	    int		cl ;
	    cchar	*tp, *cp ;
	    if ((tp = strnchr(sp,sl,'.')) != NULL) {
	        if ((cl = sfshrink(sp,(tp - sp),&cp)) > 0) {
	            if (hasalluc(cp,cl)) {
	                lsp->lp = (char *) cp ;
	                lsp->ll = cl ;
	                si = ((tp + 1) - sp) ;
		    }
	        } /* end if (shrink) */
	    } /* end if (had a period) */
	} /* end if (positive) */
	return si ;
}
/* end subroutine (siletter) */


