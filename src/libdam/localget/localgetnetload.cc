/* localgetnetload SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the LOCAL network-load (NETLOAD) */
/* version %I% last-modified %G% */

#define	CF_UCPROGDATA	1		/* use |ucprogdata_xxx(3uc)| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetnetload

	Description:
	This subroutine retrieves the LOCAL system network-load.

	Synopsis:
	int localgetnetload(cchar *pr,char *rbuf,int rlen) noex

	Arguments:
	pr		program root
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer

	Returns:
	>=0		length of returned value
	<0		error (system-return)

	Notes:

	Q. Why the program-cache?
	A. Because this subroutine, and a couple others like it,
	get called everytime certain pseudo-"files" are read out.
	We want some of those files to read out very quickly, so
	caching away an extra real-file read in this routine (and
	others like it) really speeds things up.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucprogdata.h>		/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"localget.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |sysword(3u)| */

/* local defines */

#define	NETLOADFNAME	"netload"
#define	TO_TTL		(5*60)

#ifndef	CF_UCPROGDATA
#define	CF_UCPROGDATA	1
#endif


/* external subroutines */


/* external variables */


/* local structures */

enum netloadcos {
	netloaderco_start,
	netloaderco_finish,
	netloaderco_overlast
} ; /* end enum */

namespace {
    struct netloader ;
    struct netloader_co {
	netloader	*op{} ;
	int		w = -1 ;
	void operator () (netloader *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
        operator int () noex ;
    } ; /* end struct (netloader_co) */
    struct netloader {
	cchar		*pr ;
	char		*rbuf ;
	int		rlen ;
	netloader_co	start ;
	netloader_co	finish ;
	netloader(cc *p,char *rb,int rl) noex : pr(p), rbuf(rb), rlen(rl) {
	    start	(this,netloaderco_start) ;
	    finish	(this,netloaderco_finish) ;
	} ;
	operator int () noex ;
	int env() noex ;
	int cache() noex ;
	int localconf() noex ;
	int istart() noex ;
	int ifinish() noex ;
	destruct netloader() {
	    (void) ifinish() ;
	} ;
    } ; /* end struct (netloader) */
} /* end namespace */

typedef int (netloader::*netloader_m)() noex ;


/* forward references */


/* local variables */

constexpr int		di = UCPROGDATA_DNETLOAD ;
constexpr int		ttl = TO_TTL ;
constexpr char		nlname[] = NETLOADFNAME ;
constexpr cchar		*vardname = sysword.w_vardir ;
constexpr bool		f_ucprogdata = CF_UCPROGDATA ;

constexpr netloader_m	msubs[] = {
	&netloader::env,
	&netloader::cache,
	&netloader::localconf,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int localgetnetload(cchar *pr,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
		if (netloader no(pr,rbuf,rlen) ; (rs = no.start) >= 0) {
		    {
			rs = no ;
			len = rs ;
		    }
		    rs1 = no.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (netloader) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (localgetnetload) */

/* user environment */
int netloader::env() noex {
    	int		rs = SR_OK ;
	int		len = 0 ;
	static cchar	*netload = getenv(varname.netload) ;
	if (netload && (netload[0] != '\0')) {
	    rs = sncpy1(rbuf,rlen,netload) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
} /* end method (netloader::env) */

int netloader::cache() noex {
    	int		rs = SR_OK ;
	int		len = 0 ;
	if_constexpr (f_ucprogdata) {
	    if ((rs = ucprogdata_get(di,rbuf,rlen)) > 0) {
		len = rs ;
	    }
	} /* end if_constexpr (f_ucprogdata) */
	return (rs >= 0) ? len : rs ;
} /* end method (netloader::cache) */

/* software facility (LOCAL) configuration */
int netloader::localconf() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
        if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
            if ((rs = mkpath(tbuf,pr,vardname,nlname)) >= 0) ylikely {
                if ((rs = filereadln(tbuf,rbuf,rlen)) > 0) ylikely {
                    len = rs ;
                    if_constexpr (f_ucprogdata) {
                        rs = ucprogdata_set(di,rbuf,len,ttl) ;
                    }
                } else if (isNotPresent(rs)) {
                    rs = SR_OK ;
                }
            } /* end if (mkpath) */
            rs1 = lm_free(tbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (netloader::localconf) */

int netloader::istart() noex {
	return SR_OK ;
}

int netloader::ifinish() noex {
	return SR_OK ;
}

netloader::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && msubs[i] ; i += 1) {
	    netloader_m		m = msubs[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
} /* end method */

netloader_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case netloaderco_start:
		rs = op->istart() ;
		break ;
	    case netloaderco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	}
	return rs ;
} /* end method (netloader_co::operator) */


