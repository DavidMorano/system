/* ucgetrandom SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/types.h>		/* POSIX® |caddr_t| */
#include	<sys/random.h>		/* POSIX® |getentropy(2)| */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysutility.hh>	/* LIBU |ugetrandom(3u)| */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetrandom.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::ugetrandom ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

csize		maxget = 256 ;		/* |getrandom| maximum to get */


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
	    if ((rs = ugetrandom((ca+rl),ml,fl)) >= 0) {
		rl += rs ;
		rem -= rs ;
	    }
	} /* end while */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (uc_getrandom) */

int uc_getentropy(void *vbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (vbuf) {
	    char *rbuf = cast_static<charp>(vbuf) ;
	    rs = SR_OK ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
	        int rl = 0 ;
		for (size_t rem = size_t(rlen) ; rem > 0 ; ) {
		    csize msize = min(rem,maxget) ;
	            if ((rs = getentropy((rbuf + rl),msize)) >= 0) {
			rem -= msize ;
			rl += intconv(msize) ;
		    } else {
		        rs = (neg errno) ;
		    }
		    if (rs < 0) break ;
		} /* end for */
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
} /* end subroutine (uc_getentropy) */


/* local subroutines */


