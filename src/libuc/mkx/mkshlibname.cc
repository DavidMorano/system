/* mkshlibname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the filename for a shared library (shared object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-07-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkshlibname

	Description:
	This subroutine formulates (makes) the file-name for a
	shared library (shared object).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxnamelen(getbufsize_mn) ;


/* exported variables */


/* exported subroutines */

int mkshlibname(char *shlibname,cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (shlibname && pnp) ylikely {
	    rs = SR_INVALID ;
	    if (pnp[0]) ylikely {
	        cchar	*lc = "lib" ;
		if (pnl < 0) pnl = lenstr(pnp) ;
		if ((rs = maxnamelen) >= 0) ylikely {
		    storebuf	sb(shlibname,rs) ;
	            bool	f = ((pnl >= 3) && (strncmp(pnp,lc,3) == 0)) ;
	            if (! f) {
			rs = sb.strw(lc,3) ;
	            }
	            if (rs >= 0) ylikely {
	                rs = sb.strw(pnp,pnl) ;
	            }
	            if (rs >= 0) ylikely {
	                rs = sb.chr('.') ;
	            }
	            if (rs >= 0) ylikely {
	                rs = sb.strw("so",2) ;
	            }
		    rl = sb ;
		} /* end if (maxnamelen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkshlibname) */


