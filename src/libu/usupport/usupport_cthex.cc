/* usupport_cthex SUPPORT */
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
	cthex

	Description:
	Convert a binary integer to its hexadecimal representation.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<xxtostr.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |HEXBUFLEN| */

#include	"usupport_sncpyx.hh"
#include	"usupport_cthex.hh"

/* local defines */


/* imported namespaces */

using libu::sncpy ;			/* subroutine */


/* local typedefs */

template<typename T>
using ctx_f = charp (*)(T,char *,int) noex ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		b = 16 ;	/* base-16 */


/* exported variables */


/* exported subroutines */

namespace libu {
    template<typename T>
    local int cthexx(ctx_f<T> ctx,char *dp,int dl,T uv) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	dlen = HEXBUFLEN ;
	    char	dbuf[HEXBUFLEN + 1] ;
	    char	*bp = ctx(uv,(dbuf + dlen),b) ;
	    rs = sncpy(dp,dl,bp) ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cthexx) */
    int cthexi		(char *dp,int dl,int uv)	noex {
	return cthexx(itostr,dp,dl,uv) ;
    }
    int cthexl		(char *dp,int dl,long uv)	noex {
	return cthexx(ltostr,dp,dl,uv) ;
    }
    int cthexll		(char *dp,int dl,longlong uv)	noex {
	return cthexx(lltostr,dp,dl,uv) ;
    }
    int cthexui		(char *dp,int dl,uint uv)	noex {
	return cthexx(uitostr,dp,dl,uv) ;
    }
    int cthexul		(char *dp,int dl,ulong uv)	noex {
	return cthexx(ultostr,dp,dl,uv) ;
    }
    int cthexull	(char *dp,int dl,ulonglong uv)	noex {
	return cthexx(ulltostr,dp,dl,uv) ;
    }
    int cthex_unknown	(char *dp,int dl)		noex {
	return sncpy(dp,dl,"unknown") ;
    }
} /* end namespace (libu) */


/* local subroutines */


