/* strenv SUPPORT */
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

	Type:
	strenv

	Recommended usage within source code:
		#include	<strenv.hh>
		strenv	strpath(strenv_path) ;
		if (strpath != nullptr) {
			* do something w/ string-value pointer 'strpath' *
		}

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>		/* for |getenv(3c)| */
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<timewatch.hh>
#include	<bufsizevar.hh>
#include	<getbufsize.h>
#include	<ptm.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"strenv.hh"


/* local defines */

#define	PLMULT		2		/* path-length multiplier */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		valstore_init() noex ;
    int		valstore_fini() noex ;
}


/* external variables */


/* local strutures */

namespace {
    struct strvarenv {
	cchar		*name[strenv_overlast] ;
	constexpr strvarenv() noex ;
    } ; /* end struct (strvarenv) */
    struct valstore {
	cchar		*strp[strenv_overlast] ;
	char		*a[strenv_overlast] ;
	bool		facc[strenv_overlast] ;
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int getval(int,cchar **) noex ;
	int valenv(int) noex ;
	int calc(int) noex ;
        void atforkbefore() noex {
	    mx.lockbegin() ;
        }
        void atforkafter() noex {
	    mx.lockend() ;
        }
	void dtor() noex ;
	~valstore() {
	    dtor() ;
	} ;
    } ; /* end struct (valstore) */
}

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
}
/* end method (strvarenv::ctor) */


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
	    int		rs ;
	    if ((rs = data.getval(w,&rp)) >= 0) {
		strp = rp ;
		facc = true ;
	    } else {
	        ulogerror("strenv",rs,"operator") ;
	    }
	} /* end if (needed) */
	return rp ;
}
/* end method (strenv::operator) */

#ifdef	COMMENT

strenv::operator ccharp () noex {
	cchar		*rp = strp ;
	if (! facc) {
	    cint	to = utimeout[uto_busy] ;
	    if (! fmx.testandset) {
	        if ((w >= 0) && (w < strenv_overlast)) {
		    switch (w) {
		    case strenv_tmpdir:
		        rp = strtmpdir() ;
		        break ;
		    case strenv_maildir:
		        rp = strmaildir() ;
		        break ;
		    case strenv_path:
		        rp = strpath() ;
		        break ;
		    default:
		        rp = valenv() ;
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
/* end method (strenv::operator) */

ccharp strenv::strtmpdir() noex {
	cchar	*rp = nullptr ;
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_tmpdir ;
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strenv::strtmpdir) */

ccharp strenv::strmaildir() noex {
	cchar	*rp = nullptr ;
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		rp = sysword.w_maildir ;
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strenv::strmaildir) */

cchar *strenv::strpath() noex {
	cchar	*rp = nullptr ;
	if (cchar *vn ; (vn = enver.name[w]) != nullptr) {
	    if ((rp = getenv(vn)) == nullptr) {
		int	rs ;
		int	rs1 ;
		if ((rs = maxpathlen) >= 0) {
		    cint	tlen = (rs * PLMULT) ;
		    char	*tbuf{} ;
		    if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
		        cchar	*usrlocal = sysword.w_usrlocaldir ;
		        if ((rs = mkpath(tbuf,usrlocal,"bin")) >= 0) {
			    int		tl = rs ;
			    if ((rs = sncpy((tbuf+tl),(tlen-tl),":")) >= 0) {
		                cint	cmd = _CS_PATH ;
			        cint	clen = (tlen - (tl+rs)) ;
			        char	*cbuf = (tbuf + (tl+rs)) ;
			        tl += rs ;
		                if ((rs = uc_confstr(cbuf,clen,cmd)) >= 0) {
			            tl += rs ;
			            a = mallocstrw(tbuf,tl) ;
			            rp = a ;
		                } /* end if (uc_confstr) */
			    } /* end if (sncpy) */
		        } /* end if (mkpath) */
		        rs1 = uc_free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
		if (rs < 0) {
		    rp = nullptr ;
		    ulogerror("strenv::strpath",rs,"path construction") ;
		}
	    } /* end if (env-variable access) */
	    strp = rp ;
	} /* end if (env-variabiel name) */
	return rp ;
}
/* end method (strenv::strpath) */

#endif /* COMMENT */

int valstore::init() noex {
	int		rs = SR_NXIO ;
	int		fr = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = valstore_atforkbefore ;
	            void_f	a = valstore_atforkafter ;
	            if ((rs = uc_atforkrecord(b,a,a)) >= 0) {
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
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rs = 1 ;
	            }
	            return rs ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? fr : rs ;
}
/* end method (valstore::init) */

int valstore::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
		for (int i = 0 ; i < strenv_overlast ; i += 1) {
		    if (a[i]) {
			rs1 = uc_free(a[i]) ;
			if (rs >= 0) rs = rs1 ;
			a[i] = nullptr ;
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
/* end method (valstore::fini) */

int valstore::getval(int aw,cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	if ((aw >= 0) && (aw < strenv_overlast)) {
	    rs = SR_OK ;
	    if (! facc[aw]) {
		if ((rs = mx.lockbegin) >= 0) {
		    switch (aw) {
		    case strenv_tmpdir:
		        break ;
		    case strenv_maildir:
		        break ;
		    case strenv_path:
		        break ;
		    default:
		        rs = valenv(aw) ;
		        break ;
		    } /* end switch */
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	    } /* end if (not accessed) */
	    *rpp = strp[aw] ;
	} /* end if (valid) */
	return rs ;
}
/* end method (valstore::getval) */

int valstore::calc(int aw) noex {
	int		rs = SR_OK ;
	cchar		*rp = strp[aw] ;
	if (! facc) {
	    cint	to = utimeout[uto_busy] ;
	    if (! fmx.testandset) {
	        if ((w >= 0) && (w < strenv_overlast)) {
		    switch (w) {
		    case strenv_tmpdir:
		        rp = strtmpdir() ;
		        break ;
		    case strenv_maildir:
		        rp = strmaildir() ;
		        break ;
		    case strenv_path:
		        rp = strpath() ;
		        break ;
		    default:
		        rp = valenv() ;
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
/* end method (valstore::calc) */

int valstore::valenv(int aw) noex {
	int		rs = SR_OK ;
	if (! facc[aw]) {
		cchar	*vn = enver.name[aw] ;
		strp[aw] = getenv(vn) ;
		facc[aw] = true ;
	}
	return rs ;
}
/* end method (strenv::valenv) */

void valstore::dtor() noex {
	cint		rs = fini() ;
	if (rs < 0) {
	    ulogerror("strenv",rs,"dtor-fini") ;
	}
}
/* end method (valstore::dtor) */

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


