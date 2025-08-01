/* cpuspeed HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load a CPUSPEED module and call it */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-08-13, David A­D­ Morano
	This is an original write.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cpuspeed

  	Description:
	This subroutine loads up a CPUSPEED module.

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
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<mkpr.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<sncpyx.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"cpuspeed.h"


/* local defines */

#ifdef	PRDOMAIN
#define	PRDOMAIN	"LOCAL"
#endif

#ifndef	PROGRAMEOOT
#define	PROGRAMROOT	"/usr/add-on/local"
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
    typedef int (*sub_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct loadmgr {
	void		*dhp ;
	cchar		*pr ;
	cchar		*name ;
	char		*fname{} ;
	int		dlmode ;
	int		nruns ;
	loadmgr(cc *r,cc *n,int s) noex : pr(r), name(n), nruns(s) { } ;
	operator int () noex ;
    } ; /* end struct (loadmgr) */
} /* end namespace */

int loadmgr::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = malloc_mp(&fname)) >= 0) {
	    {
		rs = SR_OK ;
	    }
	    rs1 = uc_free(fname() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end method (loadmgr::operator) */


/* forward references */

static int	lfile(loadmgr *,cchar *,cchar *) noex ;


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
	nullptr
} ; /* end array (exts) */


/* exported variables */


/* exported suroutines */

int cpuspeed(cchar *pr,cchar *name,int nruns) noex {
    	cnullptr	np{} ;
	loadmgr		lf{} ;
	int		rs ;
	int		i ;
	int		speed ;
	void		*dhp ;

/* program root */

	if (pr == nullptr)
	    pr = PROGRAMROOT ;

	if (nruns <= 0)
	    nruns = NRUNS ;

#if	CF_PARENT
	lf.dlmode = RTLD_LAZY | RTLD_LOCAL | RTLD_PARENT ;
#else
	lf.dlmode = RTLD_LAZY | RTLD_LOCAL ;
#endif

	rs = SR_NOTFOUND ;
	if ((name == nullptr) || (name[0] == '\0')) {
	    for (i = 0 ; names[i] != nullptr ; i += 1) {
	        rs = lfile(&lf,pr,names[i]) ;
	        if (rs > 0) break ;
	    } /* end for */
	} else {
	    rs = lfile(&lf,pr,name) ;
	}

	if (rs >= 0) {
	    dhp = lf.dhp ;
	    if (dhp != nullptr) {
	        cchar	*en = ENTRYNAME ;
	        if (sub_f fp ; (fp = (sub_f) dlsym(dhp,en)) != np) {
	            speed = (*fp)(nruns) ;
	        } else {
	            rs = SR_LIBBAD ;
	        }
	        dlclose(dhp) ;
	    } else {
	        rs = SR_LIBACC ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? speed : rs ;
}
/* end subroutine (cpuspeed) */


/* local subroutines */

static int lfile(loadmgr *lfp,cc *pr,cc *name) noex {
	ustat	sb ;
	int		i, j, k ;
	int		fl = 0 ;
	cchar	*lp = nullptr ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (lfp == nullptr)
	    return SR_FAULT ;

	for (i = 0 ; i < 2 ; i += 1) {

	    lp = (i == 0) ? OFD : "" ;
	    for (j = 0 ; subdirs[j] != nullptr ; j += 1) {

	        mkpath5(tmpfname,pr,LIBDNAME,lp,subdirs[j],name) ;

	        for (k = 0 ; exts[k] != nullptr ; k += 1) {

	            if (exts[k][0] != '\0') {
	                fl = mkfnamesuf1(lfp->fname,tmpfname,exts[k]) ;
	            } else {
	                fl = sncpy1(lfp->fname,MAXPATHLEN,tmpfname) ;
		    }

	            lfp->dhp = nullptr ;
	            if ((u_stat(lfp->fname,&sb) >= 0) && (S_ISREG(sb.st_mode)))
	                lfp->dhp = dlopen(lfp->fname,lfp->dlmode) ;

	            if (lfp->dhp != nullptr) break ;
	        } /* end for (extensions) */

	        if (lfp->dhp != nullptr) break ;
	    } /* end for (subdirs) */

	    if (lfp->dhp != nullptr) break ;
	} /* end for (major machine designator) */

	return (lfp->dhp != nullptr) ? fl : SR_NOENT ;
}
/* end subroutine (lfile) */


