/* cfcharsx MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10,10, David A-D- Morano
	This is originally written.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cfcharsx

	Description:
	Subroutines to convert digit strings to binary integers.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<charconv>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>		/* extended integer types */
#include	<cfutil.h>		/* |cstrlen(3u)| */
#include	<localmisc.h>

export module cfcharsx ;

template<typename T> inline int cvt(cc *sp,int sl,int b,T *rp) noex {
	using std::from_chars ;			/* subroutine */
	using std::errc ;			/* enumeration */
	using res = std::from_chars_result ;	/* type */
	int		rs = SR_FAULT ;
	if (sp) {
	    constexpr errc	ec_ok = errc() ;	/* first value */
	    T		v{} ;
	    rs = SR_OK ;
	    if (res r = from_chars(sp,(sp+sl),v,b) ; (r.ec == ec_ok)) {
		*rp = v ;
	    } else {
		using enum	std::errc ;
		switch (r.ec) {
		case result_out_of_range:
		    rs = SR_RANGE ;
		    break ;
		case invalid_argument:
		    rs = SR_INVALID ;
		    break ;
		default:
		    rs = SR_BADFMT ;
		    break ;
		} /* end switch */
	    } /* end if (failed) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine-template (cvt) */

export {
    template<typename T> inline int cfcharsx(cc *sp,int sl,int b,T *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = cfx::sfchars(sp,sl,&cp)) > 0) {
		rs = cvt(cp,cl,b,rp) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine-template (cfcharsx) */
} /* end export */


