/* strenv SUPPORT */
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
	strenv

	Description:
	Recommended usage within source code:
		#include	<strenv.hh>
		strenv	strpath(strenv_path) ;
		if (strpath != nullptr) {
			* do something w/ string-value pointer 'strpath' *
		}

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<timewatch.hh>
#include	<bufsizevar.hh>
#include	<ptm.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<varnames.hh>		/* |varname(3u)| */
#include	<localmisc.h>

#include	"strenv.hh"

import uconstants ;

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
	cchar		*name[strenv_overlast] ;
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
	cchar		*strp[strenv_overlast] ;
	char		*ma[strenv_overlast] ;	/* memory-allocation */
	bool		facc[strenv_overlast] ;
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	valstore_co	init ;
	valstore_co	fini ;
	valstore_co	monbegin ;
	valstore_co	monend ;
	valstore() noex {
	    init(this,valstoremem_init) ;
	    fini(this,valstoremem_fini) ;
	    monbegin(this,valstoremem_monbegin) ;
	    monend(this,valstoremem_monend) ;
	} ; /* end ctor */
	int valget(int,cchar **) noex ;
	int valtmpdir(int) noex ;
	int valmaildir(int) noex ;
	int valpath(int) noex ;
	int valenv(int) noex ;
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
	name[strenv_path] 		= varname.path ;
	name[strenv_fpath] 		= varname.fpath ;
	name[strenv_incpath] 		= varname.incpath ;
	name[strenv_libpath] 		= varname.libpath ;
	name[strenv_manpath] 		= varname.manpath ;
	name[strenv_infopath] 		= varname.infopath ;
	name[strenv_cdpath] 		= varname.cdpath ;
	name[strenv_tmpdir] 		= varname.tmpdir ;
	name[strenv_maildir] 		= varname.maildir ;
	name[strenv_node] 		= varname.node ;
	name[strenv_domain] 		= varname.domain ;
	name[strenv_localdomain]	= varname.localdomain ;
	name[strenv_username] 		= varname.username ;
	name[strenv_user] 		= varname.user ;
	name[strenv_logname] 		= varname.logname ;
	name[strenv_home] 		= varname.home ;
	name[strenv_mail] 		= varname.mail ;
	name[strenv_organization]	= varname.organization ;
	name[strenv_orgloc] 		= varname.orgloc ;
	name[strenv_orgcode] 		= varname.orgcode ;
} /* end method (strvarenv::ctor) */


/* forward references */

extern "C" {
    static void	valstore_atforkbefore() noex ;
    static void	valstore_atforkafter() noex ;
    static void	valstore_exit() noex ;
}


/* local variables */

constexpr strvarenv	enver ;

static bufsizevar	maxpathlen(getbufsize_mp) ;

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

strenv::operator ccharp () noex {
	cchar		*rp = strp ;
	if (! facc) {
	    if (int rs ; (rs = data.valget(w,&rp)) >= 0) {
		strp = rp ;
		facc = true ;
	    } else {
	        ulogerror("strenv",rs,"operator") ;
	    }
	} /* end if (needed) */
	return rp ;
}
/* end method (valstore::operator) */

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
	            if ((rs = uc_atforkrecord(b,a,a)) >= 0) ylikely {
	                if ((rs = uc_atexit(valstore_exit)) >= 0) {
	                    finitdone = true ;
	                    fr = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,a,a) ;
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
		        rsl = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? fr : rs ;
}
/* end method (valstore::iinit) */

int valstore::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
		for (int i = 0 ; i < strenv_overlast ; i += 1) {
		    if (ma[i]) {
			rs1 = uc_free(ma[i]) ;
			if (rs >= 0) rs = rs1 ;
			ma[i] = nullptr ;
		    }
		} /* end for */
	    }
	    {
	        void_f	b = valstore_atforkbefore ;
	        void_f	a = valstore_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
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
}
/* end method (valstore::ifini) */

int valstore::valget(int aw,cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	if ((aw >= 0) && (aw < strenv_overlast)) ylikely {
	    if ((rs = monbegin) >= 0) ylikely {
	        if (! facc[aw]) {
		    switch (aw) {
		    case strenv_tmpdir:
		        rs = valtmpdir(aw) ;
		        break ;
		    case strenv_maildir:
		        rs = valmaildir(aw) ;
		        break ;
		    case strenv_path:
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
}
/* end method (valstore::valget) */

int valstore::valtmpdir(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ; /* used-afterwards */
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_tmpdir ;
	    } /* end if (env-variable access) */
	    strp[aw] = rp ;
	} /* end if (env-variabiel name) */
	facc[aw] = true ;
	return rs ;
}
/* end method (valstore::valtmpdir) */

int valstore::valmaildir(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ;
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_maildir ;
	    } /* end if (env-variable access) */
	    strp[aw] = rp ;
	} /* end if (env-variabiel name) */
	facc[aw] = true ;
	return rs ;
}
/* end method (valstore::valmaildir) */

int valstore::valpath(int aw) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    cchar	*rp ; /* used-afterwards */
	    if ((rp = getenv(vn)) == nullptr) {
		if ((rs = maxpathlen) >= 0) ylikely {
		    cint	tlen = (rs * PLMULT) ;
		    if (char *tbuf ; (rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
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
			            ma[aw] = mallocstrw(tbuf,tl) ;
		                } /* end if (uc_sysconfstr) */
			    } /* end if (sncpy) */
		        } /* end if (mkpath) */
		        rs1 = uc_free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (getenv) */
	    strp[aw] = rp ;
	} /* end if (env-variabiel name) */
	facc[aw] = true ;
	return rs ;
}
/* end method (valstore::valpath) */

int valstore::valenv(int aw) noex {
	int		rs = SR_OK ;
	if (cchar *vn ; (vn = enver.name[aw]) != nullptr) {
	    strp[aw] = getenv(vn) ;
	}
	facc[aw] = true ;
	return rs ;
}
/* end method (valstore::valenv) */

void valstore::dtor() noex {
	if (cint rs = fini() ; rs < 0) {
	    ulogerror("strenv",rs,"dtor-fini") ;
	}
}
/* end method (valstore::dtor) */

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
}
/* end method (valstore_co) */

static void valstore_atforkbefore() noex {
	data.atforkbefore() ;
}
/* end subroutine (valstore_atforkbefore) */

static void valstore_atforkafter() noex {
	data.atforkafter() ;
}
/* end subroutine (valstore_atforkafter) */

static void valstore_exit() noex {
	data.fini() ;
}
/* end subroutine (valstore_exit) */


