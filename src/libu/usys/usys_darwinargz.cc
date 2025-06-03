/* usys_darwinargz SUPPORT */
/* encoding=ISO8859-1 */
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

#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getprogname(3c)| |getexecname(3c) */
#include	<cstring>		/* |strncpy(3c)| */
#include	<new>			/* |nothrow(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"usys_darwin.h"
#include	"usysargz.hh"		/* |usys:usysargz(3u)| */

using usys::usysargz ;			/* type */
using std::nothrow ;			/* constant */

static usysargz		argz_data ;

namespace usys {
    extern int usys_darwinargz(char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (abuf) {
	    rs = SR_INVALID ;
	    if (alen > 0) {
	        static cint	rsa = argz_data ;
	        if ((rs = rsa) >= 0) {
	            rs = argz_data.get(abuf,alen) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subrouine (usys_darwinargz) */
    cchar *darwin_getargz() noex {
	static cint	rsa = argz_data ;
	cchar		*rp = nullptr ;
	if (int rs ; (rs = rsa) >= 0) {
	    rp = argz_data.name ;
	} else {
	    ulogerror("darwin_getargz",rs,"usysargz") ;
	}
	return rp ;
    } /* end subrouine (darwin_getargz) */
} /* end namespace (usys) */

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */


