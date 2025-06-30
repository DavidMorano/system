/* usupport_cfdec SUPPORT */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<intsat.h>
#include	<stdintx.h>
#include	<strtox.h>
#include	<strnul.hh>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usupport_cfdec.h"


/* local defines */


/* imported namespaces */


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
    static int cfdecx(T (*cfx)(cc *,char **,int),cc *sp,int sl,T *rp) noex {
	[[maybe_unused]] char	*endp{} ; /* <- unused */
	cint		b = 10 ;
	int		rs = SR_FAULT ;
	if (sp) {
	    T		v{} ;
	    strnul	str(sp,sl) ;
	    errno = 0 ;
	    v = cfx(str,&endp,b) ;
	    if (rp) *rp = v ;
	    if (errno) {
		rs = (- errno) ;
	    } else {
		rs = intsat(v) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfdecx) */
    int cfdec(cchar *sp,int sl,int *rp) noex {
	return cfdecx(strtoxi,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,long *rp) noex {
	return cfdecx(strtoxl,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,longlong *rp) noex {
	return cfdecx(strtoxll,sp,sl,rp) ;
    }
} /* end namespace (libu) */


/* local subroutines */


