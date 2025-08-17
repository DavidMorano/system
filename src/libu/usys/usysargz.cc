/* usysargz SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrievd the ARGZ value from the system for the current process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.	All rights reserved. */

/*******************************************************************************

	Object:
	usysargz
	getexecname

	Description:
	This object provides for retrieving the ARGZ value from
	the system for the current process.

	Synopsis:
	cchar *getexecname() noex

	Returns:
	-		the exec-name pointer or NULL (if not found)

	Notes:
	1. This subroutine helps (is itself a header object)
	facilitate retrieving or otherwise attempting to calculate
	the full path-name of the executable file of the current
	process.
	2. This object is both Thread-Safe and Fork-Safe.  The issue
	is when two threads call the two interfces (or each of the
	two threds call the same interface on the same instantiated
	object) to this object in a race situation.  We maintain
	some thread-safety by storing the lock in the instantiated
	object itself.  Further different threads calling simultaneously
	on differet object instantiations will not interfere since
	the whole object is really pure with respect to reentrancy
	and thread-safety.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getpid(2)| */
#include	<new>			/* |nothrow(3c)| */
#include	<atomic>		/* |atomic_flag(3c++)| */
#include	<mutex>			/* |call_once(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>		/* |libu::snwcpy(3u)| */
#include	<localmisc.h>

#include	"usys_pathpid.h"	/* |usys:usys_namepid(3usys)| */
#include	"usysargz.hh"

import libutil ;

#ifndef MAXPATH
#define MAXPATH		(4 * 1024)	/* resonable value for exec-name */
#endif

using libu::sncpy ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using std::call_once ;			/* subroutine */
using std::nothrow ;			/* constant */

cint		tlen = MAXPATH ;

static void oncefunc(usys::usysargz *op) noex {
	op->initonce() ;
}

namespace usys {
    usysargz::operator int () noex {
	call_once(ox,oncefunc,this) ;
	return rss ;
    } /* end method (usysargz::operator) */
    void usysargz::initonce() noex {
	cnothrow	nt{} ;
	cnullptr	np{} ;
	if ((rss >= 0) && (name == nullptr)) {
	    rss = SR_NOMEM ;
	    if (char *tbuf ; (tbuf = new(nt) char[tlen + 1]) != np) {
		const pid_t	    pid = getpid() ;
		if ((rss = usys_namepid(tbuf,tlen,pid)) >= 0) {
		    nlen = rss ;
		    rss = SR_NOMEM ;
		    if ((name = new(nt) char[nlen + 1]) != np) {
			rss = sncpy(name,nlen,tbuf) ;
		    } /* end if (new-name) */
		} /* end if (usys_namepid) */
		delete [] tbuf ;
	    } /* end if (new-delete) */
	} /* end if (was null) */
    } /* end method (usysargz::initonce) */
    int usysargz::get(char *rbuf,int rlen) noex {
	if ((rss >= 0) && name) {
	    rss = snwcpy(rbuf,rlen,name,nlen) ;
	}
	return rss ;
    } /* end method (usysargz::get) */
} /* end namespace (usys) */

namespace usys {
    void usysargz::dtor() noex {
	if (name) {
	    delete [] name ;
	    name = nullptr ;
	    nlen = 0 ;
	}
    } ; /* end struct (usysargz::dtor) */
} /* end namespace (usys) */


