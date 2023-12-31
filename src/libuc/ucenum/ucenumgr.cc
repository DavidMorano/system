/* ucenumgr */
/* lang=C++20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A�D� Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumgr.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMGR_MAGIC ;

constexpr cchar		dfname[] = UCENUMGR_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumgr_open(ucenumgr *op,cchar *efname) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
}
/* end if (ucenumgr_open) */

int ucenumgr_close(ucenumgr *op) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumgr_close) */

int ucenumgr_readent(ucenumgr *op,ucenumgr_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumgr_readent) */

int ucenumgr_reset(ucenumgr *op) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumgr_reset) */


/* local subroutines */


