/* vecstrx_loadpjusers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find and load UNIX® users who have the given project name */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_loadpjusers

	Description:
	This subroutine finds all users who have the given specified
	project.

	Synopsis:
	int vecstrx_loadpjusers(vecstrx *,cchar *) noex

	Returns:
	>=	ok
	<0	bad (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucenumua.h>
#include	<strwcmp.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"vecstrx.hh"


/* local defines */

#ifndef	USERATTRLEN
#define	USERATTRLEN		(4 * 1024)
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef ucentua		ent ;


/* external subroutines */


/* local structures */


/* forward references */

static int	vecstrx_loadpjnent(vecstrx *,ent *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstrx::loadpjusers(cchar *pjn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (pjn) {
	    rs = SR_INVALID ;
	    if (pjn[0]) {
		cint ualen = USERATTRLEN ;
		if (char *uab ; (rs = libmem.mall(ualen,&uab)) >= 0) {
	            cint	ual = rs ;
	            if (ucenumua eua ; (rs = eua.open) >= 0) {
	                ucentua		ua{} ;
	                while ((rs = eua.readent(&ua,uab,ual)) > 0) {
	                    rs = vecstrx_loadpjnent(this,&ua,pjn) ;
	                    c += rs ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = eua.close ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (sysuserattr) */
	            rs1 = libmem.free(uab) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadpjusers) */


/* local subroutines */

static int vecstrx_loadpjnent(vecstrx *vsp,ent *uap,cchar *pjn) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*k = "project" ;
	if (cchar *vp{} ; (rs = uc_kvamatch(uap->attr,k,&vp)) >= 0) {
	    if (strwcmp(pjn,vp,rs) == 0) {
	        rs = vsp->adduniq(uap->name) ;
	        if (rs < INT_MAX) c += 1 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadpjnent) */


