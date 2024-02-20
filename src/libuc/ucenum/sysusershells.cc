/* sysusershells SUPPORT */
/* lang=C++20 */

/* system user-shell enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user-shell
	entries from the system user-shell database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getax.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"sysusershells.h"


/* local defines */


/* local namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* exported variables */


/* forward references */

static inline int sysusershells_ctor(sysusershells *op) noex {
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
/* end subroutine sysusershells_ctor) */

static inline int sysusershells_dtor(sysusershells *op) noex {
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
/* end subroutine sysusershells_dtor) */

template<typename ... Args>
static int sysusershells_magic(sysusershells *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSUSERSHELLS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysusershells_magic) */


/* local variables */


/* exported subroutines */

int SYSUSERSHELLS_open(sysusershells *op,cchar *sufname) noex {
	csize		max = INT_MAX ;
	int		rs ;
	cchar		*defufname = SYSUSERSHELLS_FNAME ;
	if ((rs = sysusershells_ctor(op)) >= 0) {
	    if (sufname == NULL) sufname = defufname ; /* default */
	    if ((rs = filemap_open(op->fmp,sufname,O_RDONLY,max)) >= 0) {
	        op->magic = SYSUSERSHELLS_MAGIC ;
	    }
	    if (rs < 0) {
	        sysusershells_dtor(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end if (SYSUSERSHELLS_start) */

int sysusershells_close(sysusershells *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysusershells_magic(op)) >= 0) {
	    {
	        rs1 = filemap_close(op->fmp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysusershells_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (sysusershells_close) */

int SYSUSERSHELLS_readent(sysusershells *op,char *ubuf,int ulen) noex {
	int		rs ;
	if ((rs = sysusershells_magic(op,ubuf)) >= 0) {
	    int		ll ;
	    cchar	*lp ;
	    ubuf[0] = '\0' ;
	    while ((rs = filemap_getline(op->fmp,&lp)) > 0) {
	        ll = rs ;
	        if (lp[ll-1] == '\n') ll -= 1 ;
	        rs = snwcpy(ubuf,ulen,lp,ll) ;
	        if (rs > 0) break ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (SYSUSERSHELLS_readent) */

int SYSUSERSHELLS_reset(sysusershells *op) noex {
	int		rs ;
	if ((rs = sysusershells_magic(op)) >= 0) {
	    rs = filemap_rewind(op->fmp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (SYSUSERSHELLS_reset) */


