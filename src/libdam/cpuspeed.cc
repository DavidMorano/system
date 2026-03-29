/* cpuspeed HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load a CPUSPEED module and call it */
/* version %I% last-modified %G% */

#define	CF_PARENT	0		/* emable searching parent */

/* revision history:

	= 2003-08-13, David A­D­ Morano
	This is an original write, but was inspired by previous similar
	subroutines.

	= 2025-11-12, David A­D­ Morano
	I modified this to dynamically search for a program-root.
	We all live in a more complicated world now.

*/

/* Copyright © 2003,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cpuspeed

  	Description:
	This subroutine finds and loads up a CPUSPEED module, and
	then executes it.  The resulting "speed" is returned by the
	called module and then is returned by this subroutine.

	Synopsis:
	int cpuspeed(cchar *pr,cchar *name,int nruns) noex

	Arguments:
	pr	program-root
	name	module name to load
	nruns	number of runs to execute

	Returns:
	>=0	completed OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getnodedomain.h>	/* |getinetdomain(3uc)| */
#include	<mkpr.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<sncpyx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"cpuspeed.h"


/* local defines */

#ifndef	CF_PARENT
#define	CF_PARENT	0		/* emable searching parent */
#endif

#ifndef	PRNAME
#define	PRNAME		"LOCAL"
#endif

#ifndef	OFD
#define	OFD		"S5"
#endif

#define	LIBDNAME	"lib/cpuspeed"

#define	ENTRYNAME	"cpuspeed"
#define	NRUNS		10000000


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*mod_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct loadmgr {
	void		*dhp{} ;
	char		*tbuf{} ;
	char		*pbuf{} ;
	char		*fname{} ;	/* loadable-module file-name */
	cchar		*prn ;		/* derived */
	int		tlen ;
	int		plen ;
	int		flen ;
	int		dlmode ;
	int		nruns ;
	loadmgr(int s) noex : nruns(s) { } ;
	int operator () (cchar *,cchar *) noex ;
	int defs(cc *,cc *) noex ;
	int defvals() noex ;
	int defpr(cchar *) noex ;
	int defname(cchar *) noex ;
	int modbegin(cchar *) noex ;
	int modend() noex ;
	int mkfname(cchar *) noex ;
	int speedrun() noex ;
    } ; /* end struct (loadmgr) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr cpcchar	subdirs[] = {
	"sparcv9",
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ; /* end array (subdirs) */

constexpr cpcchar	names[] = {
	"dhry",
	nullptr
} ; /* end array (names) */

constexpr cpcchar	exts[] = {
	"",
	"so",
	"o",
	"dyld",
	nullptr
} ; /* end array (exts) */

cbool			f_parent = CF_PARENT ;


/* exported variables */


/* exported suroutines */

int cpuspeed(cchar *pr,cchar *name,int nruns) noex {
	loadmgr		lf(nruns) ;
	return lf(pr,name) ;
} /* end subroutine (cpuspeed) */


/* local subroutines */

