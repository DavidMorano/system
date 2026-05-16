/* sysproject SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system project-entry enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A-D- Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sysproject

	Description:
	We enumerate (reentrantly and thread safely) PROJECT entries
	from the system PROJECT database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucentpj.h>
#include	<filemap.h>
#include	<localmisc.h>

#include	"sysproject.h"

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
local int sysproject_ctor(sysproject *op,Args ... args) noex {
    	SYSPROJECT	*hop = op ;
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
} /* end subroutine (sysproject_ctor) */

local int sysproject_dtor(sysproject *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->fmp) ylikely {
		delete op->fmp ;
		op->fmp = nullptr ;
	    } /* end if (momory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysproject_dtor) */

template<typename ... Args>
local inline int sysproject_magic(sysproject *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SYSPROJECT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (sysproject_magic) */


/* local variables */

constexpr cchar		defufname[] = SYSPROJECT_FNAME ;


/* exported variables */


/* exported subroutines */

int sysproject_open(sysproject *op,cchar *sufname) noex {
	csize		nmax = INT_MAX ;
	int		rs ;
	if (sufname == nullptr) sufname = defufname ; /* default */
	if ((rs = sysproject_ctor(op)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    if ((rs = fmp->open(sufname,nmax)) >= 0) ylikely {
	        op->magval = SYSPROJECT_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end if (sysproject_start) */

int sysproject_close(sysproject *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysproject_magic(op)) >= 0) ylikely {
	    if (op->fmp) ylikely {
		filemap		*fmp = op->fmp ;
		rs1 = fmp->close ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = sysproject_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (sysproject_close) */

int sysproject_readent(sysproject *op,ucentpj *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	if ((rs = sysproject_magic(op,pwp,pwbuf)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    for (cchar *lp ; (rs = fmp->getln(&lp)) > 0 ; ) {
	        rs = pwp->parse(pwbuf,pwlen,lp,rs) ;
	        if (rs) break ;
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysproject_readent) */

int sysproject_reset(sysproject *op) noex {
	int		rs ;
	if ((rs = sysproject_magic(op)) >= 0) ylikely {
	    filemap	*fmp = op->fmp ;
	    rs = fmp->rewind ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysproject_reset) */


