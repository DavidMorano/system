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

	Description:
	Recommended usage within source code:
		#include	<strlibval.hh>
		strlibval	strpath(strlibval_path) ;
		if (strpath != nullptr) {
			* do something w/ string-value pointer 'strpath' *
		}

	Synopsis:
	val = strlibval(strlibvals w) noex

	Arguements:
	w		which value to retrieve

	Returns:
	val		return value (either substantive or NULL)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |getenver(3u)| */
#include	<ptm.h>			/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucfork.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"strlibval.hh"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |sysword(3u)| + |varname(3u)| */

/* local defines */

#define	PLMULT		4		/* path-length multiplier */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		valstore_init() noex ;
    int		valstore_fini() noex ;
}


/* external variables */


/* local structures */

namespace {
    struct strvarenv {
	cchar		*name[strlibval_overlast] ;
	constexpr strvarenv() noex ;
    } ; /* end struct (strvarenv) */
    enum valstoremems {
	valstoremem_init,
	valstoremem_fini,
	valstoremem_monbegin,
	valstoremem_monend,
	valstoremem_overlast
    } ; /* end enum (valstoremems) */
    struct valstore ;
    struct valstore_co {
	valstore	*op = nullptr ;
	int		w = -1 ;
	void operator () (valstore *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (valstore_co) */
    struct valstore {
	friend		valstore_co ;
	cchar		*strp	[strlibval_overlast] ;
	char		*ma	[strlibval_overlast] ;	/* memory-acquire */
	bool		facc	[strlibval_overlast] ;
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	valstore_co	init ;
	valstore_co	fini ;
	valstore_co	monbegin ;
	valstore_co	monend ;
	valstore() noex {
	    init	(this,valstoremem_init) ;
	    fini	(this,valstoremem_fini) ;
	    monbegin	(this,valstoremem_monbegin) ;
	    monend	(this,valstoremem_monend) ;
	} ; /* end ctor */
	int valget	(strlibvals,cchar **) noex ;
	int valtmpdir	(int) noex ;
	int valmaildir	(int) noex ;
	int valpath	(int) noex ;
	int valenv	(int) noex ;
        void atforkbefore() noex {
	    mx.lockbegin() ;
        }
        void atforkafter() noex {
	    mx.lockend() ;
        }
	void dtor() noex ;
	destruct valstore() {
	    if (finit || finitdone) dtor() ;
	} ;
    private:
	int iinit() noex ;
	int ifini() noex ;
	int imonbegin() noex {
	    int		rs ;
	    if ((rs = init) >= 0) {
		rs = mx.lockbegin ;
	    }
	    return rs ;
	} ;
	int imonend() noex {
	    return mx.lockend ;
	} ;
    } ; /* end struct (valstore) */
} /* end namespace */

constexpr strvarenv::strvarenv() noex {
	name[strlibval_cdpath]		= varname.cdpath ;
	name[strlibval_domain]		= varname.domain ;
	name[strlibval_fpath]		= varname.fpath ;
	name[strlibval_home]		= varname.home ;
	name[strlibval_incpath]		= varname.incpath ;
	name[strlibval_infopath]	= varname.infopath ;
	name[strlibval_libpath]		= varname.libpath ;
	name[strlibval_localdomain]	= varname.localdomain ;
	name[strlibval_logid]		= varname.logid ;
	name[strlibval_logline]		= varname.logline ;
	name[strlibval_logname]		= varname.logname ;
	name[strlibval_mail]		= varname.mail ;
	name[strlibval_maildir]		= varname.maildir ;
	name[strlibval_manpath]		= varname.manpath ;
	name[strlibval_node]		= varname.node ;
	name[strlibval_organization]	= varname.organization ;
	name[strlibval_orgcode]		= varname.orgcode ;
	name[strlibval_orgloc]		= varname.orgloc ;
	name[strlibval_path]		= varname.path ;
	name[strlibval_tmpdir]		= varname.tmpdir ;
	name[strlibval_user]		= varname.user ;
	name[strlibval_username]	= varname.username ;
	name[strlibval_utmpid]		= varname.utmpid ;
	name[strlibval_utmpline]	= varname.utmpline ;
	name[strlibval_utmpname]	= varname.utmpname ;
} /* end method (strvarenv::ctor) */


/* forward references */

extern "C" {
    local void	valstore_atforkbefore() noex ;
    local void	valstore_atforkafter() noex ;
    local void	valstore_exit() noex ;
}


/* local variables */

constexpr strvarenv	enver ;

static bufsizevar	maxpathlen(bufsize_mp) ;

static valstore		data ;


/* exported variables */


/* exported subroutines */

int valstore_init() noex {
	return data.init() ;
}

int valstore_fini() noex {
	return data.fini() ;
}


/* local subroutines */

strlibval::operator ccharp () noex {
	cchar		*rp = strp ;
	if (! facc) {
	    if (int rs ; (rs = data.valget(w,&rp)) >= 0) {
		strp = rp ;
		facc = true ;
	    } else {
	        ulogerror("strlibval",rs,"operator") ;
	    }
	} /* end if (needed) */
	return rp ;
} /* end method (valstore::operator) */

int valstore::iinit() noex {
	int		rs = SR_NXIO ;
	int		fr = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) ylikely {
	            void_f	b = valstore_atforkbefore ;
	            void_f	a = valstore_atforkafter ;
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) ylikely {
	                if ((rs = uc_atexit(valstore_exit)) >= 0) {
	                    finitdone = true ;
	                    fr = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexp(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (!finitdone) {
	        timewatch	tw(to) ;
	        cauto lamb = [this] () -> int {
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? fr : rs ;
} /* end method (valstore::iinit) */

int valstore::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
		for (int i = 0 ; i < strlibval_overlast ; i += 1) {
		    if (ma[i]) {
			rs1 = lm_free(ma[i]) ;
			if (rs >= 0) rs = rs1 ;
			ma[i] = nullptr ;
		    }
		} /* end for */
	    }
	    {
	        void_f	b = valstore_atforkbefore ;
	        void_f	a = valstore_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finitdone = false ;
	    finit = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end method (valstore::ifini) */

int valstore::valget(strlibvals aw,cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	if ((aw >= 0) && (aw < strlibval_overlast)) ylikely {
	    if ((rs = monbegin) >= 0) ylikely {
	        if (! facc[aw]) {
		    switch (aw) {
		    case strlibval_tmpdir:
		        rs = valtmpdir(aw) ;
		        break ;
		    case strlibval_maildir:
		        rs = valmaildir(aw) ;
		        break ;
		    case strlibval_path:
		        rs = valpath(aw) ;
		        break ;
		    default:
		        rs = valenv(aw) ;
		        break ;
		    } /* end switch */
	        } /* end if (not accessed) */
	        *rpp = strp[aw] ;
		rs1 = monend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mon) */
	} /* end if (valid) */
	return rs ;
} /* end method (valstore::valget) */

int valstore::valtmpdir(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ; /* used-afterwards */
	    if ((rp = getenver(vn)) == nullptr) {
		rp = sysword.w_tmpdir ;
	    } /* end if (env-variable access) */
	    strp[aw] = rp ;
	} /* end if (env-variable name) */
	facc[aw] = true ;
	return rs ;
} /* end method (valstore::valtmpdir) */

int valstore::valmaildir(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ; /* used-afterwards */
	    if ((rp = getenver(vn)) == nullptr) {
		rp = sysword.w_maildir ;
	    } /* end if (env-variable access) */
	    strp[aw] = rp ;
	} /* end if (env-variable name) */
	facc[aw] = true ;
	return rs ;
} /* end method (valstore::valmaildir) */

int valstore::valpath(int aw) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ; /* used-afterwards */
	    if ((rp = getenver(vn)) == nullptr) {
		if ((rs = maxpathlen) >= 0) ylikely {
		    cint	tlen = (rs * PLMULT) ;
		    if (char *tbuf ; (rs = lm_mall((tlen+1),&tbuf)) >= 0) {
		        cchar	*usrlocal = sysword.w_usrlocaldir ;
		        if ((rs = mkpath(tbuf,usrlocal,"bin")) >= 0) {
			    int		tl = rs ;
			    if ((rs = sncpy((tbuf+tl),(tlen-tl),":")) >= 0) {
		                cint	cmd = _CS_PATH ;
			        cint	clen = (tlen - (tl + rs)) ;
			        char	*cbuf = (tbuf + (tl + rs)) ;
			        tl += rs ;
		                if ((rs = uc_sysconfstr(cmd,cbuf,clen)) >= 0) {
			            tl += rs ;
				    cchar *cp ; 
				    if ((rs = lm_strw(tbuf,tl,&cp)) >= 0) {
			                ma[aw] = charp(cp) ;
				    }
		                } /* end if (uc_sysconfstr) */
			    } /* end if (sncpy) */
		        } /* end if (mkpath) */
		        rs1 = lm_free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (getenver) */
	    strp[aw] = rp ;
	} /* end if (env-variable name) */
	facc[aw] = true ;
	return rs ;
} /* end method (valstore::valpath) */

int valstore::valenv(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    strp[aw] = getenver(vn) ;
	}
	facc[aw] = true ;
	return rs ;
} /* end method (valstore::valenv) */

void valstore::dtor() noex {
	if (cint rs = fini() ; rs < 0) {
	    ulogerror("strlibval",rs,"dtor-fini") ;
	}
} /* end method (valstore::dtor) */

valstore_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case valstoremem_init:
	        rs = op->iinit() ;
	        break ;
	    case valstoremem_fini:
	        rs = op->ifini() ;
	        break ;
	    case valstoremem_monbegin:
	        rs = op->imonbegin() ;
	        break ;
	    case valstoremem_monend:
	        rs = op->imonend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (valstore_co) */

local void valstore_atforkbefore() noex {
	data.atforkbefore() ;
} /* end subroutine (valstore_atforkbefore) */

local void valstore_atforkafter() noex {
	data.atforkafter() ;
} /* end subroutine (valstore_atforkafter) */

local void valstore_exit() noex {
	data.fini() ;
} /* end subroutine (valstore_exit) */


