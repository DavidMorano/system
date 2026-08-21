/* usys_project SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support for the UNIX® Shadow-Password facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usys_project

	Description:
	These subroutines offer support for the UNIX® Project
	facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |getpid(2)| */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* |getenv(3c)| + |getprogname(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"usys_project.h"


#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
/******************************************************************************/


/* *nothing* */


/******************************************************************************/
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */
/******************************************************************************/

projid_t getprojid() noex {
    	projid_t	id = 0 ;
	return id ;
} /* end subroutine (getprojid) */

projid_t getprojidbyname(cchar *name) noex {
    	projid_t	id = 0 ;
	errno_t		ec = 0 ;
	if (name) {
	    if (! name[0]) {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return id ;
} /* end subroutine (getprojidbyname) */

int inproj(cchar *name,cc *projname,void *rbuf,size_t) noex {
        errno_t		ec = EFAULT ;
	int		f = false ;
        if (name && projname && rbuf) {
            ec = EINVAL ;
        } /* end if (non-null) */
	if (ec) errno = ec ;
        return f ;
} /* end subroutine (inproj) */

/* Solaris® implementation */
void setprojent(void) noex {
    	errno = ENOSYS ;
} /* end */

void endprojent(void) noex {
    	errno = ENOSYS ;
} /* end */

CPROJECT *getprojent(PROJECT *,void *,size_t) noex {
    	PROJECT	 *rp = nullptr ;
	errno_t	ec = 0 ;
	ec = ENOSYS ;
	if (ec) errno = ec ;
	return rp ;
} /* end */

CPROJECT *getprojbyname(cchar *,PROJECT *,void *,size_t) noex {
    	PROJECT *rp = nullptr ;
	errno_t	ec = 0 ;
	ec = ENOSYS ;
	if (ec) errno = ec ;
	return rp ;
} /* end */

CPROJECT *getprojbyid(projid_t,PROJECT *,void *,size_t) noex {
    	PROJECT *rp = nullptr ;
	errno_t	ec = 0 ;
	ec = ENOSYS ;
	if (ec) errno = ec ;
	return rp ;
} /* end */

CPROJECT *getdefaultproj(cchar *,PROJECT *,void *,size_t) noex {
    	PROJECT *rp = nullptr ;
	errno_t	ec = 0 ;
	ec = ENOSYS ;
	if (ec) errno = ec ;
	return rp ;
} /* end subroutine (getdefaultproj) */


/******************************************************************************/
#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


