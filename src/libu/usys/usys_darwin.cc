/* usys_darwin SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"usys_darwin.h"


/* USYS_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/mman.h>
#include	<unistd.h>		/* |getdomainname(3darwin)| */


/*----------------------------------------------------------------------------*/
/* TIMER begin */
#if	(!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0)

EXTERNC_begin

errno_t timer_create(clockid_t,SIGEVENT *,timer_t *tmp) noex {
	errno_t		ec = EFAULT ;
	if (tmp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
errno_t timer_delete(timer_t) noex {
    return ENOSYS ;
}
errno_t timer_settime(timer_t,int,CITIMERSPEC *ntvp,ITIMERSPEC *) noex {
	errno_t		ec = EFAULT ;
	if (ntvp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
errno_t timer_gettime(timer_t,ITIMERSPEC *otvp) noex {
	errno_t		ec = EFAULT ;
	if (otvp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
errno_t timer_getoverrun(timer_t) noex {
    return ENOSYS ;
}

EXTERNC_end

#endif /* (!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0) */
/* TIMER end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* RELTIMEDWAIT begin */
#if	(!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0)

errno_t pthread_cond_reltimedwait_np(PTC *op,PTM *mp,CTIMESPEC *) noex {
	errno_t		ec = EFAULT ;
	if (op && mp) {
	     ec = ENOSYS ;
	}
	return ec ;
}

#endif /* (!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0) */
/* RELTIMEDWAIT end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* MEMCNTL begin */
errno_t memcntl(void *ma,size_t ms,int,void *,int,int) noex {
	errno_t		ec = EFAULT ;
	if (ma) {
	    ec = EINVAL ;
	    if (ms > 0) {
		ec = ENOSYS ;
	    }
	}
	return ec ;
}
/* MEMCNTL end */
/*----------------------------------------------------------------------------*/


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYS_DARWIN finish */


