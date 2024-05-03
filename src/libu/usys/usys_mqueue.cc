/* usys_mqueue SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<usysrets.h>

#include	"usys_mqueue.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */

consteval mqd_t mkmqdbad() noex {
	mqd_t	r = 0 ;
	return (~r) ;
}


/* local variables */

static constexpr mqd_t	mqdbad = mkmqdbad() ;


/* exported subroutines */

/* PMQ begin */
#if	(!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0)

#ifndef	MQATTR
#define	MQATTR		struct mq_attr
#endif

mqd_t mq_open(const char *n,int,mode_t,const MQATTR *) noex {
	int	ec = EFAULT ;
	if (n) {
	    ec = EINVAL ;
	    if (n[0]) {
		ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return mqdbad ;
}
int mq_close(mqd_t qd) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = ENOSYS ;
	}
	errno = ec ;
	return -1 ;
}
int mq_send(mqd_t qd,const char *sp,int,int) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (sp) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}
int mq_receive(mqd_t qd,char *rp,int,uint *) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (rp) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}
int mq_setattr(mqd_t qd,const MQATTR *ap,MQATTR *) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (ap) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}
int mq_getattr(mqd_t qd,MQATTR *ap) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (ap) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}
int mq_notify(mqd_t qd,struct sigevent *sep) noex {
	int	ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (sep) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}
int mq_unlink(const char *n) noex {
	int	ec = EFAULT ;
	if (n) {
	    ec = EINVAL ;
	    if (n[0]) {
	        ec = ENOSYS ;
	    }
	}
	errno = ec ;
	return -1 ;
}

#endif /* (!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0) */
/* PMQ end */


