/* syspasswd SUPPORT */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucentpw.h>
#include	<filemap.h>
#include	<localmisc.h>

#include	"syspasswd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int syspasswd_ctor(syspasswd *op,Args ... args) noex {
    	SYSPASSWD	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->fmp = new(nt) filemap) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-filemap) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (syspasswd_ctor) */

local int syspasswd_dtor(syspasswd *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->fmp) ylikely {
		delete op->fmp ;
		op->fmp = nullptr ;
	    } /* end if (momory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (syspasswd_dtor) */

template<typename ... Args>
local inline int syspasswd_magic(syspasswd *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SYSPASSWD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (syspasswd_magic) */


/* local variables */

constexpr cchar		defufname[] = SYSPASSWD_FNAME ;


/* exported variables */


/* exported subroutines */

int syspasswd_open(syspasswd *op,cchar *sufname) noex {
	csize		nmax = INT_MAX ;
	int		rs ;
	if (sufname == nullptr) sufname = defufname ; /* default */
	if ((rs = syspasswd_ctor(op)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    if ((rs = fmp->open(sufname,nmax)) >= 0) ylikely {
	        op->magval = SYSPASSWD_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end if (syspasswd_start) */

int syspasswd_close(syspasswd *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = syspasswd_magic(op)) >= 0) ylikely {
	    if (op->fmp) ylikely {
		filemap		*fmp = op->fmp ;
		rs1 = fmp->close ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = syspasswd_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (syspasswd_close) */

int syspasswd_readent(syspasswd *op,ucentpw *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	if ((rs = syspasswd_magic(op,pwp,pwbuf)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    for (cchar *lp ; (rs = fmp->getln(&lp)) > 0 ; ) {
	        rs = pwp->parse(pwbuf,pwlen,lp,rs) ;
	        if (rs) break ;
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syspasswd_readent) */

int syspasswd_reset(syspasswd *op) noex {
	int		rs ;
	if ((rs = syspasswd_magic(op)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    rs = fmp->rewind ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syspasswd_reset) */


