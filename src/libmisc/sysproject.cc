/* sysproject SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysdbfn.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"sysproject.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = SYSPROJECT_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_pj ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int sysproject_open(sysproject *op,cchar *efname) noex {
	ucenumxxco<sysproject_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp{} ; (rs = sysdbfn_get(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfn_get) */
	return rs ;
}
/* end if (sysproject_open) */

int sysproject_close(sysproject *op) noex {
	ucenumxxco<sysproject_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (sysproject_close) */

int sysproject_readent(sysproject *op,sysproject_ent *ep,char *bp,int bl) noex {
	ucenumxxco<sysproject_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (sysproject_readent) */

int sysproject_reset(sysproject *op) noex {
	ucenumxxco<sysproject_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (sysproject_reset) */


/* local subroutines */


