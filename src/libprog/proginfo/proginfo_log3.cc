/* proginfo_log3 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* log handling for Time-Zones stastics */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	proglogzone

	Description:
	Here we do some logging of time-zone information.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsize.h>		/* LIBUC */
#include	<memo.hh>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<logzone.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.log ;

import libutil ;			/* |lenstr(3u)| */
import proginfo.base ;

/* local defines */

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#ifndef	LOGZONEFNAME
#define LOGZONEFNAME    "zones"
#endif


/* imported namespaces */

using std::destroy_at ;			/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct beginer {
	proginfo	*pip ;
	beginer(proginfo *p) noex : pip(p) { } ;
	operator int	() noex ;
	int zfmake	(char *) noex ;
	int zfopen	(cchar *,int) noex ;
	int zfstore	(cchar *,int) noex ;
    } ; /* end struct (beginer) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;
cchar			lzfname[]	= LOGZONEFNAME ;


/* exported variables */


/* exported subroutines */

int proglogzone_begin(proginfo *pip) noex {
	int		rs = SR_OK ;
	if (pip->pf.optlogzone) {
	    if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		cint sz = szof(logzone) ;
		if (void *p ; (rs = mem.mall(sz,&p)) >= 0) {
		    if (logzone *lzp = new(p) logzone ; lzp) {
			pip->lzp = lzp ;
			{
		            beginer bo(pip) ;
		            rs = bo ;
			}
		    } /* end if (new-logzone) */
		} /* end if (memory-allocation) */
	    } /* end if (vars) */
	} /* end if (log-zones) */
	return rs ;
} /* end subroutine (proglogzone_begin) */

beginer::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (charp pbuf ; (rs = mem.mp(&pbuf)) >= 0) {
	    if ((rs = zfmake(pbuf)) >= 0) {
		rs = zfopen(pbuf,rs) ;
		rv = rs ;
	    } /* end if (zfmake) */
	    rs1 = mem.free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    pbuf = nullptr ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
} /* end method (beginer::operator) */

int beginer::zfmake(char *pbuf) noex {
    	int		rs ;
	int		rs1 ;
	int		pl = 0 ; /* return-value */
	ccharp		sn = pip->searchname ;
	if (char *nbuf ; (rs = mem.mn(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if ((rs = snsds(nbuf,nlen,sn,lzfname)) >= 0) {
		ccharp	logcname = LOGCNAME ;
	        rs = mkpath(pbuf,pip->pr,logcname,nbuf) ;
		pl = rs ;
	    } /* end if (snsds) */
	    rs1 = mem.free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? pl : rs ;
} /* end method (mkfnane) */

int beginer::zfopen(ccharp pbuf,int pl) noex {
    	int		rs = SR_BUGCHECK ;
	if (logzone *lzp = resumelife<logzone>(pip->lzp)) {
	    cmode	om = 0666 ;
	    cint	of = O_RDWR ;
	    if ((rs = lzp->open(pbuf,of,om)) >= 0) {
		pip->pfopen.logzone = true ;
		{
		    rs = zfstore(pbuf,pl) ;
		}
		if (rs < 0) {
		    pip->pfopen.logzone = false ;
		    lzp->close() ;
		} /* end if (error) */
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end method (beginer::zfopen) */

int beginer::zfstore(ccharp pbuf,int pl) noex {
    	int		rs ;
	ccharpp	vpp = &pip->zfname ;
	if ((rs = pip->setent(vpp,pbuf,pl)) >= 0) {
	    if (pip->debuglevel > 0) {
		pip->eprintf("zf=%s\n",pip->zfname) ;
	    }
	} /* end if (setent) */
	return rs ;
} /* end method (beginer::zfstore) */

int proglogzone_end(proginfo *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->pfopen.logzone) {
	    rs = SR_BUGCHECK ;
	    if (logzone *lzp = resumelife<logzone>(pip->lzp)) {
	        pip->pfopen.logzone = false ;
		{
	            rs1 = lzp->close ;
	            if (rs >= 0) rs = rs1 ;
		} /* end */
		{
		    destroy_at(lzp) ;
		} /* end if (destruct) */
		{
		    rs1 = mem.free(lzp) ;
	            if (rs >= 0) rs = rs1 ;
		    pip->lzp = nullptr ;
		} /* end block (memory-release) */
	    } /* end if (non-null) */
	} /* end */
	return rs ;
} /* end subroutine (proglogzone_end) */

int proglogzone_update(proginfo *pip,cchar *zn,int zl,int zoff,cchar *zs) noex {
    	int		rs = SR_FAULT ;
	if (pip && zn) ylikely {
	    rs = SR_OK ;
	    if (pip->pfopen.logzone) {
	        rs = SR_BUGCHECK ;
	        if (logzone *lzp = resumelife<logzone>(pip->lzp)) {
	            rs = lzp->update(zn,zl,zoff,zs) ;
	        }
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglogzone_update) */


/* local subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_nn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    	maxpathlen = rs ;
	    }
	} /* end */
	return rs ;
} /* end method (vars::operator) */


