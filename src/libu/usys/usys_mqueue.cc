/* usys_mqueue SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>

#include	"usys_mqueue.h"

consteval mqd_t mkmqdbad() noex {
	mqd_t	r = 0 ;
	return (~r) ;
}

static constexpr mqd_t	mqdbad = mkmqdbad() ;

/* PMQ begin */
#if	(!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0)

#ifndef	MQATTR
#define	MQATTR		struct mq_attr
#endif

mqd_t mq_open(const char *n,int,mode_t,const MQATTR *) noex {
	errno_t		ec = EFAULT ;
	if (n) {
	    ec = EINVAL ;
	    if (n[0]) {
		ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return mqdbad ;
}
unixret_t mq_close(mqd_t qd) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = ENOSYS ;
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_send(mqd_t qd,const char *sp,int,int) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (sp) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_receive(mqd_t qd,char *rp,int,uint *) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (rp) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_setattr(mqd_t qd,const MQATTR *ap,MQATTR *) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (ap) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_getattr(mqd_t qd,MQATTR *ap) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (ap) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_notify(mqd_t qd,struct sigevent *sep) noex {
	errno_t		ec = EBADFD ;
	if (qd != mqdbad) {
	    ec = EFAULT ;
	    if (sep) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}
unixret_t mq_unlink(const char *n) noex {
	errno_t		ec = EFAULT ;
	if (n) {
	    ec = EINVAL ;
	    if (n[0]) {
	        ec = ENOSYS ;
	    }
	}
	if (ec) errno = ec ;
	return -1 ;
}

#endif /* (!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0) */
/* SYSHAS_PMQ end */


