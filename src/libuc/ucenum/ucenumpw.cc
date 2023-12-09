/* ucenumpw */
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
#include	"ucenumpw.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMPW_MAGIC ;

constexpr cchar		dfname[] = UCENUMPW_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumpw_open(ucenumpw *op,cchar *efname) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
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


