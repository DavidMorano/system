/* usupport_ctdec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	msleep

	Description:
	This subroutine sleeps for some number of milliseconds.

	Synopsis:
	int msleep(int msec) noex

	Arguments:
	msec		number of millisecond to sleep

	Returns:
	>=0		amount of data returned
	<0		error (system-return)


	Name:
	umtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |gethostid(3c)| */
#include	<poll.h>
#include	<cerrno>
#include	<ctime>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |memset(3c)| + |strlcpy(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<intsat.h>
#include	<stdintx.h>
#include	<xxtostr.h>		/* |{x}tostr(3u)| */
#include	<strtox.h>
#include	<strnul.hh>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usupport.h"


/* local defines */


/* imported namespaces */

using libu::sncpy ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    template<typename T>
    static int ctdecx(charp (*ctx)(T,char *),char *dp,int dl,T uv) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	dlen = DECBUFLEN ;
	    char	dbuf[DECBUFLEN + 1] ;
	    char	*bp = ctx(uv,(dbuf + dlen)) ;
	    rs = sncpy(dp,dl,bp) ;
	}
	return rs ;
    }
    int ctdecui(char *dp,int dl,uint uv) noex {
	return ctdecx(uitostr,dp,dl,uv) ;
    }
    int ctdecul(char *dp,int dl,ulong uv) noex {
	return ctdecx(ultostr,dp,dl,uv) ;
    }
    int ctdecull(char *dp,int dl,ulonglong uv) noex {
	return ctdecx(ulltostr,dp,dl,uv) ;
    }
} /* end namespace (libu) */


/* local subroutines */


