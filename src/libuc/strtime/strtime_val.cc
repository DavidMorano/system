/* strtime_val SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* format a TIMVAL value to a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-27, David A­D­ Morano
	This is a replacement for some systems (UNIX yes, but not
	all others -- hence this code up) that do not have a
	|dirname(3c)| type of subroutine.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strtime_val

	Description:
	This subroutine formats a TIMEVAL value to a c-string.

	Synopsis:
	char *strtimeval(con TIMEBAL &tv,char *rbuf) noex

	Arguments:
	tv	TIMEBAL value to format
	rbuf	result buffer pointer (should be TIMEBUFLEN long)

	Returns:
	-	pointer to result buffer (if successful, otherwise NULL)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |ctdec(3u)| */
#include	<strbcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"strtime_val.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::ctdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern char *strtime_log(time_t,char *) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rlen	= TIMEBUFLEN ;
constexpr int		tlen	= TIMEBUFLEN ;
constexpr int		dlen	= DECBUFLEN ;


/* exported variables */


/* exported subroutines */

char *strtime_val(CTIMEVAL &tv,char *rbuf) noex {
    	cnullptr	np{} ;
    	char		*rp = nullptr ;
	if (rbuf) ylikely {
	    custime	t = tv.tv_sec ;
	    suseconds_t	us = tv.tv_usec ;
	    rbuf[0] = '\0' ;
	    if (char tbuf[tlen +1] ; strtime_log(t,tbuf) != np) ylikely {
		cint ms = (us / 1000) ;
		if (char dbuf[dlen +1] ; ctdec(dbuf,dlen,ms) >= 0) ylikely {
		    if (strbcpy(rbuf,rlen,tbuf,"_",dbuf,"ms") != np) {
		        rp = rbuf ;
		    }
		} /* end if (ctdec) */
	    } /* end if (strtime_log) */
	} /* end if (non-null) */
    	return rp ;
} /* end subroutine (strtime_val) */


