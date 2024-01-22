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

#include	<envstandards.h>

/* USYS_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"usys_darwin.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* TIMER begin */
#if	(!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

int timer_create(clockid_t,SIGEVENT *,timer_t *tmp) noex {
	int	ec = EFAULT ;
	if (tmp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
int timer_delete(timer_t) noex {
    return ENOSYS ;
}
int timer_settime(timer_t,int,ITIMERSPEC *ntvp,ITIMERSPEC *) noex {
	int	ec = EFAULT ;
	if (ntvp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
int timer_gettime(timer_t,ITIMERSPEC *otvp) noex {
	int	ec = EFAULT ;
	if (otvp) {
	    ec = ENOSYS ;
	}
	return ec ;
}
int timer_getoverrun(timer_t) noex {
    return ENOSYS ;
}

#ifdef	__cplusplus
}
#endif

#endif /* (!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0) */
/* TIMER end */


/* RELTIMEDWAIT begin */
#if	(!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0)

int pthread_cond_reltimedwait_np(PTC *op,PTM *mp,CTIMESPEC *) noex {
	int	ec = EFAULT ;
	if (op && mp) {
	     ec = ENOSYS ;
	}
	return ec ;
}

#endif /* (!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0) */
/* RELTIMEDWAIT end */


/* MEMCNTL begin */
int memcntl(void *ma,size_t ms,int,void *,int,int) noex {
	int	ec = EFAULT ;
	if (ma) {
	    ec = EINVAL ;
	    if (ms > 0) {
		ec = ENOSYS ;
	    }
	}
	return ec ;
}
/* MEMCNTL end */


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYS_DARWIN finish */


