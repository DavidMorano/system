/* sysdbfname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* special function to retrieve system-database (SYSDB) filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D-­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysdbfnameget

	Description:
	This is a specially created subroutine to pretty much fulfill
	one purpose.  This subroutine is called by the various
	subroutines that open the system-database (SYSDB) special
	files.  These special files (SYSDB files) are located at a
	certain location in the system filesystem.  The location
	itself is not so important, as long as we have a convenient
	subroutine (like this present one) that will return that
	location for us.  That is one of the purposes for this
	present subroutine, to return the location in the system
	filesystem for each of the special SYSDB files.  But
	additioanlly, and this is a pure hack specialization for
	our particular callers, if we are called with a filename
	(presumably the filename of a SYSDB file), we honor that
	and simply return it.  This is a bone to the fact that we
	are pretty much called by one (or one type) of caller, and
	we may as well provide any additional small service that
	they (the callers) may like.  We are thread and fork safe,
	et cetera.  We are not (like almost everybody else)
	async-signal-safe (it is just not needed in the current
	world).

	Symopsis:
	int sysdbfnameget(sysdbfiles n,cc *fn,cc **rpp) noex

	Arguments:
	n		name to retrieve (the type is |sysdbfnames|)
	fn		given file-name (or NULL if want the default)
	rpp		pointer to the result c-string

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<syswords.hh>
#include	<libmallocxx.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"sysdbfname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		sysdbmgr_init() noex ;
    int		sysdbmgr_fini() noex ;
}


/* external variables */


/* local structures */

namespace {
    struct sysdbmgr {
	cchar		*strs[sysdbfile_overlast] ;
	ptm		mx ;		/* data mutex */
	pid_t		pid ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int get(int,cchar **) noex ;
        void atforkbefore() noex {
	    mx.lockbegin() ;
        }
        void atforkafter() noex {
	    mx.lockend() ;
        }
	~sysdbmgr() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
		ulogerror("sysdbmgr",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (sysdbmgr) */
    } ; /* end structure (sysdbmgr) */
}


/* forward references */

extern "C" {
    static void	sysdbmgr_atforkbefore() noex ;
    static void	sysdbmgr_atforkafter() noex ;
    static void	sysdbmgr_exit() noex ;
}


/* lcoal variables */

static sysdbmgr		sysdbmgr_data ;


/* exported variables */


/* exported subroutines */

int sysdbmgr_init() noex {
	return sysdbmgr_data.init() ;
}

int sysdbmgr_fini() noex {
	return sysdbmgr_data.fini() ;
}

int sysdbfnameget(enum sysdbfiles w,cchar *fname,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rpp) {
	    rs = SR_INVALID ;
	    if ((w >= 0) && (w < sysdbfile_overlast)) {
		if (fname) {
		    if (fname[0]) {
			rs = SR_OK ;
			*rpp = fname ;
		    } /* end if (valid) */
		} else {
		    rs = sysdbmgr_data.get(w,rpp) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysdbfnameget) */


/* local subroutines */

int sysdbmgr::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = sysdbmgr_atforkbefore ;
	            void_f	a = sysdbmgr_atforkafter ;
	            if ((rs = uc_atforkrecord(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(sysdbmgr_exit)) >= 0) {
	                    finitdone = true ;
	                    f = true ;
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
	        rs = tw(lamb) ;		/* <- time watching */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (sysdbmgr::init) */

int sysdbmgr::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
		for (int i = 0 ; i < sysdbfile_overlast ; i += 1) {
		    if (cchar *str ; (str = strs[i]) != nullptr) {
			rs1 = uc_libfree(str) ;
			if (rs >= 0) rs = rs1 ;
			strs[i] = nullptr ;
		    }
		} /* end for */
	    }
	    {
	        void_f	b = sysdbmgr_atforkbefore ;
	        void_f	a = sysdbmgr_atforkafter ;
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
/* end method (sysdbmgr::fini) */

int sysdbmgr::get(int w,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((*rpp = strs[w]) == nullptr) {
	    if ((rs = init()) >= 0) {
		if ((rs = mx.lockbegin) >= 0) {
		    if ((*rpp = strs[w]) == nullptr) {
		        char	*pbuf{} ;
		        if ((rs = libmalloc_mp(&pbuf)) >= 0) {
		            cchar	*sysdbdir = sysword.w_sysdbdir ;
			    cchar	*fn = sysdbfnames[w] ;
		            if ((rs = mkpath(pbuf,sysdbdir,fn)) >= 0) {
				auto	alloc = uc_libmallocstrw ;
	                        cchar	*rp{} ;
		                if ((rs = alloc(pbuf,rs,&rp)) >= 0) {
			            strs[w] = rp ;
			            *rpp = rp ;
			        } /* end if (memory-allocation) */
		            } /* end if (mkpath) */
		            rs1 = uc_libfree(pbuf) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
		    } /* end if (string-check) */
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	    } /* end if (sysdbmgr::init) */
	} /* end if (value needed to be calculated) */
	return rs ;
}
/* end subroutine (sysdbmgr::get) */

static void sysdbmgr_atforkbefore() noex {
	sysdbmgr_data.atforkbefore() ;
}
/* end subroutine (sysdbmgr_atforkbefore) */

static void sysdbmgr_atforkafter() noex {
	sysdbmgr_data.atforkafter() ;
}
/* end subroutine (sysdbmgr_atforkafter) */

static void sysdbmgr_exit() noex {
	cint	rs = sysdbmgr_data.fini() ;
	if (rs < 0) {
	    ulogerror("sysdbmgr",rs,"exit-fini") ;
	}
}
/* end subroutine (sysdbmgr_exit) */


