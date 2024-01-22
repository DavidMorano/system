/* ucenumho SUPPORT */
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
#include	"ucenumho.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMHO_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_ho ;


/* local subroutines */


/* exported subroutines */

int ucenumho_open(ucenumho *op,cchar *efname) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
	return rs ;
}
/* end if (ucenumho_open) */

int ucenumho_close(ucenumho *op) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumho_close) */

int ucenumho_readent(ucenumho *op,ucenumho_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumho_readent) */

int ucenumho_reset(ucenumho *op) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumho_reset) */


/* local subroutines */


