/* usys_sigx SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	For those operating systems that do not have |sigx(2)|
	we try to define it.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<climits>		/* |PATH_MAX| + |INT_MAX| */
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysflag.h>
#include	<utypedefs.h>

#include	"usys_sigx.h"


/* SIGQUEUE start */
#if	(!defined(SYSHAS_SIGQUEUE)) || (SYSHAS_SIGQUEUE == 0)

int sigqueue(pid_t pid,int sn,const SIGVAL) noex {
	int		rc = 0 ;
	if ((pid > 0) && (sn >= 0)) {
	    rc = kill(pid,sn) ;
	} else {
	    errno = EINVAL ;
	    rc = -1 ;
	}
	return rc ;
}

#endif /* (!defined(SYSHAS_SIGQUEUE)) || (SYSHAS_SIGQUEUE == 0) */
/* SIGQUEUE end */

/* SIGSEND start */
#if	(!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0)

#ifndef	ID_CONSTEXPR
#define	ID_CONSTEXPR
constexpr id_t		idend = id_t(-1) ;
#endif

int sigsend(idtype_t type,id_t id,int sn) noex {
	int		rc = 0 ;
	if ((id != idend) && (sn >= 0)) {
	    switch (type) {
	    case P_ALL:
	        id = id_t(-1) ;
	        rc = kill(id,sn) ;
	        break ;
	    case P_PID:
	        if (id != idend) {
		    if (id > 0) {
	               rc = kill(id,sn) ;
		    } else {
	               rc = killpg(0,sn) ;
		    }
	        } else {
	            errno = EINVAL ;
	            rc = -1 ;
	        }
	        break ;
	    case P_PGID:
	        id = id_t(- id) ;
	        rc = kill(id,sn) ;
	        break ;
	    default:
	        errno = ENOTSUP ;
	        rc = -1 ;
	        break ;
	    } /* end switch */
	} else {
	    errno = EINVAL ;
	    rc = -1 ;
	}
	return rc ;
}

#endif /* (!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0) */
/* SIGSEND end */

/* SIGSENDSET start */
#if	(!defined(SYSHAS_SIGSENDSET)) || (SYSHAS_SIGSENDSET == 0)

int sigsendset(procset_t *sp,int sn) noex {
	int		rc = 0 ;
	if (sp) {
	    if (sn >= 0) {
	        errno = ENOTSUP ;
	        rc = -1 ;
	    } else {
	        errno = EINVAL ;
	        rc = -1 ;
	    }
	} else {
	    errno = EFAULT ;
	    rc = -1 ;
	}
	return rc ;
}

#endif /* (!defined(SYSHAS_SIGSENDSET)) || (SYSHAS_SIGSENDSET == 0) */
/* SIGSENDSET end */

/* PTHREADSIGQUEUE begin */
#if	(!defined(SYSHAS_PTHREADSIGQUEUE)) || (SYSHAS_PTHREADSIGQUEUE == 0)

extern int pthread_sigqueue(pthread_t tid,int sn,const SIGVAL) noex {
	int		rc = 0 ;
	if (sn >= 0) {
	    if (errno_t ec ; (ec = pthread_kill(tid,sn)) > 0) {
		errno = ec ;
		rc = -1 ;
	    }
	} else {
	    errno = EINVAL ;
	    rc = -1 ;
	}
	return rc ;
}

#endif /* (!defined(SYSHAS_PTHREADSIGQUEUE)) || (SYSHAS_PTHREADSIGQUEUE == 0) */
/* PTHREADSIGQUEUE end */


