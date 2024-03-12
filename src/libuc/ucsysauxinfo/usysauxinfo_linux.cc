/* usysauxinfo_linux */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.


*******************************************************************************/

#include	<envstandards.h>

/* USYSAUXINFO_LINUX start */
#if	defined(OSNAME_Linux) && (OSNAME_Linux > 0)


#include	<sys/types.h>
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstring>
#include	<usysrets.h>
#include	<xlanguage.h>

#include	"usysauxinfo_linux.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* SYSAUXINFO begin */
#if	defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int snwcpy(char *,const char *,int) noex ;

int sysauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    const char	*vp = nullptr ;
	    rs = SR_NOENT ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
	 	vp = "x86_64" ;
		break ;
	    case SAI_PLATFORM:
	 	vp = "Mac-Mini" ;
		break ;
	    case SAI_HWPROVIDER:
	 	vp = "Apple" ;
		break ;
	    } /* end switch */
	    if (vp) {
		rs = snwcpy(rbuf,rlen,vp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysauxinfo) */

#ifdef	__cplusplus
}
#endif

#endif /* defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0) */
/* SYSAUXINFO end */


#endif /* defined(OSNAME_Linux) && (OSNAME_Linux > 0) */
/* USYSAUXINFO_LINUX finish */


