/* usys_strtox SUPPORT */
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
#include	<numeric>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strtox.h"

template<typename T>
constexpr int nbits = (szof(T) * CHAR_BIT) ;

template<typename T> constexpr inline bool bit(T v,int n) noex {
	return !!((v >> n) & 1) ;
} /* end subroutine (bit) */

sint	strtosi(cchar *s,char **epp,int b) noex {
	cint		nb = nbits<long> ;
    	sint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	bool		fneg ;
    	if (s) {
	    clong	v = strtol(s,epp,b) ;
	    fneg = bit(v,(nb - 1)) ;
	    if (errno == 0) {
	        ulong	uv = conv<ulong>(v) ;
		res = conv<sint>(v) ;
	        if (fneg) {
		    uv = (compl uv) ;
	            uv >>= (nb / 2) ;
		    if (uv || (! bit(v,((nb / 2) - 1)))) {
			res = INT_MIN ;
			ec = ERANGE ;
		    }
		} else {	/* test poitive value */
	            uv >>= (nb / 2) ;
		    if (uv || bit(v,((nb / 2) - 1))) {
			res = INT_MAX ;
			ec = ERANGE ;
		    }
		} /* end if */
	    } else {
		if (fneg) {
	            res = UINT_MIN ;
		} else {
	            res = UINT_MAX ;
		}
	    } /* end if */
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (strtosi) */

uint	strtoui(cchar *s,char **epp,int b) noex {
    	uint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    con ulong resl = strtoul(s,epp,b) ;
	    if (errno == 0) {
	        if ((resl >> (szof(uint) * CHAR_BIT)) == 0L) {
	            res = conv<uint>(resl) ;
		} else {
		    res = UINT_MAX ;
		    ec = ERANGE ;
	        } /* end if */
	    } else {
	        res = UINT_MAX ;
	    } /* end if (errno) */
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (strtoui) */

slong	strtosl(cchar *s,char **epp,int b) noex {
    	slong		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    res = strtol(s,epp,b) ;
	} else {
	    ec = EFAULT ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (strtosl) */


