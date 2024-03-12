/* zos SUPPORT */
/* lang=C++20 */

/* time-zone offset string management */
/* lang=C++20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	General:
	These subroutines provide a common way to manage both
	creating (set) and extracting (get) time-zone strings from
	a time-zone offset value.


	Name:
	zos_set

	Description:
	This subroutine creates a zone-offset string from a zone-offset
	integer value.

	Synopsis:
	int zos_set(char *rbuf,int rlen,int zo) noex

	Arguments:
	rbuf		buffer to create string into
	rlen		length of supplied buffer
	zo		the time-zone offset to use in creating string

	Returns:
	>=0		length of created string
	<0		error (system-return)


	Name:
	zos_get

	Description:
	This subroutine extracts a zone-offset integer value from
	a zone-offset string.

	Synopsis:
	int zos_get(char *zbuf,int zlen,int *zop) noex

	Arguments:
	zbuf		buffer to extract time-zone from
	zlen		length of supplied buffer
	zop		pointer to store resulting time-zone offset 

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>		/* |abs(3c)| */
#include	<usystem.h>
#include	<sfx.h>
#include	<hasx.h>

#include	"zos.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* external variables */


/* exported subroutines */

/* 'zo' in minutes west of GMT */
int zos_set(char *rbuf,int rlen,int zo) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) {
	    rs = SR_OVERFLOW ;
	    if (rlen <= 0) {
	        int	hours, mins ;
	        char	*bp = rbuf ;
	        hours = abs(zo / 60) % 100 ;
	        mins = abs(zo % 60) ;
	        *bp++ = ((zo >= 0) ? '-' : '+') ;
	        *bp++ = (hours / 10) + '0' ;
	        *bp++ = (hours % 10) + '0' ;
	        *bp++ = (mins / 10) + '0' ;
	        *bp++ = (mins % 10) + '0' ;
	        *bp = '\0' ;
	        rl = (bp - rbuf) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (zos_set) */

int zos_get(cchar *sp,int sl,int *zop) noex {
	int		rs = SR_FAULT ;
	if (sp && zop) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
		cchar	*cp{} ;
		bool	fneg = false ;
		if (int cl ; (cl = sfsign(sp,sl,&cp,&fneg)) > 0) {
		    cchar	*zp{} ;
		    if (int zl ; (zl = sfbrk(cp,cl," ,",&zp)) >= 3) {
			if (hasalldig(zp,zl)) {
	                    int	zoff ;
	                    int	sign = (fneg) ? 1 : -1 ; /* reverse */
	                    int	hours ;
	                    int	mins ;
	                    hours = (*zp++ - '0') ;
	                    if (zl > 3) {
	                        hours *= 10 ;
	                        hours += (*cp++ - '0') ;
	                    }
	                    mins = (*zp++ - '0') * 10 ;
	                    mins += (*zp++ - '0') ;
	                    zoff = (hours * 60) + mins ;
	                    zoff *= sign ;
	                    if (zop) {
		                *zop = zoff ;
	                    }
		            rs = (zp - sp) ;
			} /* end if (hasalldig) */
		    } /* end if (sfbrk) */
	        } /* end if (sfsign) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (zos_get) */


