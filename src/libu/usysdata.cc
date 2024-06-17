/* usysdata SUPPORT */
/* lang=C++23 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_uname

	Description:
	Retrieve some operating system and machine information.

	Synopsis:
	int u_uname(UTSNAME up) noex

	Arguments:
	up		UTSNAME object pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* <- for |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"usysdata.h"


/* local defines */


/* imported namespaces */

using namespace	usys ;			/* namespace */


/* local typedefs */

typedef int (*uname_f)(UTSNAME *) noex ;


/* external subroutines */


/* external variables */


/* local structuers */

namespace {
    struct umachiner {
	char	*mbuf ;
	int	mlen ;
    } ; /* end struct (umachiner) */
}


/* forward references */

static int uuname_machine(UTSNAME *) noex ;


/* local variables */

static constexpr uname_f	usubs[] = {
	uuname_machine,
	nullptr
} ;

#ifdef	COMMENT
static umachiner	um ;
#endif

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;


/* exported variables */


/* exported subroutines */

int u_uname(UTSNAME *up) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (up) {
	    if ((rs = uname(up)) >= 0) {
		rc = rs ;
		for (int i = 0 ; (rs >= 0) && usubs[i] ; i += 1) {
		    uname_f	func = usubs[i] ;
		    rs = func(up) ;
		} /* end for */
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (u_uname) */


/* local subroutines */

static int uuname_machine(UTSNAME *up) noex {
	cint		mlen = (sizeof(up->machine)-1) ;
	int		rs = SR_OK ;
	char		*mbuf = up->machine ;
	if (strcmp(mbuf,"x86_64") == 0) {
	    cchar	*sp = "Intel" ;
	    if_constexpr (f_darwin) {
	        sp = "Apple-Macintosh-Intel" ;
	    }
	    rs = sncpy(mbuf,mlen,sp) ;
	}
	return rs ;
}
/* end subroutine (uname_machine) */

#ifdef	COMMENT
static int uuname_architecture(UTSNAME *up) noex {
	cint		alen = (sizeof(up->architecture)-1) ;
	int		rs = SR_OK ;
	cchar		*abuf = up->architecture ;
	if_constexpr (f_darwin) {
	    if (strcmp(abuf,"i386") == 0) {
		cchar	*sp = "x86_64" ;
	        rs = sncpy(abuf,alen,sp) ;
	    }
	} /* end if_constexpr (f_darwin) */
	return rs ;
}
/* end subroutine (uname_architecture) */
#endif /* COMMENT */


