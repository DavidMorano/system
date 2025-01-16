/* initnow SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* initialize both TIMEB and ZNAME */
/* version %I% last-modified %G% */

#define	CF_FTIME	1		/* actually call |uc_ftime(3uc)| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	initnow

	Description:
	This subroutine initializes a couple of time-related
	variables.  One of these variables is a TIMEB structure.  The
	other is a time-zone string.

	Synopsis:
	int initnow(TIMEB *tbp,char *zbuf,int zlen) noex

	Arguments:
	tbp		pointer to TIMEB structure
	zbuf		zone-name buffer pointer (returned to caller)
	zlen		zone-name buffer length of result

	Returns:
	>=0		OK
	<0		error (system-return)

	Implementation notes:
	Calling |ftime(3c)| sets the local time-zone information
	(through) a secret call to (as you know) |tzset(3c)|.  Note
	also that the Darwin OS (used on Macs as the core of MacOS)
	does not maintain the normal external variables that are
	set by |tzset(3c)| as previous, more traditional, OSes did.
	This is a positive development and one that should have
	been in there from the beginning, but provision has to made
	for it none-the-less.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* <- Apple says this is necessary! */
#include	<sys/param.h>
#include	<sys/timeb.h>		/* <- the money shot! */
#include	<unistd.h>
#include	<ctime>			/* structure |TIMEB| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |memset(3c)| */
#include	<usystem.h>		/* UNIX® structure aliases */
#include	<tmtime.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"initnow.h"


/* local defines */

#ifndef	CF_FTIME
#define	CF_FTIME	0		/* actually call |uc_ftime(3uc)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int initnow_ftime(TIMEB *,char *,int) noex ;
static int initnow_gettime(TIMEB *,char *,int) noex ;


/* local variables */

constexpr bool		f_ftime = CF_FTIME ;


/* exported variables */


/* exported subroutines */

int initnow(TIMEB *tbp,char *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (tbp) {
	    memclear(tbp) ;
	    if (zbuf) zbuf[0] = '\0' ;
	    if_constexpr (f_ftime) {
		rs = initnow_ftime(tbp,zbuf,zlen) ;
		len = rs ;
	    } else {
		rs = initnow_gettime(tbp,zbuf,zlen) ;
		len = rs ;
	    } /* end if_constexpr (f_ftime) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (initnow) */


/* local subroutines */

static int initnow_ftime(TIMEB *tbp,char *zbuf,int zlen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = uc_ftime(tbp)) >= 0) {
	    if (zbuf) {
	        if (tmtime tmt ; (rs = tmtime_localtime(&tmt,tbp->time)) >= 0) {
	            rs = sncpy1(zbuf,zlen,tmt.zname) ;
	            len = rs ;
	        } /* end if (tmtime_localtime) */
	    } /* end if (zbuf) */
	} /* end if (uc_ftime) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (initnow_ftime) */

static int initnow_gettime(TIMEB *tbp,char *zbuf,int zlen) noex {
	int		rs ;
	int		len = 0 ;
	if (TIMEVAL tv ; (rs = uc_gettimeofday(&tv,nullptr)) >= 0) {
            tbp->time = tv.tv_sec ;
            tbp->millitm = (tv.tv_usec / 1000) ;
            if (tmtime tmt ; (rs = tmtime_localtime(&tmt,tbp->time)) >= 0) {
                tbp->timezone = (tmt.gmtoff / 60) ;
                tbp->dstflag = tmt.isdst ;
                if (zbuf) {
                    rs = sncpy1(zbuf,zlen,tmt.zname) ;
                    len = rs ;
                }
            } /* end if (tmtime_localtime) */
	} /* end if (uc_gettimeofday) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (initnow_gettime) */


