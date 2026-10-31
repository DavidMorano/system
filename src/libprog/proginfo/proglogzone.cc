/* proglogzone SUPPORT */
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

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bufsize.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<logzone.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"
#include	"proglogzone.hh"


/* local defines */

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#ifndef	LOGZONEFNAME
#define LOGZONEFNAME    "zones"
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace vars {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int proglogzone_begin(proginfo *pip) noex {
	int		rs = SR_OK ;
	if (pip->fl.optlogzone) {
	    cint	nlen = MAXNAMELEN ;
	    cchar	*sn = pip->searchname ;
	    cchar	*zone = LOGZONEFNAME ;
	    char	nbuf[MAXNAMELEN+1] ;
	    if ((rs = snsds(nbuf,nlen,sn,zone)) >= 0) {
		cchar	*logcname = LOGCNAME ;
	        char	zfname[MAXPATHLEN+1] ;
	        if ((rs = mkpath3(zfname,pip->pr,logcname,nbuf)) >= 0) {
		    logzone	*lzp = &pip->lz ;
		    cmode	om = 0666 ;
		    cint	of = O_RDWR ;
		    cint	zl = rs ;
	            if ((rs = logzone_open(lzp,zfname,of,om)) >= 0) {
			cchar	**vpp = &pip->zfname ;
			pip->pfopen.logzone = true ;
	        	rs = pip->setent(vpp,zfname,zl) ;
	        	if (pip->debuglevel > 0) {
	            	    pip->eprintf("zf=%s\n",zfname) ;
			}
			if (rs < 0) {
			    logzone_close(lzp) ;
			} /* end if (error) */
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    } /* end if */
		} /* end if */
	    } /* end if */
	} /* end if (log-zones) */
	return rs ;
} /* end subroutine (proglogzone_begin) */

int proglogzone_end(proginfo *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->pfopen.logzone) {
	    rs = SR_BUGCHECK ;
	    if (logzone *lzp = resumelife<logzone>(pip->lzp)) {
	        pip->pfopen.logzone = false ;
		{
	            rs1 = logzone_close(lzp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = mem.free(lzp) ;
	            if (rs >= 0) rs = rs1 ;
		    lzp = nullptr ;
		} /* end block (memory-release) */
	    } /* end if (non-null) */
	} /* end */
	return rs ;
} /* end subroutine (proglogzone_end) */

int proglogzone_update(proginfo *pip,cchar *zn,int zl,int zoff,cchar *zs) noex {
    	int		rs = SR_FAULT ;
	if (pip) ylikely {
	    rs = SR_OK ;
	    if (pip->pfopen.logzone) {
	        rs = SR_BUGCHECK ;
	        if (logzone *lzp = resumelife<logzone>(pip->lzp)) {
	            rs = logzone_update(lzp,zn,zl,zoff,zs) ;
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
	    if ((rs = bufsizeget(bufsize_np)) >= 0) {
	    	maxpathlen = rs ;
	    }
	} /* end */
	return rs ;
} /* end method (vars::operator) */


