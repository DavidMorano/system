/* sysusers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system user-entry enumeration */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sysusers

	Description:
	We enumerate (reentrantly and thread safely) user names
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ucentpw.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getax.h>
#include	<getpwx.h>
#include	<rmx.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"sysusers.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namesapces */

using libuc::libmem ;			/* variable */


/* local typdefs */


/* external subroutines */


/* local structures */


/* forward references */

local inline int sysusers_ctor(sysusers *op) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    if ((op->fmp = new(nt) filemap) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-filemap) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine sysusers_ctor) */

local inline int sysusers_dtor(sysusers *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->fmp) ylikely {
		delete op->fmp ;
		op->fmp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine sysusers_dtor) */

template<typename ... Args>
local int sysusers_magic(sysusers *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == SYSUSERS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (sysusers_magic) */


/* local variables */

constexpr char		defufname[] = SYSUSERS_FNAME ;


/* exported variables */


/* exported subroutines */

int sysusers_open(sysusers *op,cchar *sufname) noex {
	int		rs ;
	if ((rs = sysusers_ctor(op)) >= 0) ylikely {
	    csize	nmax = INT_MAX ;
	    if (sufname == nullptr) sufname = defufname ;
	    if ((rs = filemap_open(op->fmp,sufname,nmax)) >= 0) ylikely {
	        op->magval = SYSUSERS_MAGIC ;
	    }
	    if (rs < 0) {
		sysusers_dtor(op) ;
	    }
	} /* end if (ctor) */
	return rs ;
}
/* end if (sysusers_start) */

int sysusers_close(sysusers *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysusers_magic(op)) >= 0) ylikely {
	    {
	        rs1 = filemap_close(op->fmp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysusers_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (sysusers_close) */

int sysusers_readent(sysusers *op,PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysusers_magic(op,pwp,pwbuf)) >= 0) ylikely {
	    ucentpw *entp = resumelife<ucentpw>(pwp) ;
            if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) ylikely {
                cint        ulen = rs ;
                for (cc *lp ; (rs = filemap_getln(op->fmp,&lp)) > 0 ; ) {
                    if (cint ll = rmeol(lp,rs) ; ll > 0) {
                        rs = intconv(strdcpy1w(ubuf,ulen,lp,ll) - ubuf) ;
                        if (rs > 0) {
                            rs = getpwx_name(entp,pwbuf,pwlen,ubuf) ;
                            if (rs == SR_NOTFOUND) rs = SR_OK ;
                        }
		    } /* end if (non-zero positive) */
                    if (rs) break ;
                } /* end for */
                rs1 = lm_free(ubuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusers_readent) */

int sysusers_reset(sysusers *op) noex {
	int		rs ;
	if ((rs = sysusers_magic(op)) >= 0) ylikely {
	    rs = filemap_rewind(op->fmp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusers_reset) */


