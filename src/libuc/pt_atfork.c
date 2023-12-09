/* pt_atfork */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int pt_atfork(void (*b)(),void (*ap)(),void (*ac)()) noex {
	int		rs = SR_OK ;
	repeat {
	    if (pthread_atfork(b,ap,ac) != 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (pt_atfork) */


