/* usys_shadow SUPPORT */
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
	usys_shadow

	Description:
	These subroutines offer support for the UNIX® Shadow-Password
	facility.

	Synopsis:
	int usys_shadow(char *rbuf,int rlen,pid_t pid) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	pid		PID of process to retrieve execuatable path for

	Returns:
	>=0		exec-file found and this is its length in result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |getpid(2)| */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| + |getprogname(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"usys_shadow.h"


#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)
/******************************************************************************/


/* *nothing* */


/******************************************************************************/
#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */
/******************************************************************************/


/* Solaris® implementation */
namespace solaris {
    CSPWD *getspent_r(SPWD *sps,char *rbuf,int rlen) noex {
    	SPWD		*rp = nullptr ;
	int		ec = 0 ;
	if (sps && rbuf) {
	    if (rlen > 0) {
		    ec = ENOSYS ;
	    } else {
		ec = EOVERFLOW ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
    } /* end subroutine (getspent_r) */
    CSPWD *getspnam_r(cchar *nam,SPWD *sps,char *rbuf,int rlen) noex {
    	SPWD		*rp = nullptr ;
	int		ec = 0 ;
	if (nam && sps && rbuf) {
	    if (rlen > 0) {
		if (nam[0]) {
		    ec = ENOSYS ;
		} else {
		    ec = EINVAL ;
		}
	    } else {
		ec = EOVERFLOW ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	return rp ;
    } /* end subroutine (getspnam_r) */
} /* end namespace (solaris) */


#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */
/* SYSHAS_SHADOW finish */


