/* mkcdpath */
/* lang=C++11 */

/* try to make a prefix-variable path */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkcdpath

	Description:
	This subroutine possibly expands the given path string if
	it is prefixed with the CDPATH indicator character.

	Synopsis:
	int mkcdpath(char *ebuf,cchar *pp,int pl) noex

	Arguments:
	ebuf		result buffer (should be MAXPATHLEN+1 long)
	pp		source path pointer
	pl		source path length

	Returns:
	<0		error
	==0		no expansion
	>0		expansion

	Implementation note:
	Yes, we do not add extra slash characters between components
	of file paths (reasonably). And we are not ashamed of this
	practice. We do not though, remove extra slashes that are
	already preent (although we could if we wanted, to be extra
	smart about it).

	Form:
	¬[<varname>]/<path>

	Example:
	¬/stage/daytime
	¬cdpath/stage/daytime

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<strwcpy.h>
#include	<storebuf.h>
#include	<ucvariables.hh>
#include	<getbufsize.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>


/* local defines */

#define	MKCDPATH_MP	(4*1024)

#undef	CH_EXPAND
#define	CH_EXPAND	MKCHAR('¬')

#ifndef	VARCDPATH
#define	VARCDPATH	"CDPATH"
#endif


/* external subroutines */

extern "C" int	pathadd(char *,int,cchar *) noex ;
extern "C" int	pathaddw(char *,int,cchar *,int) noex ;
extern "C" int	sfbasename(cchar *,int,cchar **) noex ;
extern "C" int	isNotPresent(int) noex ;

extern "C" cchar	*getenver(cchar *,int) noex ;

extern "C" char		*strnchr(cchar *,int,int) noex ;


/* external variables */


/* local structures */

class mkcdpathsub {
	int		vl = 0 ;
	int		sl ;
	int		bl = 0 ;
	cchar		*plist = NULL ;
	cchar		*vp = NULL ;
	cchar		*sp ;
	cchar		*bp = NULL ;
	char		*ebuf ;
public:
	mkcdpathsub(char *abuf,cchar *asp,int asl) noex : sl(asl), sp(asp) {
	    ebuf = abuf ;
	    vp = sp ;
	} ;
	int getvarname() noex {
	    cchar	*tp ;
	    if ((tp = strnchr(sp,sl,'/')) != NULL) {
		vl = (tp-sp) ;
		sl -= ((tp+1)-sp) ;
		sp = (tp+1) ;
	    }
	    return vl ;
	} ;
	int getplist() noex {
	     int	rs = SR_OK ;
	     if (vl > 0) {
		char	*vn ;
		if ((vn = new char [vl+1]) != NULL) {
		    strwcpyuc(vn,vp,vl) ;
		    plist = getenver(vn,vl) ;
		    delete [] vn ;
		} else {
		    rs = SR_NOMEM ;
	 	} /* end if (m-a-f) */
	     } else {
		plist = getenv(varname.cdpath) ;
	     }
	     return rs ;
	} ;
	int getbasename() noex {
	    if ((bl = sfbasename(sp,sl,&bp)) > 0) {
		sl = (bp-sp-1) ;
	    }
	    return sl ;
	} ;
	int testpaths() noex ;
	int testpath(cchar *,int) noex ;
	int mkjoin(cchar *,int) noex ;
	int mkresult(int) noex ;
} ;
/* end class (mkcdpathsub) */


/* forward references */

extern "C" int	mkcdpath(char *,cchar *,int) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKCDPATH_MP) ;


/* exported subroutines */

int mkcdpath(char *ebuf,cchar *fp,int fl) noex {
	int		rs = SR_FAULT ;
	int		el = 0 ;
	if (ebuf && fp) {
	    cint	ec = CH_EXPAND ;
	    ebuf[0] = '\0' ;
	    if (fl < 0) fl = strlen(fp) ;
	    if ((fl > 0) && (MKCHAR(fp[0]) == ec)) {
	        mkcdpathsub 	*sip = new mkcdpathsub(ebuf,(fp+1),(fl+1)) ;
	        if ((rs = sip->getvarname()) >= 0) {
		    if ((rs = sip->getplist()) >= 0) {
		        if ((rs = sip->getbasename()) >= 0) {
			    if ((rs = sip->testpaths()) > 0) {
			        rs = sip->mkresult(rs) ;
			        el = rs ;
		            }
		        }
		    } /* end if (mkcdpathsub::getplist) */
	        } /* end if (mkcdpathsub::getvarname) */
	        delete sip ;
	    } /* end if (have one) */
	} /* end if (non-null) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkcdpath) */


/* local srubroutines */

int mkcdpathsub::testpaths() noex {
	int		rs = SR_OK ;
	int		el = 0 ;
	if (plist != NULL) {
	    int		pl = strlen(plist) ;
	    cchar	*pp = plist ;
	    cchar	*tp ;
	    while ((tp = strnchr(pp,pl,':')) != NULL) {
		if ((tp-pp) > 0) {
		    rs = testpath(pp,(tp-pp)) ;
		    el = rs ;
	        }
		pl -= ((tp+1)-pp) ;
	        pp = (tp+1) ;
		if (rs != 0) break ;
	    } /* end while */
	    if ((rs == 0) && (pl > 0)) {
	        rs = testpath(pp,pl) ;
		el = rs ;
	    }
	} /* end if (plist) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkcdpathsub::testpaths) */

int mkcdpathsub::testpath(cchar *cp,int cl) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = mkjoin(cp,cl)) >= 0) {
	    USTAT	sb ;
	    el = rs ;
	    if ((rs = u_stat(ebuf,&sb)) >= 0) {
		rs = 0 ;
	    } else if (isNotPresent(rs)) {
		el = 0 ;
		rs = SR_OK ;
	    }
	} /* end if (mkjoin) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkcdpathsub::testpath) */

int mkcdpathsub::mkjoin(cchar *cp,int cl) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint		elen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(ebuf,elen,el,cp,cl) ;
	        el += rs ;
	    }
	    if ((rs >= 0) && el && (ebuf[el-1] != '/')) {
	        rs = storebuf_char(ebuf,elen,el,'/') ;
	        el += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(ebuf,elen,el,sp,sl) ;
	        el += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkcdpathsub::mkjoin) */

int mkcdpathsub::mkresult(int el) noex {
	return pathaddw(ebuf,el,bp,bl) ;
}
/* end subroutine (mkcdpathsub::mkresult) */


