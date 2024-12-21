/* sysmemutil SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* retrieve the utilization (in a percentage of total) of system memory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-18, David A­D­ Morano
	This little subroutine was put together to get the system
	physical memory utilization in terms of a percentage of the
	total.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysmemutil

	Description:
	We hope that the OS has some |sysconf(3c)| subroutines to
	help us here.  We really are tired of searching through the
	kernel for this sort of information. We return SR_NOSYS if
	the OS does not provide an easy want to get this stuff.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"sysmemutil.h"


/* local defines */

#ifndef	_SC_PHYS_PAGES
#define	_SC_PHYS_PAGES		-1
#endif
#ifndef	_SC_AVPHYS_PAGES
#define	_SC_AVPHYS_PAGES	-1
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	cmd = _SC_PHYS_PAGES ;
constexpr int	acmd = _SC_AVPHYS_PAGES ;


/* exported variables */


/* exported subroutines */

int sysmemutil(sysmemutil_dat *mup) noex {
	int		rs = SR_NOSYS ;
	int		percent = 0 ;
	if_constexpr ((cmd >= 0) && (acmd >= 0)) {
	    long	mt{} ;
	    if ((rs = uc_sysconfval(cmd,&mt)) >= 0) {
	        long	ma{} ;
	        if ((rs = uc_sysconfval(acmd,&ma)) >= 0) {
	      	    ulong	mu100 ;
	    	    if (mt > 0) {
	        	ulong	mu = (mt - ma) ;
	        	mu100 = (mu * 100) ;
	        	percent = (mu100 / mt) ;
		        if (mup) {
			    memclear(mup) ;
			    mup->mt = mt ;
			    mup->ma = ma ;
			    mup->mu = percent ;
			}
		    }
		} /* end if (sysconf) */
	    } /* end if (sysconf) */
	} else {
	    memclear(mup) ;
	} /* end if_constexpr (_SC_PHUS_PAGES) */
	return (rs >= 0) ? percent : rs ;
}
/* end subroutine (sysmemutil) */


