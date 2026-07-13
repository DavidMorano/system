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
#include	<climits>		/* CSTD |INT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"usys_strtox.h"

import bitmanip ;

#ifndef	SUBROUTINE_STRTOX
#define	SUBROUTINE_STRTOX
sint	strtosi(cchar *s,char **endpp,int b) noex {
    	sint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
	        clong	v = strtol(s,endpp,b) ;
	        if (errno == 0) {
	            ulong	uv = ulong(v) ;
	            cint	nb = nbits<long> ;
		    res = intconv(v) ;
	            {
	                cbool	fneg = bit(v,(nb - 1)) ;
		        if (fneg) {	/* test negative value */
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
	            } /* end block */
	        } /* end if (no error so far) */
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return res ;
} /* end subroutine (strtosi) */
uint	strtoui(cchar *s,char **endpp,int b) noex {
    	uint		res = 0 ; /* return-value */
	errno_t		ec = 0 ;
	if (s) {
	    if (s[0]) {
	        con ulong resl = strtoul(s,endpp,b) ;
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
} /* end subroutine (strtoui) */
#endif /* SUBROUTINE_STRTOX */


