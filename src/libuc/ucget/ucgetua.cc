/* ucgetua SUPPORT */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

	= 2017-05-17, David A-D- Morano
	I did some update to use C++11 ('if constexpr').

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	These subroutines were written so that we could use a single
	interface to access the 'group' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'group' database access subroutines.

	Note:
	1. Coding note: On some platforms, the non-reentrant versions
	of these subroutines do NOT set 'errno'.
	2. I made no special handling for those platforms that do
	not support reentrant variations for one or more of these
	subroutines. I otherwise I could have put a mutex lock
	around everything to get the same behavior as on other
	(minimally sane) platforms. I will not name names (messed-up
	Darwin) so as not to offend any particular operation system.
	But, YES, curse those operating systems that did not have
	the minimal decency to put mutex locks around all of these
	subroutines within their standard-C library.  In a parting
	thought: curse Apple Darwin.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"userattr.h"
#include	"ucgetua.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getuabegin() noex {
	return uc_userattrbegin() ;
}
/* end subroutine (uc_getuabegin) */

int uc_getuaend() noex {
	return uc_userattrend() ;
}
/* end subroutine (uc_getuaend) */

int uc_getuaent(ucentua *uap,char *uabuf,int ualen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		size = 0 ;
	if (uap && uabuf) {
	    rs = SR_INVALID ;
	    if (ualen > 0) {
		userattr	*ep{} ;
		if ((rs = uc_userattrent(&ep)) >= 0) {
		    {
			ucentua	*oep = static_cast<ucentua *>(ep) ;
			rs = uap->load(uabuf,ualen,oep) ;
			size = rs ;
		    }
		    rs1 = uc_userattrfree(ep) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (uc_getuserattrent) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? size : rs ;
}
/* end subroutine (uc_getuaent) */

int uc_getuanam(ucentua *uap,char *uabuf,int ualen,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		size = 0 ;
	if (uap && uabuf && name) {
	    rs = SR_INVALID ;
	    if (ualen > 0) {
		userattr	*ep{} ;
		if ((rs = uc_userattrnam(&ep,name)) >= 0) {
		    {
			ucentua	*oep = static_cast<ucentua *>(ep) ;
			rs = uap->load(uabuf,ualen,oep) ;
			size = rs ;
		    }
		    rs1 = uc_userattrfree(ep) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (uc_userattrname) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? size : rs ;
}
/* end subroutine (uc_getuanam) */

int uc_getuauid(ucentua *uap,char *uabuf,int ualen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		size = 0 ;
	if (uap && uabuf) {
	    rs = SR_INVALID ;
	    if (ualen > 0) {
		userattr	*ep{} ;
		if ((rs = uc_userattruid(&ep,uid)) >= 0) {
		    {
			ucentua	*oep = static_cast<ucentua *>(ep) ;
			rs = uap->load(uabuf,ualen,oep) ;
			size = rs ;
		    }
		    rs1 = uc_userattrfree(ep) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (uc_getuserattr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? size : rs ;
}
/* end subroutine (uc_getuauid) */


