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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>

#include	"zos.h"


/* local defines */


/* external subroutines */


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
	int		cl ;
	int		zoff ;
	int		ch = 0 ; /* ¥ GCC false complaint */
	cchar		*cp ;
	    rs = SR_INVALID ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl) ch = mkchar(*sp) ;
	if ((sl >= 2) && ((ch == '-') || (ch == '+') || isdigitlatin(ch))) {
	    int		i, sign ;
	    int		hours, mins ;

	    rs = SR_OK ;
	    sign = ((ch == '+') || isdigitlatin(ch)) ? -1 : 1 ;

	    cp = sp ;
	    cl = sl ;
	    if ((*sp == '-') || (*sp == '+')) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    for (i = 0 ; 
	        (i < cl) && cp[i] && 
	        (! CHAR_ISWHITE(cp[i])) && (cp[i] != ',') ; 
	        i += 1) {
		cint	ch = mkchar(cp[i]) ;
	        if (! isdigitlatin(ch)) {
	            rs = SR_INVALID ;
		    break ;
	        }

	    } /* end for (extra sanity check) */

/* skip over extra leading digits (usually '0' but whatever) */

	    if (i > 4) {
	        cp += (i - 4) ;
	        cl -= (i - 4) ;
	    }

/* extract hours and minutes from remaining 4 digits */

	    hours = (*cp++ - '0') ;
	    if (cl > 3) {
	        hours *= 10 ;
	        hours += (*cp++ - '0') ;
	    }

	    mins = (*cp++ - '0') * 10 ;
	    mins += (*cp++ - '0') ;

	    zoff = (hours * 60) + mins ;

/* reportedly, there are time zones at up to 14 hours off of GMT! */

#ifdef	COMMENT
	    if (zoff > (14 * 60))
		rs = SR_INVALID ;
#endif

	    zoff *= sign ;
	    if (zop) {
		*zop = zoff ;
	}

	    if (rs >= 0) {
		rs = (cp - sp) ;
	    }

	} /* end if (getting timezone offset) */

	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (zos_get) */


