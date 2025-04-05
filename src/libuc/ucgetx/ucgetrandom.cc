/* ucgetrandom SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 2010-08-28, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2010 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getrandom

	Description:
	This is the front-end subroutine for the famous |getrandom(2)| 
	asystem call.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/random.h>		/* |getentropy(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"ucgetrandom.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	sysgetrandom(void *,int,uint) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getrandom(void *rbuf,int rlen,uint fl) noex {
	const caddr_t	ca = caddr_t(rbuf) ;
	int		rem = rlen ;
	int		inc = (fl & GRND_RANDOM) ? 512 : 33554431 ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	while ((rs >= 0) && (rem > 0)) {
	    cint	ml = min(rem,inc) ;
	    if ((rs = sysgetrandom((ca+rl),ml,fl)) >= 0) {
		rl += rs ;
		rem -= rs ;
	    }
	} /* end while */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_getrandom) */

int uc_getentropy(void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    char	*cbuf = cast_static<charp>(rbuf) ;
	    rs = SR_OK ;
	    cbuf[0] = '\0' ;
	    if (rlen > 0) {
		csize	rem = size_t(rlen) ;
	        if ((rs = getentropy(rbuf,rem)) < 0) {
		    rs = (- errno) ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getentropy) */


/* local subroutines */

static int sysgetrandom(void *rbuf,int rlen,uint fl) noex {
	csize		rem = size_t(rlen) ;
	int		rs ;
	repeat {
	    errno = 0 ;
	    if ((rs = getrandom(rbuf,rem,fl)) < 0) {
	        rs = (errno != 0) ? (- errno) : SR_NOTSUP ;
	    }
	} until ((rs != SR_INTR) && (rs != SR_AGAIN)) ;
	return rs ;
}
/* end subroutine (sysgetrandom) */


