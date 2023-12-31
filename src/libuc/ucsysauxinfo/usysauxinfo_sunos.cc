/* usysauxinfo_sunos */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright � 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.


*******************************************************************************/

#include	<envstandards.h>

/* USYSAUXINFO_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

#include	<sys/types.h>
#include	<sys/systeminfo.h>	/* <- where the stuff we want is */
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstring>
#include	<usysrets.h>
#include	<xlanguage.h>

#include	"usysauxinfo_sunos.h"


/* local defines */


/* local namespaces */


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
	    int		r = -1 ;
	    rs = SR_NOENT ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
		r = SI_ARCHITECTURE ;
		break ;
	    case SAI_PLATFORM:
		r = SI_PLATFORM ;
		break ;
	    case SAI_HWPROVIDER:
		r = SI_HW_PROVIDER ;
		break ;
	    case SAI_HWSERIAL:
		r = SI_HW_SERIAL ;
		break ;
	    case SAI_SRPCDOMAIN:
		r = SI_SRPC_DOMAIN ;
		break ;
	    } /* end switch */
	    if (r >= 0) {
		rs = u_sysauxinfo(rbuf,rlen,r) ;
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


#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYSAUXINFO_SUNOS finish */


