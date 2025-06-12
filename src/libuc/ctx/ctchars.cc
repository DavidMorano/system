/* ctchars SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutines to convert an integer to a decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-14, David A­D­ Morano
	I wrote this just to have an extra value-digit conversion 
	facility available.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctcharsxx

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a decimal string that is placed into the caller supplied
	buffer (of specified length).

	Synopsis:
	int ctcharsXX(char *rbuf,int rlen,XX v) noex

	Arguments:
	rbuf		caller supplied buffer
	rlen		caller supplied buffer length
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion

	Notes:
	As it stands now, these subroutines do not perform any funny
	business in trying to make this process faster! These
	subroutines are, therefore, probably the slowest such
	conversions routinely available.  To really move (execute)
	quickly through the division-related aspects of the require
	algorithm, one would have to use assembly language where
	both the quotient and the reminder of a division are produced
	simultaneously (since each are needed to continue).  This,
	of course, assumes that the underlying machine architecture
	has such instructions.  But short of assembly (and and the
	required machine instructions) this present implemtnation
	is adequate.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<charconv>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sncpyx.h>

#include	"ctchars.h"


/* local defines */


/* imported namespaces */

using std::to_chars ;
using std::to_chars_result ;
using std::errc ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
static constexpr int ctcharsx(char *dbuf,int dlen,int base,T &v) noex {
	to_chars_result	r ;
	constexpr errc	ec_ok = errc() ;
	int		rs = SR_OK ;
	char		*ep = (dbuf + dlen) ;
	dbuf[0] = '\0' ;
	if (r = to_chars(dbuf,ep,v,base) ; (r.ec == ec_ok)) {
	    *r.ptr = '\0' ;
	} else {
	    using enum std::errc ;
	    dbuf[0] = '\0' ;
	    switch (r.ec) {
	    case value_too_large:
		rs = SR_OVERFLOW ;
		break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	} /* end if (handle errors) */
	return rs ;
}
/* end subroutine-template (ctcharsx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int ctcharsi(char *dp,int dl,int b,int v) noex {
	return ctcharsx(dp,dl,b,v) ;
}

int ctcharsl(char *dp,int dl,int b,long v) noex {
	return ctcharsx(dp,dl,b,v) ;
}

int ctcharsll(char *dp,int dl,int b,longlong v) noex {
	return ctcharsx(dp,dl,b,v) ;
}

int ctcharsui(char *dp,int dl,int b,uint v) noex {
	return ctcharsx(dp,dl,b,v) ;
}

int ctcharsul(char *dp,int dl,int b,ulong v) noex {
	return ctcharsx(dp,dl,b,v) ;
}

int ctcharsull(char *dp,int dl,int b,ulonglong v) noex {
	return ctcharsx(dp,dl,b,v) ;
}


