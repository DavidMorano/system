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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>
#include	<xxtostr.h>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usupport_sncpyx.hh"
#include	"usupport_ctdec.hh"

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
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (ctdecx) */
    int ctdeci		(char *dp,int dl,int uv)	noex {
	return ctdecx(itostr,dp,dl,uv) ;
    }
    int ctdecl		(char *dp,int dl,long uv)	noex {
	return ctdecx(ltostr,dp,dl,uv) ;
    }
    int ctdecll		(char *dp,int dl,longlong uv)	noex {
	return ctdecx(lltostr,dp,dl,uv) ;
    }
    int ctdecui		(char *dp,int dl,uint uv)	noex {
	return ctdecx(uitostr,dp,dl,uv) ;
    }
    int ctdecul		(char *dp,int dl,ulong uv)	noex {
	return ctdecx(ultostr,dp,dl,uv) ;
    }
    int ctdecull	(char *dp,int dl,ulonglong uv)	noex {
	return ctdecx(ulltostr,dp,dl,uv) ;
    }
    int ctdec_unknown	(char *dp,int dl)		noex {
	return sncpy(dp,dl,"unknown") ;
    }
} /* end namespace (libu) */


/* local subroutines */


