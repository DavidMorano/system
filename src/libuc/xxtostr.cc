/* xxtostr */
/* lang=C++20 */

/* subroutines to convert an integer to a sring (base-10) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctdecxx

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a decimal string that is placed into the caller supplied
	buffer (of specified length).

	Synopsis:
	template<typename T,typename UT>
	char *uxxtostr(char **epp,int b,UT,T v) noex

	Arguments:
	endp		pointer to one past the end of the supplied buffer
	base		number base
	v		integer value to be converted
	epp		pointer to passed endp

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion

	Notes:
	As it stands now, these subroutines do not perform any funny
	business in trying to make this process faster! These
	subroutines are, therefore, probably the slowest such
	conversions routinely available. To really move (execute)
	quickly through the division-related aspects of the require
	algorithm, one would have to use assembly language where
	both the quotient and the reminder of a division are produced
	simultaneously (since each are needed to continue). This,
	of course, assumes that the underlying machine architecture
	has such instructions. But short of assembly (and and the
	required machine instructions) this present implemtnation
	is adequate.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"xxtostr.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */

constexpr int		b = 10 ;


/* local subroutine-templates */


/* exported subroutines */

char *itostr(int v,char *endp) noex {
	const uint	uv = 0 ;
	int		rs ;
	char		*rp = nullptr ;
	if ((rs = sxxtostr(endp,b,uv,v)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}

char *ltostr(long v,char *endp) noex {
	const ulong	uv = 0 ;
	int		rs ;
	char		*rp = nullptr ;
	if ((rs = sxxtostr(endp,b,uv,v)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}

char *lltostr(longlong v,char *endp) noex {
	const ulonglong	uv = 0 ;
	int		rs ;
	char		*rp = nullptr ;
	if ((rs = sxxtostr(endp,b,uv,v)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}

char *uitostr(uint uv,char *endp) noex {
	char		*rp = nullptr ;
	int		rs ;
	if ((rs = uxxtostr(endp,b,uv)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}

char *ultostr(ulong uv,char *endp) noex {
	char		*rp = nullptr ;
	int		rs ;
	if ((rs = uxxtostr(endp,b,uv)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}

char *ulltostr(ulonglong uv,char *endp) noex {
	char		*rp = nullptr ;
	int		rs ;
	if ((rs = uxxtostr(endp,b,uv)) >= 0) {
	    rp = (endp-rs) ;
	} else {
	    errno = (-rs) ;
	}
	return (rs >= 0) ? rp : nullptr ;
}


