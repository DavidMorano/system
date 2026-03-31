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
	ctdec

	Description:
	Convert a binary integer to its decimal representation.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<xxtostr.h>		/* |{x}tostr(3u)| */
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usupport_sncpyx.h"
#include	"usupport_ctdec.h"


/* local defines */


/* imported namespaces */

using libu::sncpy ;			/* subroutine */


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
    local int ctdecx(charp (*ctx)(T,char *),char *dp,int dl,T uv) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	dlen = DECBUFLEN ;
	    char	dbuf[DECBUFLEN + 1] ;
	    char	*bp = ctx(uv,(dbuf + dlen)) ;
	    rs = sncpy(dp,dl,bp) ;
	}
	return rs ;
    } /* end subroutine-template (ctdecx) */
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


