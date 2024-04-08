/* u_access */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* exported variables */


/* exported subroutines */

int u_access(cchar *fname,int am) noex {
	int		rs ;
	repeat {
	    if ((rs = access(fname,am)) < 0) rs = (- errno) ;
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_access) */


