/* mksoname SUPPORT */
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
	mksoname

	Description:
	This subroutine formulates (makes) the file-name for a
	shared library (shared object).

	Synopsis:
	int mksoname(char *rbuf,cchar *pnp,int pnl) noex

	Arguments:
	rbuf		result buffer
	pnp		source pointer
	pnl		source length

	Returns:
	>=0		result buffer length
	<0		error (system-return)

	See-also:
	mksofname
	mksoname
	mkshlibname

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mksoname.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxnamelen(bufsize_mn) ;

cchar			liba[] = "lib" ;

cint			libl = clenstr(liba) ;


/* exported variables */


/* exported subroutines */

int mksoname(char *shlibname,cchar *pnp,int µpnl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (shlibname && pnp) ylikely {
	    rs = SR_INVALID ;
	    if (pnp[0]) ylikely {
		if (int pnl ; (pnl = getlenstr(pnp,µpnl)) > 0) ylikely {
		    if ((rs = maxnamelen) >= 0) ylikely {
		        storebuf sb(shlibname,rs) ;
	                bool f = (pnl >= 3) ;
	                f = f && (strncmp(pnp,liba,libl) == 0) ;
	                if (! f) {
			    rs = sb.strw(liba,libl) ;
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
		} /* end if (getlenstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mksoname) */


