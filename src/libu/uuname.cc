/* u_uname */
/* lang=C++23 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>
#include	<cstring>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local definies */


/* external subroutines */


/* exported subroutines */

int u_uname(struct utsname *up) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (up) {
	    if ((rs = uname(up)) >= 0) {
		cint	nlen = (sizeof(up->machine)-1) ;
		char	*nbuf = up->machine ;
		cchar	*cp = up->machine ;
		bool	f = false ;
		rc = rs ;
		f = f || (strcmp(cp,"x86_64") == 0) ;
		if (f) {
		    cchar	*np = "Apple-Macintosh-Intel" ;
		    rs = sncpy1(nbuf,nlen,np) ;
		}
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (u_uname) */


