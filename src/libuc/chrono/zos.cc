/* zos SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* time-zone offset string management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
	rbuf		result buffer pointer
	rlen		result buffer length
	zo		the time-zone offset to use in creating string

	Returns:
	>=0		length of created string
	<0		error code (system-return)


	Name:
	zos_get

	Description:
	This subroutine extracts a zone-offset integer value from
	a zone-offset string.  The value returns is the minutes west
	of GMT.  If the zone-offset was eastwards of GMT, the value
	return is negative.  In all cases, the value returns is
	in MINUYES.

	Synopsis:
	int zos_get(cchar *zbuf,int zlen,int *zop) noex

	Arguments:
	zbuf		source string pointer
	zlen		source string length
	zop		pointer to store result (offset in SECONDS)

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |abs(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<sfx.h>			/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"zos.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int decval(int ch) noex {
    	return (ch - '0') ;
} /* end if (decval) */


/* local variables */

cint		base10		= 10 ;
cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

/* 'zo' in minutes west of GMT */
int zos_set(char *rbuf,int rlen,int zo) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (rlen <= 0) ylikely {
	        int	hours, mins ;
	        char	*bp = rbuf ;
		rs = SR_OK ;
	        hours = abs(zo / 60) % 100 ;
	        mins = abs(zo % 60) ;
	        *bp++ = ((zo >= 0) ? '-' : '+') ;
	        *bp++ = charconv((hours / base10) + '0') ;
	        *bp++ = charconv((hours % base10) + '0') ;
	        *bp++ = charconv((mins / base10) + '0') ;
	        *bp++ = charconv((mins % base10) + '0') ;
	        *bp = '\0' ;
	        rl = intconv(bp - rbuf) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (zos_set) */

int zos_get(cchar *sp,int sl,int *zop) noex {
	int		rs = SR_FAULT ;
	DPRINTF("ent\n") ;
	if_constexpr (f_debug) {
	    strnul ds(sp,sl) ;
	    DPRINTF("zbuf=>%s<\n",ccp(ds)) ;
	}
	if (sp && zop) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
		cchar	*cp{} ;
		bool	fneg = false ;
		if (int cl ; (cl = sfsign(sp,sl,&cp,&fneg)) > 0) ylikely {
		    cchar	*zp{} ;
		    if (int zl ; (zl = sfnextchr(cp,cl,',',&zp)) >= 3) ylikely {
			if (hasalldig(zp,zl)) ylikely {
	                    int	zoff ;
	                    int	sign = (fneg) ? 1 : -1 ; /* reverse */
	                    int	hours = decval(*zp++) ;
	                    int	mins = 0 ;
	                    if (zl > 3) {
	                        hours *= base10 ;
	                        hours += decval(*zp++) ;
	                    } /* end if (more than three characters) */
	                    mins += decval(*zp++) * base10 ;
	                    mins += decval(*zp++) ;
	                    zoff = (hours * 60) + mins ;
	                    zoff *= sign ;
			    {
		                *zop = zoff ;
			    }
		            rs = intconv(zp - sp) ;
			} /* end if (hasalldig) */
		    } /* end if (sfbnextchr) */
	        } /* end if (sfsign) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (zos_get) */


