/* ucuserattr SUPPORT */
/* lang=C++20 */

/* deal with user-attributes (appears in coming version of Solaris®) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module supplies the following subroutines for accessing
	the system user-attribute database:

		uc_userattrbegin()
		uc_userattrend()
		uc_userattrent()
		uc_userattrnam()
		uc_userattrfree()

	These subroutines were grouped together into a single module
	(not at all typical) because they have to be used together,
	since using them apart makes little sense.

	Note that these stupid subroutines are thread-safe but are
	not reentrant!  Yes, that is what you get to when you have
	extreme brain damaged system developers.

	Implementation-note: These function, except for the first
	two below, do not use or set the ERRNO variable. They just
	return a NULL pointer or a non-NULL pointer, indicating
	failure or success.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<numsign.h>
#include	<localmisc.h>

#include	"userattr.h"		/* <- the money shot */
#include	"ucentua.h"
#include	"ucuserattr.h"


/* local defines */

#if	defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0)
#define	F_USERATTR	1
#else
#define	F_USERATTR	0
#endif /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */


/* local variables */

constexpr bool		f_userattr = F_USERATTR ;


/* exported subroutines */

int uc_userattrbegin() noex {
	int		rs = SR_NOSYS ;
	if constexpr (f_userattr) {
	    errno = 0 ;
	    setuserattr() ;
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_userattrbegin) */

int uc_userattrend() noex {
	int		rs = SR_NOSYS ;
	if constexpr (f_userattr) {
	    errno = 0 ;
	    enduserattr() ;
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_userattrend) */

int uc_userattrent(userattr **rpp) noex {
	int		rs = SR_FAULT ;
	if (rpp) {
	    userattr	*uap = nullptr ;
	    rs = SR_NOSYS ;
	    if constexpr (f_userattr) {
	        uap = getuserattr() ;
	        if (uap) {
		    ucentua	*ep = static_cast<ucentua *>(uap) ;
		    rs = ep->size() ;
	        }
	    } /* end if-constexpr (f_userattr) */
	    *rpp = (rs >= 0) ? uap : nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_userattrent) */

int uc_userattrnam(userattr **rpp,cchar *un) noex {
	int		rs = SR_FAULT ;
	if (rpp && un) {
	    userattr	*uap = nullptr ;
	    rs = SR_INVALID ;
	    if (un[0]) {
		rs = SR_NOSYS ;
	        if constexpr (f_userattr) {
		    rs = SR_NOTFOUND ;
	            uap = getusernam(un) ;
	            if (uap) {
		    	ucentua	*ep = static_cast<ucentua *>(uap) ;
		        rs = ep->size() ;
	            }
	 	} /* end if-constexpr (f_userattr) */
	    } /* end if (valid) */
	    *rpp = (rs >= 0) ? uap : nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_userattrnam) */

int uc_userattruid(userattr **rpp,uid_t uid) noex {
	int		rs = SR_FAULT ;
	if (rpp) {
	    userattr	*uap = nullptr ;
	    rs = SR_INVALID ;
	    if (!numsign(uid)) {
		rs = SR_NOSYS ;
	        if constexpr (f_userattr) {
		    rs = SR_NOTFOUND ;
	    	    uap = getuseruid(uid) ;
	            if (uap) {
		        ucentua	*ep = static_cast<ucentua *>(uap) ;
		        rs = ep->size() ;
	            }
		} /* end if-constexpr (f_userattr) */
	    } /* end if (valid) */
	    *rpp = (rs >= 0) ? uap : nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_userattruid) */

int uc_userattrfree(userattr *uap) noex {
	int		rs = SR_FAULT ;
	if (uap) {
	    if constexpr (f_userattr) {
	        rs = SR_OK ;
	        free_userattr(uap) ;
	    } else {
	        rs = SR_NOSYS ;
	    } /* end if-constexpr (f_userattr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_userattrfree) */


