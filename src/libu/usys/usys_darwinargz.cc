/* usys_darwinargz SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve the first argument (ARGZ) of the current process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-11, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	darwin_argz
	getexecname

	Description:
	This subroutine returns the ARGZ element (if any) that
	was passed to the new process.

	Synopsis:
	int darwin_argz(char *abuf,int alen) noex

	Arguments:
	abuf		result buffer pointer
	alen		result buffer length

	Returns:
	-		the number of bytes in the result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* |getprogname(3c)| |getexecname(3c) */
#include	<cstring>		/* |strncpy(3c)| */
#include	<new>			/* C++STD plaecment-new */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_darwin.h"
#include	"usysargz.hh"		/* |usys:usysargz(3u)| */

import libutil ;			/* |lenstr(3u)| */

using usys::usysargz ;			/* type */

static usysargz		argz_data ;

namespace usys {
    int darwin_argz(char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (abuf) ylikely {
	    rs = SR_INVALID ;
	    if (alen > 0) ylikely {
	        if (static cint	rsa = argz_data ; (rs = rsa) >= 0) {
	            rs = argz_data.get(abuf,alen) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subrouine (darwin_argz) */
    int darwin_getargz(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rpp) ylikely {
	    if (static cint rsa = argz_data ; (rs = rsa) >= 0) {
	        *rpp = argz_data.name ;
		rs = lenstr(argz_data.name) ;
	    } else {
	        ulogerror("darwin_getargz",rs,"usysargz") ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subrouine (darwin_getargz) */
} /* end namespace (usys) */

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */


