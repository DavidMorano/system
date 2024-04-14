/* sigaction SUPPORT */
/* lang=C++20 */

/* subroutines to manipulate SIGACTION values */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-24, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We manage (a little bit) the SIGACTION object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	SIGACTION
#define	SIGACTION	struct sigaction
#endif


/* local typedefs */

#ifndef	TYPEDEF_SIGINFOHAND
#define	TYPEDEF_SIGINFOHAND
extern "C" {
    typedef void (*siginfohand_f)(int,siginfo_t *,void *) noex ;
}
#endif


/* external subroutines */

extern "C" {
    int sigaction_load(SIGACTION *,sigset_t *,int,siginfohand_f) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int sigaction_load(SIGACTION *sap,sigset_t *ssp,int fl,siginfohand_f h) noex {
	int		rs = SR_FAULT ;
	if (sap && ssp) {
	    memclear(sap) ;
	    sap->sa_mask = *ssp ;
	    sap->sa_flags = fl ;
	    sap->sa_handler = h ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigaction_load) */


