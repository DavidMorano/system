/* strlibval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this object provides a pointer to a library string-value */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlibval

	Description
	Recommended usage within source code:
		#include	<strlibval.hh>
		strlibval	strpath(strlibval_path) ;
		if (strpath != nullptr) {
			* do something w/ string-value pointer 'strpath' *
		}

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>		/* |utimeout(3u)| */
#include	<timewatch.hh>
#include	<varnames.hh>
#include	<syswords.hh>
#include	<bufsizevar.hh>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<varnames.hh>		/* |varname(3u)| */
#include	<localmisc.h>

#include	"strlibval.hh"


/* local defines */

#define	PLMULT		4		/* path-length multiplier */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct strvarenv {
	cchar		*name[strlibval_overlast] ;
	constexpr strvarenv() noex ;
    } ; /* end struct (strvarenv) */
}

constexpr strvarenv::strvarenv() noex {
	name[strlibval_logid]		= varname.logid ;
	name[strlibval_logname]		= varname.logname ;
	name[strlibval_logline]		= varname.logline ;
	name[strlibval_utmpid]		= varname.utmpid ;
	name[strlibval_utmpname]	= varname.utmpname ;
	name[strlibval_utmpline]	= varname.utmpline ;
	name[strlibval_path]		= varname.path ;
	name[strlibval_fpath]		= varname.fpath ;
	name[strlibval_incpath]		= varname.incpath ;
	name[strlibval_libpath]		= varname.libpath ;
	name[strlibval_manpath]		= varname.manpath ;
	name[strlibval_infopath]	= varname.infopath ;
	name[strlibval_cdpath]		= varname.cdpath ;
	name[strlibval_tmpdir]		= varname.tmpdir ;
	name[strlibval_maildir]		= varname.maildir ;
	name[strlibval_node]		= varname.node ;
	name[strlibval_domain]		= varname.domain ;
	name[strlibval_localdomain]	= varname.localdomain ;
	name[strlibval_username]	= varname.username ;
	name[strlibval_user]		= varname.user ;
	name[strlibval_home]		= varname.home ;
	name[strlibval_mail]		= varname.mail ;
	name[strlibval_organization]	= varname.organization ;
	name[strlibval_orgloc]		= varname.orgloc ;
	name[strlibval_orgcode]		= varname.orgcode ;
} /* end method (strvarenv::ctor) */


/* forward references */


/* local variables */

constexpr strvarenv	enver ;

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

strlibval::operator ccharp () noex {
	cchar		*rp ; /* return-value */
	if ((rp = strp) == nullptr) {
	    cint	to = utimeout[uto_busy] ;
	    if (! fmx.testandset) {
	        if ((w >= 0) && (w < strlibval_overlast)) {
		    switch (w) {
		    case strlibval_tmpdir:
		        rp = strtmpdir() ;
		        break ;
		    case strlibval_maildir:
		        rp = strmaildir() ;
		        break ;
		    case strlibval_path:
		        rp = strpath() ;
		        break ;
		    default:
		        rp = cook() ;
		        break ;
		    } /* end switch */
		    if (rp) {
			fready.notifyall(true) ;
		    }
	        } /* end if (type) */
            } else if (!fready) {
                timewatch       tw(to) ;
                auto lamb = [this] () -> int {
                    int         rs = SR_OK ;
                    if (!fmx) {
                        rs = SR_LOCKLOST ;
                    } else if (fready) {
                        rs = 1 ;
                    }
                    return rs ;
                } ; /* end lambda */
                if (int rs ; (rs = tw(lamb)) >= 0) { /* <- time-watching */
		    rp = strp ;
	        } ;
	    } else {
		rp = strp ;
	    } /* end if (atomic access) */
	} /* end if (need to create value) */
	return rp ;
}
/* end method (strlibval::operator) */


/* local subroutines */

void strlibval::dtor() noex {
	strp = nullptr ;
	if (a) {
	    if (cint rs = uc_free(a) ; rs >= 0) {
	        a = nullptr ;
	    } else {
		ulogerror("strlibpath::dtor",rs,"dtor-uc_free") ;
	    }
	}
}
/* end method (strlibval::dtor) */

ccharp strlibval::cook() noex {
	cchar	*rp = nullptr ; /* return-value */
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    rp = getenv(vn) ;
	    strp = rp ;
	}
	return rp ;
}
/* end method (strlibval::cook) */

ccharp strlibval::strtmpdir() noex {
	cchar	*rp = nullptr ; /* return-value */
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_tmpdir ;
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strlibval::strtmpdir) */

ccharp strlibval::strmaildir() noex {
	cchar	*rp = nullptr ; /* return-value */
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_maildir ;
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strlibval::strmaildir) */

ccharp strlibval::strpath() noex {
	cchar	*rp = nullptr ; /* return-value */
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		int	rs ;
		int	rs1 ;
		if ((rs = maxpathlen) >= 0) {
		    cint	tlen = (rs * PLMULT) ;
		    if (char *tbuf{} ; (rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
		        cchar	*usrlocal = sysword.w_usrlocaldir ;
		        if ((rs = mkpath(tbuf,usrlocal,"bin")) >= 0) {
			    int		tl = rs ;
			    if ((rs = sncpy((tbuf+tl),(tlen-tl),":")) >= 0) {
		                cint	cmd = _CS_PATH ;
			        cint	clen = (tlen - (tl+rs)) ;
			        char	*cbuf = (tbuf + (tl+rs)) ;
			        tl += rs ;
		                if ((rs = uc_sysconfstr(cmd,cbuf,clen)) >= 0) {
			            tl += rs ;
			            a = mallocstrw(tbuf,tl) ;
			            rp = a ;
		                } /* end if (uc_sysconfstr) */
			    } /* end if (sncpy) */
		        } /* end if (mkpath) */
		        rs1 = uc_free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
		if (rs < 0) {
		    rp = nullptr ;
		    ulogerror("strlibval::strpath",rs,"path construction") ;
		}
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strlibval::strpath) */


