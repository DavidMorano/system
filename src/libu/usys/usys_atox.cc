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

#include	"usys_strtox.h"
#include	"usys_atox.h"


constexpr int		base10 = 10 ;	/* base-10 */

sint	atosi(cchar *s) noex {
    	sint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    res = atoi(s) ;
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atosi) */

sint	atosi(cchar *s,int b) noex {
    	return strtosi(s,nullptr,b) ;
} /* end subroutine (atosi) */

uint	atoui(cchar *s) noex {
    	uint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    con ulong resl = strtoul(s,nullptr,base10) ;
	    if (errno == 0) {
	        if ((resl >> (szof(uint) * CHAR_BIT)) == 0L) {
	    	    res = conv<uint>(resl) ;
		} else {
		    res = UINT_MAX ;
		    ec = ERANGE ;
	        }
	    } else {
	        res = UINT_MAX ;
	    } /* end if (not error) */
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atoui) */

uint	atoui(cchar *s,int b) noex {
    	return strtoui(s,nullptr,b) ;
} /* end subroutine (atoui) */

slong	atosl(cchar *s) noex {
    	slong		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    res = atol(s) ;
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atosl) */

slong	atosl(cchar *s,int b) noex {
    	return strtosl(s,nullptr,b) ;
} /* end subroutine (atosl) */

ulong	atoul(cchar *s) noex {
    	return atoul(s,base10) ;
} /* end subroutine (atoul) */

ulong	atoul(cchar *s,int b) noex {
    	ulong		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    res = strtoul(s,nullptr,b) ;
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (atoul) */


