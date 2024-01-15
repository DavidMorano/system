/* sysusernames SUPPORT */
/* lang=C++20 */

/* system user-name enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user-name
	entries from the system USER-NAME database.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<getax.h>
#include	<localmisc.h>
#include	<snwcpy.h>

#include	"sysusernames.h"


/* local defines */


/* local namespaces */

using std::nothrow ;


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

static inline int sysusernames_ctor(sysusernames *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->fmp = new(nothrow) filemap) != nullptr) {
		rs = SR_OK ;
	    } /* end if (new-filemap) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine sysusernames_ctor) */

static inline int sysusernames_dtor(sysusernames *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fmp) {
		delete op->fmp ;
		op->fmp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine sysusernames_dtor) */

template<typename ... Args>
static int sysusernames_magic(sysusernames *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSUSERNAMES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysusernames_magic) */


/* local variables */

constexpr cchar		*defufname = SYSUSERNAMES_FNAME ;


/* exported variables */


/* exported subroutines */

int sysusernames_open(sysusernames *op,cchar *sufname) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    csize	max = INT_MAX ;
	    cint	of = O_RDONLY ;
	    if (sufname == nullptr) sufname = defufname ;
	    if ((rs = filemap_open(op->fmp,sufname,of,max)) >= 0) {
	    	op->magic = SYSUSERNAMES_MAGIC ;
	    }
	    if (rs < 0) {
		sysusernames_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end if (sysusernames_start) */

int sysusernames_close(sysusernames *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysusernames_magic(op)) >= 0) {
	    {
		rs1 = filemap_close(op->fmp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = sysusernames_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (sysusernames_close) */

int sysusernames_readent(sysusernames *op,char *ubuf,int ulen) noex {
	int		rs ;
	if ((rs = sysusernames_magic(op,ubuf)) >= 0) {
	    cchar	*lp ;
	    ubuf[0] = '\0' ;
	    while ((rs = filemap_getline(op->fmp,&lp)) > 0) {
	        int		ll = rs ;
	        if (lp[ll-1] == '\n') ll -= 1 ;
	        rs = snwcpy(ubuf,ulen,lp,ll) ;
	        if (rs > 0) break ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusernames_readent) */

int sysusernames_reset(sysusernames *op) noex {
	int		rs ;
	if ((rs = sysusernames_magic(op)) >= 0) {
	    rs = filemap_rewind(op->fmp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusernames_reset) */


