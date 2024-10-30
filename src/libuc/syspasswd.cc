/* syspasswd SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* system user-entry enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A-D- Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	syspasswd

	Description:
	We enumerate (reentrantly and thread safely) PASSWD entries
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<filemap.h>
#include	<ucentpw.h>
#include	<localmisc.h>

#include	"syspasswd.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int syspasswd_ctor(syspasswd *op,Args ... args) noex {
    	SYSPASSWD	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->fmp = new(nothrow) filemap) != np) {
		rs = SR_OK ;
	    } /* end if (new-filemap) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (syspasswd_ctor) */

static int syspasswd_dtor(syspasswd *op) noex {
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
/* end subroutine (syspasswd_dtor) */

template<typename ... Args>
static inline int syspasswd_magic(syspasswd *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSPASSWD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (syspasswd_magic) */


/* local variables */

constexpr cchar		defufname[] = SYSPASSWD_FNAME ;


/* exported variables */


/* exported subroutines */

int syspasswd_open(syspasswd *op,cchar *sufname) noex {
	csize		nmax = INT_MAX ;
	int		rs ;
	if (sufname == NULL) sufname = defufname ; /* default */
	if ((rs = syspasswd_ctor(op)) >= 0) {
	    filemap	*fmp = op->fmp ;
	    if ((rs = fmp->open(sufname,nmax)) >= 0) {
	        op->magic = SYSPASSWD_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end if (syspasswd_start) */

int syspasswd_close(syspasswd *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = syspasswd_magic(op)) >= 0) {
	    if (op->fmp) {
		filemap		*fmp = op->fmp ;
		rs1 = fmp->close ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = syspasswd_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (syspasswd_close) */

int syspasswd_readent(syspasswd *op,ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	if ((rs = syspasswd_magic(op,pwp,pwbuf)) >= 0) {
	    filemap	*fmp = op->fmp ;
	    cchar	*lp{} ;
	    while ((rs = fmp->getln(&lp)) > 0) {
	        rs = pwp->parse(pwbuf,pwlen,lp,rs) ;
	        if (rs > 0) break ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syspasswd_readent) */

int syspasswd_reset(syspasswd *op) noex {
	int		rs ;
	if ((rs = syspasswd_magic(op)) >= 0) {
	    filemap	*fmp = op->fmp ;
	    rs = fmp->rewind ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syspasswd_reset) */


