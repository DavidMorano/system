/* ucenumpw SUPPORT */
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysdbfname.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumpw.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMPW_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_pw ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumpw_open(ucenumpw *op,cchar *efname) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
	return rs ;
}
/* end if (ucenumpw_open) */

int ucenumpw_close(ucenumpw *op) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumpw_close) */

int ucenumpw_readent(ucenumpw *op,ucenumpw_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumpw_readent) */

int ucenumpw_reset(ucenumpw *op) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumpw_reset) */


/* local subroutines */


