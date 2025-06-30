/* ucgetarchitecture SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the "architecture" of the present machine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getarchitecture

	Description:
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
#include	<isnot.h>
#include	<localmisc.h>


/* local defines */

#if	defined(__i386__) || defined(__x86_64__) || defined(__x86_64)
#define	F_X8664		1
#define	F_AMD		0
#define	F_ARM		0
#elif	defined(__amd_64) || defined(__amd64)
#define	F_X8664		0
#define	F_AMD		1
#define	F_ARM		1
#elif	defined(__arm__) || defined(__arm64__)
#define	F_X8664		0
#define	F_AMD		0
#define	F_ARM		1
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static sysret_t		ucgetarchitecture(char *,int) noex ;


/* local variables */

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;

constexpr bool		f_x8664		= F_X8664 ;
constexpr bool		f_amd		= F_AMD ;
constexpr bool		f_arm		= F_ARM ;


/* exported variables */


/* exported subroutines */

int uc_getarchitecture(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		cchar	*arch = nullptr ;
		rs = SR_OK ;
		if_constexpr (f_sunos) {
		    if_constexpr (f_x8664) {
		        arch = "x86_64" ;
		    } else if_constexpr (f_amd) {
		        arch = "amd64" ;
		    } else {
		        arch = "sparc" ;
		    }
		} else if_constexpr (f_darwin || f_linux) {
		    if_constexpr (f_x8664) {
		        arch = "x86_64" ;
		    } else if_constexpr (f_amd) {
		        arch = "amd64" ;
		    } else if_constexpr (f_arm) {
		        arch = "arm64" ;
		    }
		} /* end if_constexpr */
		if (arch) {
		    rs = sncpy(rbuf,rlen,arch) ;
		} else {
		    rs = ucgetarchitecture(rbuf,rlen) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getarchitecture) */


/* local subroutines */

static sysret_t ucgetarchitecture(char *rbuf,int rlen) noex {
	cint		cmd = usysauxinforeq_architecture ;
	int		rs ;
	if ((rs = uc_sysauxinfo(rbuf,rlen,cmd)) >= 0) {
	    /* nothing */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} else if (rs == SR_NOSYS) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (ucgetarchitecture) */


