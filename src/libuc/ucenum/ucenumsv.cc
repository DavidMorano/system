/* ucenumsv */
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
#include	<limits.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumsv.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMSV_MAGIC ;

constexpr cchar		dfname[] = UCENUMSV_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumsv_open(ucenumsv *op,cchar *efname) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
}
/* end if (ucenumsv_open) */

int ucenumsv_close(ucenumsv *op) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumsv_close) */

int ucenumsv_readent(ucenumsv *op,ucenumsv_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumsv_readent) */

int ucenumsv_reset(ucenumsv *op) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumsv_reset) */


/* local subroutines */


