/* ucenumpj */
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
#include	"ucenumpj.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMPJ_MAGIC ;

constexpr cchar		dfname[] = UCENUMPJ_FNAME ;


/* local subroutines */


/* exported subroutines */

int ucenumpj_open(ucenumpj *op,cchar *efname) noex {
	ucenumxxco<ucenumpj_ent>	ueo(op,mxx) ;
	if (efname == nullptr) efname = dfname ;
	return ueo.open(efname) ;
}
/* end if (ucenumpj_open) */

int ucenumpj_close(ucenumpj *op) noex {
	ucenumxxco<ucenumpj_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumpj_close) */

int ucenumpj_readent(ucenumpj *op,ucenumpj_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumpj_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumpj_readent) */

int ucenumpj_reset(ucenumpj *op) noex {
	ucenumxxco<ucenumpj_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumpj_reset) */


/* local subroutines */


