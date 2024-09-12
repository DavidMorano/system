/* ucgetarchitecture SUPPORT */
/* lang=C++20 */

/* get the "architecture" of the present machine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the "architecture" of the present machine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */

#if	defined(__i386__) || defined(__x86_64__)
#define	F_X8664		1
#define	F_ARM		0
#elif	defined(__arm__)  || defined(__arm64__)
#define	F_X8664		0
#define	F_ARM		1
#endif


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

constexpr bool		f_x8664		= F_X8664 ;
constexpr bool		f_arm		= F_ARM ;


/* exported variables */


/* exported subroutines */

int uc_getarchitecture(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		rs = SR_OK ;
		if_constexpr (f_sunos) {
		    rs = sncpy1(rbuf,rlen,"sparc") ;
		} else if_constexpr (f_darwin) {
		    if_constexpr (f_x8664) {
		        rs = sncpy1(rbuf,rlen,"x86_64") ;
		    } else if_constexpr (f_arm) {
		        rs = sncpy1(rbuf,rlen,"arm64") ;
		    }
		} else if_constexpr (f_linux) {
		    rs = sncpy1(rbuf,rlen,"x86_64") ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getarchitecture) */