int loadmgr::operator () (cc *pr,cc *name) noex {
    	int		rs ;
	int		rs1 ;
	int		speed = 0 ; /* return-value */
	if ((rs = lm_mp(&tbuf)) >= 0) ylikely {
	    tlen = rs ;
	    if ((rs = lm_mp(&pbuf)) >= 0) ylikely {
	        plen = rs ;
	        if ((rs = lm_mp(&fname)) >= 0) ylikely {
		    flen = rs ;
	            if ((rs = defs(pr,name)) >= 0) ylikely {
			{
		            rs = speedrun() ;
			    speed = rs ;
			}
			rs1 = modend() ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (defs) */
	            rs1 = lm_free(fname) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        rs1 = lm_free(pbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? speed : rs ;
} /* end method (loadmgr::operator) */

int loadmgr::defs(cc *pr,cc *name) noex {
    	int		rs ;
	if ((rs = defvals()) >= 0) ylikely {
	    if ((rs = defpr(pr)) >= 0) ylikely {
	        rs = defname(name) ;
	    }
	}
	return rs ;
} /* end method (loadmgr::defs) */

int loadmgr::defvals() noex {
    	int		rs = SR_OK ;
	if (nruns <= 0) {
	    nruns = NRUNS ;
	}
	{
	    dlmode = (RTLD_LAZY | RTLD_LOCAL) ;
	    if_constexpr (f_parent) {
	        dlmode |= RTLD_PARENT ;
	    }
	}
	return rs ;
} /* end method (loadmgr::defvals) */

int loadmgr::defpr(cc *pr) noex {
    	int		rs ;
	int		rs1 ;
	if (pr && pr[0]) {
	    prn = pr ;
	} else {
	    if (char *dbuf ; (rs = lm_hn(&dbuf)) >= 0) ylikely {
	        cint dlen = rs ;
	        if ((rs = getinetdomain(dbuf,dlen)) >= 0) ylikely {
	            if ((rs = mkpr(pbuf,plen,dbuf,PRNAME)) >= 0) ylikely {
		        prn = pbuf ;
		    }
	        } /* end if (getinetdomain) */
	        rs1 = lm_free(dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (pr) */
	return rs ;
} /* end method (loadmgr::defpr) */

int loadmgr::defname(cc *name) noex {
    	int		rs = SR_NOENT ;
	if (name && name[0]) {
	    rs = modbegin(name) ;
	} else {
	    for (int i = 0 ; names[i] != nullptr ; i += 1) {
		cchar *n = names[i] ;
	        rs = modbegin(n) ;
	        if (rs != 0) break ;
	    } /* end for */
	} /* end if (have or search) */
	return rs ;
} /* end method (loadmgr::defname) */

int loadmgr::modbegin(cc *name) noex {
    	int		rs = SR_OK ;
	int		fl = 0 ; /* return-value */
	cchar		*ldn = LIBDNAME ;
	dhp = nullptr ;
	for (int i = 0 ; i < 2 ; i += 1) {
	    cchar	*lp = (i == 0) ? OFD : "" ;
	    for (int j = 0 ; subdirs[j] != nullptr ; j += 1) {
		cchar *sd = subdirs[j] ;
	        if ((rs = mkpath(tbuf,prn,ldn,lp,sd,name)) >= 0) {
	            for (int k = 0 ; exts[k] != nullptr ; k += 1) {
			if ((rs = mkfname(exts[k])) >= 0) ylikely {
			    fl = rs ;
	                    if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
			        if (S_ISREG(sb.st_mode)) {
	                            dhp = dlopen(fname,dlmode) ;
			        }
		            } else if (isNotPresent(rs)) {
			        rs = SR_OK ;
			        fl = 0 ;
		            } /* end if (u_stat) */
			} /* end if (mkfname) */
	                if (dhp != nullptr) break ;
	            } /* end for (extensions) */
		} /* end if (mkpath) */
	        if (dhp != nullptr) break ;
	    } /* end for (subdirs) */
	    if (dhp != nullptr) break ;
	} /* end for (major machine designator) */
	if (dhp == nullptr) fl = 0 ;
	return (rs >= 0) ? fl : rs ;
} /* end method (loadmgr::modbegin) */

int loadmgr::modend() noex {
    	int		rs = SR_OK ;
	if (dhp) {
	    dlclose(dhp) ;
	    dhp = nullptr ;
	}
	return rs ;
} /* end method (loadmgr::modend) */

int loadmgr::mkfname(cchar *ext) noex {
    	int		rs ;
	if (ext[0]) {
	    rs = mkfnamesuf1(fname,tbuf,ext) ;
	} else {
	    rs = mkpath(fname,tbuf) ;
	}
	return rs ;
} /* end method (loadmgr::mkfname) */

int loadmgr::speedrun() noex {
    	cnullptr	np{} ;
	int		rs = SR_LIBACC ;
	int		speed = 0 ; /* return-value */
	if (dhp) {
	    cchar	*en = ENTRYNAME ;
	    if (mod_f fp ; (fp = (mod_f) dlsym(dhp,en)) != np) {
	        rs = (*fp)(nruns) ;
		speed = rs ;
	    } else {
	        rs = SR_LIBBAD ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? speed : rs ;
} /* end method (loadmgr::speedrun) */


