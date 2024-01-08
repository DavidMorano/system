/* vecstr_foilcmp SUPPORT */
/* lang=C20 */

/* perform a foil comparison of two VECSTR lists */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Perform a foil comparison between two VECSTR lists.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecstr.h"


/* local variables */


/* exported subroutines */

int vecstr_foilcmp(vecstr *lnp,vecstr *rnp) noex {
	int	rs = SR_FAULT ;
	int	f_match = FALSE ;
	if (lnp && rnp) {
	    cchar	*cp ;
	    for (int i = 0 ; (rs = vecstr_get(lnp,i,&cp)) >= 0 ; i += 1) {
	        if (cp) {
	            cchar	*np ;
	            for (int j = 0 ; vecstr_get(rnp,j,&np) >= 0 ; j += 1) {
		        if (np) {
	        	    f_match = (cp[0] == np[0]) && (strcmp(cp,np) == 0) ;
	        	    if (f_match) break ;
		        }
	            } /* end for */
	        } /* end if (non-null) */
	        if (f_match) break ;
	    } /* end for */
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_match : rs ;
}
/* end subroutine (vecstr_foilcmp) */


