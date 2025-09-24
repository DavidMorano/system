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

	We enumerate (reentrantly and thread safely) user names
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"sysusers.h"


/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */
#undef	COMMENT


/* local namesapces */

using std::nothrow ;			/* constant */


/* local typdefs */


/* external subroutines */


/* local structures */


/* forward references */

static inline int sysusers_ctor(sysusers *op) noex {
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
/* end subroutine sysusers_ctor) */

static inline int sysusers_dtor(sysusers *op) noex {
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
/* end subroutine sysusers_dtor) */

template<typename ... Args>
static int sysusers_magic(sysusers *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSUSERS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysusers_magic) */


/* local variables */

constexpr cchar		*defufname = SYSUSERS_FNAME ;


/* exported variables */


/* exported subroutines */

int sysusers_open(sysusers *op,cchar *sufname) noex {
	int		rs ;
	if ((rs = sysusers_ctor(op)) >= 0) {
	    csize	nmax = INT_MAX ;
	    if (sufname == nullptr) sufname = defufname ;
	    if ((rs = filemap_open(op->fmp,sufname,nmax)) >= 0) {
	        op->magic = SYSUSERS_MAGIC ;
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
	if ((rs = sysusers_magic(op)) >= 0) {
	    {
	        rs1 = filemap_close(op->fmp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysusers_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (sysusers_close) */

int sysusers_readent(sysusers *op,PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysusers_magic(op,pwp,pwbuf)) >= 0) {
            if ((rs = getbufsize(getbufsize_un)) >= 0) {
                cint        ulen = rs ;
                char        *ubuf ;
                if ((rs = lm_mall((ulen+1),&ubuf)) >= 0) {
                    cchar   *lp{} ;
                    while ((rs = filemap_getln(op->fmp,&lp)) > 0) {
                        int         ll = rs ;
                        if (lp[ll-1] == '\n') ll -= 1 ;
                        if ((rs = (strdcpy1w(ubuf,ulen,lp,ll)-ubuf)) > 0) {
                            rs = GETPW_NAME(pwp,pwbuf,pwlen,ubuf) ;
                            if (rs == SR_NOTFOUND) rs = SR_OK ;
                        }
                        if (rs > 0) break ;
                        if (rs < 0) break ;
                    } /* end while */
                    rs1 = lm_free(ubuf) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (m-a-f) */
            } /* end if (getbufsize) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusers_readent) */

int sysusers_reset(sysusers *op) noex {
	int		rs ;
	if ((rs = sysusers_magic(op)) >= 0) {
	    rs = filemap_rewind(op->fmp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysusers_reset) */


