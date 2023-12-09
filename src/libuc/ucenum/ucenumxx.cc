/* ucenumxx */
/* lang=C++20 */

/* enumerate system XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user names
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"ucenumxx.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int ucenumxxbase::open(cchar *efname) noex {
	int		rs ;
	    cint	of = O_RDONLY ;
	    csize	max = INT_MAX ;
	    memclear(op,sizeof(ucenumxx)) ;
	    if ((rs = filemap_open(&b,efname,of,max)) >= 0) {
	        magic = mxx ;
	    }
	return rs ;
}
/* end if (ucenumxxbase::open) */

int ucenumxxbase::close() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	    if (op->magic == mxx) {
		rs1 = filemap_close(&op->b) ;
		if (rs >= 0) rs = rs1 ;
		op->magic = 0 ;
	    } /* end if (open) */
	return rs ;
} 
/* end subroutine (ucenumxxbase::close) */

int ucenumxxbase::reset() noex {
	int		rs = SR_NOTOPEN ;
	    if (op->magic == mxx) {
		rs = filemap_rewind(&op->b) ;
	    } /* end if (open) */
	return rs ;
}
/* end subroutine (ucenumxxbase::reset) */


