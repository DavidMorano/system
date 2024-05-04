/* usys_ugetnisdom SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We define some missing operating system utily functions.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysrets.h>
#include	<usysflag.h>
#include	<utypedefs.h>

#include	"usys_sunos.h"
#include	"usys_darwin.h"
#include	"usys_linux.h"

#include	"usys_ugetnisdom.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;


/* exported variables */


/* exported subroutines */

int ugetnisdom(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		if constexpr (f_sunos) {
		    rs = sunos_ugetnisdom(rbuf,rlen) ;
		    len = rs ;
		} else if (f_darwin) {
		    rs = darwin_ugetnisdom(rbuf,rlen) ;
		    len = rs ;
		} else if (f_linux) {
		    rs = linux_ugetnisdom(rbuf,rlen) ;
		    len = rs ;
		} else {
		    rs = xxx_ugetnisdom(rbuf,rlen) ;
		    len = rs ;
		} /* end if-constexpr (which OS) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ugetnisdom) */


