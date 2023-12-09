/* ucenumua */
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
#include	"ucenumua.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMUA_MAGIC ;

constexpr cchar		dfname[] = UCENUMUA_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumua_open(ucenumua *op,cchar *efname) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
}
/* end if (ucenumua_open) */

int ucenumua_close(ucenumua *op) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumua_close) */

int ucenumua_readent(ucenumua *op,ucenumua_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumua_readent) */

int ucenumua_reset(ucenumua *op) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumua_reset) */


/* local subroutines */


