/* termescseq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Terminal Control Sequence (make them) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This object package is fchly finished!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	termescseq

	Description:
	This subroutine formulates a simple string that constitutes
	a terminal command sequence (of characters).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"termescseq.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int termescseq(char *dp,int dl,int fch,int a1,int a2,int a3,int a4) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (dl < 0) dl = INT_MAX ;
	if (dp) ylikely {
	    rs = SR_ILSEQ ;
	    if (fch > 0) ylikely {
	        rs = SR_OK ;
	        if (rs >= 0) ylikely {
	            rs = storebuf_chr(dp,dl,i,'\033') ;
	            i += rs ;
	        }
	        for (int ai = 1 ; (rs >= 0) && (ai <= 4) ; ai += 1) {
	            int		a = -1 ; /* superfluous! */
	            switch (ai) {
	            case 1: 
	                a = a1 ; 
	                break ;
	            case 2: 
	                a = a2 ; 
	                break ;
	            case 3: 
	                a = a3 ; 
	                break ;
	            case 4: 
	                a = a4 ; 
	                break ;
	            } /* end switch */
	            if (a >= 0) ylikely {
	                if ((a >= 0x20) && (a < 0x2f)) ylikely {
		            rs = storebuf_chr(dp,dl,i,a) ;
		            i += rs ;
	                } else {
		            rs = SR_ILSEQ ;
			}
	            } /* end if */
	        } /* end for */
	        if (rs >= 0) ylikely {
	            if ((fch >= 0x30) && (fch <= 0x7e)) ylikely {
	                rs = storebuf_chr(dp,dl,i,fch) ;
	                i += rs ;
	            } else {
		        rs = SR_ILSEQ ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (termescseq) */


