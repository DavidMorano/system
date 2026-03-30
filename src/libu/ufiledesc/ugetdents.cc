/* ugetdents SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_getdents

	Description:
	This subroutine is provided by some operating systems (like
	Solaris®, IllumOs®, and others) to retieve directory entries
	given a file-descriptor open on a directory. Not all operating
	systems support the used of this subroutine when the calling
	process is running in the ULP32 execution model (I will
	give no names at this time: Apple Darwin). So the use of
	this subroutine on wirdo systems (like Apple Darwin) should
	be avoided.

	Synopsis:
	int u_getdents(int fd,void *dbuf,int dsz) noex

	Returns:
	>=0		size of data returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<dirent.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"ufiledescbase.hh"
#include	"ugetdents.h"


/* local defines */


/* imported namespaces */

using namespace usys ;			/* namespace */

using libu::ufiledescbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct udirop ;
    typedef int (udirop::*udirop_m)(int) noex ;
    struct udirop : ufiledescbase {
	udirop_m	m ;
	void		*dbuf ;
	int		dlen ;
	udirop(udirop_m µm,void *b,int l) noex : m(µm) { 
	    dbuf = b ;
	    dlen = l ;
	} ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	sysret_t igetdents(int) noex ;
    } ; /* end struct (udirop) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_getdents(int fd,void *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
		udirop diro(&udirop::igetdents,dbuf,dlen) ;
		rs = diro(fd) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_getdents) */


/* local subroutines */

sysret_t udirop::igetdents(int fd) noex {
	return ugetdents(fd,dbuf,dlen) ;
}


