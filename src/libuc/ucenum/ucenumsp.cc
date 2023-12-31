/* ucenumsp */
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
#include	"ucenumsp.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMSP_MAGIC ;

constexpr cchar		dfname[] = UCENUMSP_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumsp_open(ucenumsp *op,cchar *efname) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
}
/* end if (ucenumsp_open) */

int ucenumsp_close(ucenumsp *op) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumsp_close) */

int ucenumsp_readent(ucenumsp *op,ucenumsp_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumsp_readent) */

int ucenumsp_reset(ucenumsp *op) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumsp_reset) */


/* local subroutines */


