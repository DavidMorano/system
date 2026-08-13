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
	cfdec

	Description:
	Convert from a decimal representation to its binary integer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<strtox.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */

#include	"usupport_cfdec.hh"


/* local defines */

#define	CFBASE		10		/* Convert-From Base */


/* imported namespaces */


/* local typedefs */

template<typename T>
using cfx_f = T (*)(cc *,char **,int) noex ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    template<typename T>
    local int cfdecx(cfx_f<T> cfx,cc *sp,int sl,T *rp) noex {
	[[maybe_unused]] char	*endp{} ; /* <- unused */
	cint		b = CFBASE ;	/* conversion number base */
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    T		v{} ;
	    strnul	str(sp,sl) ;
	    errno = 0 ;
	    v = cfx(str,&endp,b) ;
	    if (rp) *rp = v ;
	    if (errno) {
		rs = (neg errno) ;
	    } else {
		rs = intsat(v) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfdecx) */
} /* end namespace (libu) */

namespace libu {
    int cfdec(cchar *sp,int sl,int *rp)		noex {
	return cfdecx(strtoxsi,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,long *rp)	noex {
	return cfdecx(strtoxsl,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,longlong *rp)	noex {
	return cfdecx(strtoxsll,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,uint *rp)	noex {
	return cfdecx(strtoxui,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,ulong *rp)	noex {
	return cfdecx(strtoxul,sp,sl,rp) ;
    }
    int cfdec(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfdecx(strtoxull,sp,sl,rp) ;
    }
} /* end namespace (libu) */


/* local subroutines */


