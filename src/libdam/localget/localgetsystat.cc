/* localgetsystat SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the LOCAL system-status (SYSTAT) */
/* version %I% last-modified %G% */

#define	CF_UCPROGDATA	1		/* use |ucprogdata_xxx(3uc)| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetsystat

	Description:
	This subroutine retrieves the LOCAL system-status (SYSTAT).

	Synopsis:
	int localgetsystat(cchar *pr,char *rbuf,int rlen) noex

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<ucprogdata.h>
#include	<mallocxx.h>
#include	<filereadln.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"


import uconstants ;

/* local defines */

#define	SYSTATFNAME	"systat"
#define	TO_TTL		(5*60)

#ifndef	CF_UCPROGDATA
#define	CF_UCPROGDATA	1
#endif


/* external subroutines */


/* external variables */


/* local structures */

enum orgloccos {
	systaterco_start,
	systaterco_finish,
	systaterco_overlast
} ;

namespace {
    struct systater ;
    struct systater_co {
	systater	*op{} ;
	int		w = -1 ;
	void operator () (systater *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
        operator int () noex ;
    } ; /* end struct (systater_co) */
    struct systater {
	cchar		*pr ;
	char		*rbuf ;
	int		rlen ;
	systater_co	start ;
	systater_co	finish ;
	systater(cc *p,char *rb,int rl) noex : pr(p), rbuf(rb), rlen(rl) {
	    start(this,systaterco_start) ;
	    finish(this,systaterco_finish) ;
	} ;
	operator int () noex ;
	int env() noex ;
	int cache() noex ;
	int localconf() noex ;
	int istart() noex ;
	int ifinish() noex ;
	~systater() {
	    (void) ifinish() ;
	} ;
    } ; /* end struct (systater) */
}

typedef int (systater::*systater_m)() noex ;


/* forward references */


/* local variables */

constexpr int		di = UCPROGDATA_DSYSTAT ;
constexpr int		ttl = TO_TTL ;

constexpr bool		f_ucprogdata = CF_UCPROGDATA ;

constexpr char		ssn[] = SYSTATFNAME ;

constexpr cchar		*vardname = sysword.w_vardir ;

constexpr systater_m	mems[] = {
	&systater::env,
	&systater::cache,
	&systater::localconf,
	nullptr
} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int localgetsystat(cchar *pr,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
		if (systater st(pr,rbuf,rlen) ; (rs = st.start) >= 0) {
		    {
		        rs = st ;
		        len = rs ;
		    }
		    rs1 = st.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (systater) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetsystat) */

/* user environment */
int systater::env() noex {
	static cchar	*systat = getenv(varname.systat) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (systat && (systat[0] != '\0')) {
	    rs = sncpy1(rbuf,rlen,systat) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
} 
/* end method (systater::env) */

/* program cache */
int systater::cache() noex {
    	int		rs = SR_OK ;
	int		len = 0 ;
	if_constexpr (f_ucprogdata) {
	    if ((rs = ucprogdata_get(di,rbuf,rlen)) > 0) {
	        len = rs ;
	    }
	} /* end if_constexpr (f_ucprogdata) */
	return (rs >= 0) ? len : rs ;
}
/* end method (systater::cache) */

/* software facility (LOCAL) configuration */
int systater::localconf() noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
        if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
            if ((rs = mkpath(tbuf,pr,vardname,ssn)) >= 0) {
                if ((rs = filereadln(tbuf,rbuf,rlen)) > 0) {
                    len = rs ;
                    if_constexpr (f_ucprogdata) {
                        rs = ucprogdata_set(di,rbuf,len,ttl) ;
                    }
                } else if (isNotPresent(rs)) {
                    rs = SR_OK ;
                }
            } /* end if (mkpath) */
            rs1 = uc_free(tbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end method (systater::localconf) */

int systater::istart() noex {
	return SR_OK ;
}

int systater::ifinish() noex {
	return SR_OK ;
}

systater::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && mems[i] ; i += 1) {
	    systater_m	m = mems[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}

systater_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case systaterco_start:
		rs = op->istart() ;
		break ;
	    case systaterco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (systater_co::operator) */


