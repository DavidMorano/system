/* usys_atox SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |UINT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_atox.h"


constexpr int		base10 = 10 ;	/* base-10 */

sint	atosi(cchar *s) noex {
	return atoi(s) ;
} /* end subroutine (atosi) */

uint	atoui(cchar *s) noex {
    	uint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
	        con ulong resl = strtoul(s,nullptr,base10) ;
		if ((resl >> (szof(uint) * CHAR_BIT)) == 0L) {
	            res = uintconv(resl) ;
		} else {
		    ec = ERANGE ;
		}
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atoui) */

slong	atosl(cchar *s) noex {
	return atol(s) ;
} /* end subroutine (atosl) */

ulong	atoul(cchar *s) noex {
    	ulong		res = 0 ;
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
	        res = strtoul(s,nullptr,base10) ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atoul) */


