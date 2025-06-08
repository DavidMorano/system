/* cfcharsx MODULE (primary-module-interface) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-10, David A-D- Morano
	This is originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2017,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cfcharsx

	Description:
	This subroutine-template is used to convert digit c-strings
	to binary integers.  This object uses the uderlying (system
	supplied) digit conversion functions to implement its
	interface.  Note that the (so-called) system supplied
	conversion facilities are (clandestinely) supplemented to
	support an interger type of 128-bits (a |longlong| type).
	But the (so-called) system does not generally support integer
	types larger than 128 bits.  So this subroutine-template
	|cfcharx(3uc)| also does not support integer types larger
	than 128-bits.  Also, only digit number bases of omly a
	maximum of 36 is supported.  So any number bases larger
	than 36 require a differnt conversion facility, like for
	example |cfdigx(3uc)|.

	Synopsis:
    	template<typename T> 
	inline int cfcharsx(cc *sp,int sl,int b,T *rp) noex

	Arguments:
	sp		c-string of digits to convert pointer
	sl		c-string of digits to convert length
	b		number base
	rp		pointer to resulting value (of given type)

	Returns:
	>=0		OK
	<0		error (system-return)

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
#include	<cfutil.hh>		/* namespace |cfx| + |cstrlen(3u)| */
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


