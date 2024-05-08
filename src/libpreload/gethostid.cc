/* gethostid SUPPORT */
/* lang=C++20 */

/* Get-Host-ID UNIX® System interposer */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a version of |gethostid(3c)| that is preloaded to over-ride
	the standard UNIX® system version.

	Q. Is this multi-thread safe?
	A. Since it is a knock-off of an existing UNIX® system LIBC (3c)
	   subroutine that is already multi-thread safe -- then of course
	   it is!

	Q. Is this much slower than the default system version?
	A. No, not really.

	Q. How are we smarter than the default system version?
	A. Let me count the ways:
		+ value optionally from environment
		+ value optionally from a configuration file
		+ customizable built-in compiled default
		+ value is cached!

	Q. Why are you so smart?
	A. I do not know.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/systeminfo.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"preload.h"


/* local defines */

#ifndef	VARHOSTID
#define	VARHOSTID	"HOSTID"
#endif

#define	GETHOSTID	struct gethostid_head


/* external subroutines */


/* local structures */

struct gethostid_head {
	long		hostid ;
} ;


/* forward references */


/* local variables */

static GETHOSTID	gethostid_data ; /* zero-initialized */


/* exported variables */


/* exported subroutines */

long gethostid(void) noex {
	GETHOSTID	*gip = &gethostid_data ;
	long		rc = 0 ;
	int		rs = SR_OK ;
	if (gip->hostid == 0) {
	    cint	dlen = DIGBUFLEN ;
	    int		vl = -1 ;
	    cchar	*var = VARHOSTID ;
	    cchar	*vp ;
	    char	dbuf[DIGBUFLEN+1] ;
	    if (((vp = getenv(var)) == NULL) || (vp[0] == '\0')) {
	        if ((rs = u_sysinfo(SI_HW_SERIAL,dbuf,dlen)) >= 0) {
	            vp = dbuf ;
	            vl = rs ;
	        } /* end if (u_sysinfo) */
	    } /* end if (getenv) */
	    if (rs >= 0) {
	        uint	uv ;
	        if ((rs = cfnumui(vp,vl,&uv)) >= 0) {
		    gip->hostid = (long) uv ;
		    rc = (long) uv ;
	        }
	    } /* end if (ok) */
	    if (rs < 0) errno = (- rs) ;
	} else {
	    rc = gip->hostid ;
	}
	return rc ;
}
/* end subroutine (gethostid) */


