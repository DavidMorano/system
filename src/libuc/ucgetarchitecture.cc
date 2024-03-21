/* ucgetarchitecture SUPPORT */
/* lang=C++20 */

/* get the "architecture" of the present machine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the "architecture" of the present machine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;


/* exported variables */


/* exported subroutines */

int uc_getarchitecture(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		if constexpr (f_sunos) {
		    rs = sncpy1(rbuf,rlen,"sparc") ;
		} else if constexpr (f_darwin) {
		    rs = sncpy1(rbuf,rlen,"x86_64") ;
		} else if constexpr (f_linux) {
		    rs = sncpy1(rbuf,rlen,"x86_64") ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gethostname) */


