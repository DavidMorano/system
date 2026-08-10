/* mknpathvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* try to make a prefix-variable path of type VARPATH */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mknpathvar

	Description:
	This subroutine possibly expands the given path string if
	it is prefixed with the VARPATH indicator character.

	Synopsis:
	int mknpathvar(char *rbuf,int rlen,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	pp		source path pointer
	pl		source path length

	Returns:
	>0		expansion
	==0		no expansion
	<0		error (system-return)

	Implementation note:
	Yes, we do not add extra slash characters between components
	of file paths (reasonably).  And we are not ashamed of this
	practice.  We do not though, remove extra slashes that are
	already preent (although we could if we wanted, to be extra
	smart about it).

	Form:
	¬[<varname>]/<path>

	Given (for example):
	VARPATH=/home/tools:/somewhere
	OTHER=/home/tools:/other/place

	Example:
	¬/stage/daytime
	¬pathvar/stage/daytime
	¬other/stage/daytime

	Results:
	/home/tools/stage/daytime
	/home/tools/stage/daytime
	/other/place/stage/daytime

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<storebuf.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<sfx.h>			/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkx.h"
#include	"mknpathxx.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |getlenstr(3u)| */
import uconstants ;

/* local defines */

#define	MKPATHVAR_MP	(4*1024)

#define	CHX_EXP		MKCHAR('¬')


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */
namespace {
    class mksub {
	cchar		*plist = nullptr ;
	cchar		*vp = nullptr ;
	cchar		*sp ;		/* caller-supplied argument */
	cchar		*bp = nullptr ;
	char		*rbuf ;		/* caller-supplied argument */
	int		rlen ;		/* caller-supplied argument */
	int		vl = 0 ;
	int		sl ;		/* caller-supplied argument */
	int		bl = 0 ;
	int		el = 0 ;	/* result expanded length */
    public:
	mksub(char *µrbuf,int µrlen,cchar *µsp,int µsl) noex {
	    rbuf = µrbuf ;
	    rlen = µrlen ;
	    sp = µsp ;
	    sl = µsl ;
	    vp = sp ;
	} ; /* end ctor */
	int getvarname	()		noex ;
	int getplist	()		noex ;
	int getbasename	()		noex ;
	int testpath	(cchar *,int)	noex ;
	int mkjoin	(cchar *,int)	noex ;
	int testpaths	()		noex ;
	int mkresult	()		noex ;
    } ; /* end class (mksub) */
    typedef int (mksub::*mksub_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr mksub_m	mksubs[] = {
    &mksub::getvarname,
    &mksub::getplist,
    &mksub::getbasename,
    &mksub::testpaths,
    &mksub::mkresult
} ; /* end array (mksubs) */


/* exported variables */


/* exported subroutines */

int mknpathvar(char *rbuf,int rlen,cchar *fp,int µfl) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	int		el = 0 ;
	if (rbuf && fp) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (int fl ; (fl = getlenstr(fp,µfl)) > 0) ylikely {
	        cint	chx_ec = CHX_EXP ;
		rs = SR_OK ;
		if (cint ch = mkchar(fp[0]) ; ch == chx_ec) ylikely {
		    cint	sl = (fl + 1) ;
		    cchar	*sp = (fp + 1) ;
		    rs = SR_NOMEM ;
	            if (mksub *sip = new(nt) mksub(rbuf,rlen,sp,sl) ; sip) {
		        for (cauto &m : mksubs) {
			    rs = (sip->*m)() ;
			    if (rs <= 0) break ;
		        } /* end for */
		        el = rs ;
	                delete sip ;
		    } /* end if (m-a-f) */
	        } /* end if (have one) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? el : rs ;
} /* end subroutine (mknpathvar) */


/* local srubroutines */

int mksub::getvarname() noex {
	if (cchar *tp ; (tp = strnchr(sp,sl,'/')) != nullptr) {
	    vl = intconv(tp - sp) ;
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	} /* end if (strnchr) */
	return vl ;
} /* end method (mksub::getvarname) */

int mksub::getplist() noex {
	cnothrow	nt{} ;
	int		rs = SR_OK ;
	if (vl > 0) {
	    rs = SR_NOMEM ;
	    if (char *vn = new(nt) char [vl+1] ; vn) {
		strwcpyuc(vn,vp,vl) ;
		plist = getenver(vn,vl) ;
		rs = SR_OK ;
		delete [] vn ;
	    } /* end if (m-a-f) */
	} else {
	    static cchar *vvp = getenver(varname.cdpath) ;
	    plist = vvp ;
	}
	return rs ;
} /* end method (mksub::getplist) */

int mksub::getbasename() noex {
	if ((bl = sfbasename(sp,sl,&bp)) > 0) {
	    sl = intconv(bp - sp - 1) ; /* <- return dir-path len */
	} /* end if (sfbasename) */
	return sl ;
} /* end method (mksub::getbasename) */

int mksub::testpaths() noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (plist != nullptr) ylikely {
	    int		pl = lenstr(plist) ;
	    cchar	*pp = plist ;
	    for (cchar *tp ; (tp = strnchr(pp,pl,':')) != np ; ) {
		cint	ll = intconv(tp - pp) ;
		if (ll > 0) ylikely {
		    rs = testpath(pp,ll) ;
		    el = rs ;
	        }
		pl -= intconv((tp + 1) - pp) ;
	        pp = (tp + 1) ;
		if (rs != 0) break ;
	    } /* end for */
	    if ((rs == 0) && (pl > 0)) {
	        rs = testpath(pp,pl) ;
		el = rs ;
	    }
	} /* end if (plist) */
	return (rs >= 0) ? el : rs ;
} /* end subroutine (mksub::testpaths) */

int mksub::testpath(cchar *cp,int cl) noex {
	int		rs ;
	if ((rs = mkjoin(cp,cl)) >= 0) ylikely {
	    cint	jl = rs ;
	    if (ustat sb ; (rs = u_stat(rbuf,&sb)) >= 0) ylikely {
		if (S_ISDIR(sb.st_mode)) {
		    rs = jl ;
		} else {
		    rs = SR_OK ;
		}
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (mkjoin) */
	return rs ;
} /* end subroutine (mksub::testpath) */

int mksub::mkjoin(cchar *cp,int cl) noex {
	int		rs ;
	int		len = 0 ;
	if (storebuf sb(rbuf,rlen) ; (rs = sb.strw(cp,cl)) >= 0) ylikely {
	    if ((rs > 0) && (rbuf[rs - 1] != '/')) {
	        rs = sb.chr('/') ;
	    }
	    if (rs >= 0) ylikely {
	        rs = sb.strw(sp,sl) ;
		len = sb.idx ;
	    }
	} /* end if (storebuf) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mksub::mkjoin) */

int mksub::mkresult() noex {
	return pathnaddw(rbuf,rlen,el,bp,bl) ;
} /* end subroutine (mksub::mkresult) */


