/* usys_darwin SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* define some missing or special subroutines for Apple-Darwin */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usys_darwin

	Description:
	I define some subroutine that are either missing or that
	are specially needed by the Apple-Darwin operating system.
	The more stuff that is in here means that Apple-Darwin is
	missing more stuff (noramlly the case) than other operating
	systems that have more of the normal stuff in them.

	Notes:
	1. the Apple-Darwin operating system is normally missing a
	log of stuff as compared with other operating systems.  The
	missing stuff is all throughout this entire code base,
	rather than just missig in this area.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* USYS_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<unistd.h>		/* |getdomainname(3darwin)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscallbase.hh>	/* |usyscallbase(3u)| */
#include	<intsat.h>
#include	<localmisc.h>

#include	"usys_darwin.h"


using namespace	libu ;

namespace {
    struct syscaller ;
    typedef int (syscaller::*syscaller_m)() noex ;
    struct syscaller : usyscallbase {
	syscaller_m	m = nullptr ;
	cchar		*name ;
	char		*obuf ;
	int		olen ;
	int operator () (char *b,int l,cchar *n) noex {
	    name = n ;
	    obuf = b ;
	    olen = l ;
	    return handler() ;
	} ;
        int callstd() noex override {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ;
	int isysctl() noex ;
    } ; /* end struct (syscaller) */
} /* end namespace */

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
/* SIGWAITINFO begin */
#if	(!defined(SYSHAS_SIGWAITINFO)) || (SYSHAS_SIGWAITINFO == 0)

typedef	CTIMESPEC 	ctsp ;

EXTERNC_begin

unixret_t sigwaitinfo(const sigset_t *setp,siginfo_t *sip) noex {
    	int	rc = -1 ;	
	if (setp && sip) {
	    errno = ENOSYS ;
	} else {
	    errno = EFAULT ;
	}
	return rc ;
} /* end subroutine (sigwaitinfo) */

unixret_t sigtimedwait(const sigset_t *setp,siginfo_t *sip,ctsp *tsp) noex {
    	int	rc = -1 ;	
	if (setp && sip && tsp) {
	    errno = ENOSYS ;
	} else {
	    errno = EFAULT ;
	}
	return rc ;
} /* end subroutine (sigtimedwait) */

unixret_t sigwaitinfoto(const sigset_t *setp,siginfo_t *sip,ctsp *tsp) noex {
    	return sigtimedwait(setp,sip,tsp) ;
}

EXTERNC_end

#endif /* (!defined(SYSHAS_SIGWAITINFO)) || (SYSHAS_SIGWAITINFO == 0) */
/* SIGWAITINFO end */
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

/*----------------------------------------------------------------------------*/
/* PSEM begin (unnamed POSIX® semaphores) */
#if	(!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0)

EXTERNC_begin

unixret_t darwinsem_init(sem_t *semp,int,unsigned int) noex {
    	int		rs = SR_FAULT ;
	if (semp) {
	    rs = SR_NOSYS ;
	}
	return rs ;
}

unixret_t darwinsem_destroy(sem_t *semp) noex {
    	int		rs = SR_FAULT ;
	if (semp) {
	    rs = SR_NOSYS ;
	}
	return rs ;
}

EXTERNC_end

#endif /* (!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0) */
/* PSEM end */
/*----------------------------------------------------------------------------*/

namespace usys {
    sysret_t darwin_usysctl(char *obuf,int olen,cchar *name) noex {
	int		rs = SR_FAULT ;
	if (obuf && name) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
	        syscaller syscall ;
	        syscall.m = &syscaller::isysctl ;
	        rs = syscall(obuf,olen,name) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_usysctl) */
} /* end namespace (usys) */

int syscaller::isysctl() noex {
	cnullptr	np{} ;
	size_t		osize = size_t(olen) ;
	int		rs ;
	int		len = 0 ;
        if ((rs = sysctlbyname(name,obuf,&osize,np,0uz)) >= 0) {
            len = intsat(osize) ;
            obuf[len] = '\0' ;
        } else {
            rs = (- errno) ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (syscaller:isysctl) */

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYS_DARWIN finish */


