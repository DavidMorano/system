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
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<logzones.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif


/* external subroutines */


/* external variables */


/* local structures */


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
		    logzones	*lzp = &pip->lz ;
		    cmode	om = 0666 ;
		    cint	of = O_RDWR ;
		    cint	zl = rs ;
	            if ((rs = logzones_open(lzp,zfname,of,om)) >= 0) {
			cchar	**vpp = &pip->zfname ;
			pip->open.logzone = TRUE ;
	        	rs = proginfo_setentry(pip,vpp,zfname,zl) ;
	        	if (pip->debuglevel > 0) {
			    cchar	*pn = pip->progname ;
	            	    bprintf(pip->efp,"%s: zf=%s\n",pn,zfname) ;
			}
			if (rs < 0)
			    logzones_close(lzp) ;
		    } else if (isNotPresent(rs))
			rs = SR_OK ;
		}
	    }
	} /* end if (log-zones) */
	return rs ;
}
/* end subroutine (proglogzone_begin) */

int proglogzone_end(proginfo *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.logzone) {
	    pip->open.logzone = FALSE ;
	    rs1 = logzones_close(&pip->lz) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (proglogzone_end) */

int proglogzone_update(proginfo *pip,cchar *zn,int zl,int zoff,cchar *zs) noex {
	int		rs = SR_OK ;
	if (pip->open.logzone) {
	    logzones_update(&pip->lz,zn,zl,zoff,zs) ;
	}
	return rs ;
}
/* end subroutine (proglogzone_update) */


