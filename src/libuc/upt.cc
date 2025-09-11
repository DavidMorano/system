/* upt SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® POSIX Thread manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	upt

	Description:
	This module provides an informal way to abstract some of
	the junk needed to work with threads.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<unistd.h>
#include	<cerrno>
#include	<csignal>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdint>
#include	<usystem.h>
#include	<hasx.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"upt.h"


/* local defines */

#ifndef	pta
#define	pta		pthread_attr_t
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct ourarg {
	int		(*start)(void *) noex ;
	void		*ap ;
} ;


/* forward references */

static int	uptcreator(pthread_t *,pta *,void *) noex ;

static void	*uptruner(void *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uptcreate(pthread_t *rp,pta *ptap,uptsub_f start,void *arg) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		tid = -1 ;
	if (rp && start) {
	    cint	osz = szof(ourarg) ;
	    if (void *vp ; (rs = libmem.mall(osz,&vp)) >= 0) {
	        ourarg	*oap = (ourarg *) vp ;
	        sigset_t	osm ;
	        sigset_t	nsm ;
	        uc_sigsetfill(&nsm) ;
	        if ((rs = u_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
	            oap->ap = arg ;
	            oap->start = start ;
		    if ((rs = uptcreator(rp,ptap,oap)) >= 0) {
			uintptr_t	ut = uintptr_t(*rp) ;
		        tid = int(ut & INT_MAX) ;
		    }
		    rs1 = u_sigmask(SIG_SETMASK,&osm,nullptr) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	        if ((rs < 0) && (tid < 0)) {
		    libmem.free(oap) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? tid : rs ;
}
/* end subroutine (uptcreate) */

int uptexit(int rs) noex {
	uintptr_t	up = uintptr_t(rs) ;
	{
	    void	*vrp = (void *) up ;
	    pthread_exit(vrp) ;
	}
	return SR_OK ;
}
/* end subroutine (uptexit) */

int uptonce(pthread_once_t *op,void_f initsub) noex {
	int		rs ;
	repeat {
	    rs = pthread_once(op,initsub) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptonce) */

int uptjoin(pthread_t tid,int *rsp) noex {
	void		*vp{} ;
	int		rs ;
	repeat {
	    if (errno_t ec ; (ec = pthread_join(tid,&vp)) > 0) {
	        rs = (- ec) ;
	    }
	} until (rs != SR_INTR) ;
	if (rsp) {
	    uintptr_t	up = uintptr_t(vp) ;
	    *rsp = int(up) ;
	}
	return rs ;
}
/* end subroutine (uptjoin) */

int uptdetach(pthread_t tid) noex {
	int		rs ;
	repeat {
	    rs = pthread_detach(tid) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptdetach) */

int uptcancel(pthread_t tid) noex {
	int		rs ;
	repeat {
	    rs = pthread_cancel(tid) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptcancel) */

int uptkill(pthread_t tid,int sig) noex {
	int		rs ;
	repeat {
	    rs = pthread_kill(tid,sig) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptkill) */

int uptsetschedparam(pthread_t tid,int policy,SCHEDPARAM *pp) noex {
	int		rs ;
	repeat {
	    rs = pthread_setschedparam(tid,policy,pp) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptsetschedparam) */

int uptgetschedparam(pthread_t tid,int *policyp,SCHEDPARAM *pp) noex {
	int		rs ;
	repeat {
	    rs = pthread_getschedparam(tid,policyp,pp) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptgetschedparam) */

int uptsetcancelstate(int intstate,int *oldstate) noex {
	int		rs ;
	repeat {
	    rs = pthread_setcancelstate(intstate,oldstate) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptsetcancelstate) */

int uptsetcanceltype(int intstate,int *oldstate) noex {
	int		rs ;
	repeat {
	    rs = pthread_setcanceltype(intstate,oldstate) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptsetcanceltype) */

int upttestcancel() noex {
	pthread_testcancel() ;
	return SR_OK ;
}
/* end subroutine (upttestcancel) */

int uptequal(pthread_t t1,pthread_t t2) noex {
	return pthread_equal(t1,t2) ;
}
/* end subroutine (uptequal) */

int uptself(pthread_t *rp) noex {
	const pthread_t	tid = pthread_self() ;
	int		rc{} ;
	if (rp) *rp = tid ;
	{
	    uintptr_t	up = uintptr_t(tid) ;
	    rc = int(up & INT_MAX) ;
	}
	return rc ;
}
/* end subroutine (uptself) */

int uptgetconcurrency() noex {
	int		rs ;
	repeat {
	    errno = 0 ;
	    rs = pthread_getconcurrency() ;
	    if (errno != 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uptgetconcurrency) */

int uptsetconcurrency(int c) noex {
	int		rs ;
	if (c < 1) c = 1 ;
	repeat {
	    rs = pthread_setconcurrency(c) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;
	return c ;
}
/* end subroutine (uptsetconcurrency) */

int uptatfork(void_f pre,void_f par,void_f chi) noex {
	return uc_atfork(pre,par,chi) ;
}
/* end subroutine (uptatfork) */

/* ARGSUSED */
int uptncpus(int w) noex {
	int		rs = SR_OK ;
	int		n = 0 ;
	if (n == 0) {
	    static cchar	*valp = getenv(varname.ncpu) ;
	    if (valp) {
	        if (hasalldig(valp,-1)) {
	            if (int v{} ; (cfdeci(valp,-1,&v)) >= 0) {
			n = v ;
		    }
	        }
	    } /* end if (environment) */
	}
	if (n == 0) {
	    rs = uc_nprocessors(w) ;
	    n = rs ;
	}
	if (n == 0) n = 1 ;
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uptncpus) */


/* local subroutines */

static int uptcreator(pthread_t *rp,pta *ptap,void *arg) noex {
	int		to_nomem = utimeout[uto_nomem] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = pthread_create(rp,ptap,uptruner,arg)) > 0) {
		rs = (- rs) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
		    if (to_nomem-- > 0) {
		        msleep(1000) ;
		    } else {
		        f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
		    break ;
	        default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (uptcreator) */

static void *uptruner(void *vp) noex {
	struct ourarg	*oap = (struct ourarg *) vp ;
	void		*vrp = nullptr ;
	int		rs = SR_NOEXEC ;
	if (oap) {
	    int		(*start)(void *) = oap->start ;
	    void	*arg = oap->ap ;
	    if ((rs = libmem.free(oap)) >= 0) {
	        rs = (*start)(arg) ;
	    }
	    {
		uintptr_t	up = uintptr_t(rs) ;
	        vrp = (void *) up ;
	    }
	} /* end if (non-null) */
	return vrp ;
}
/* end subroutine (uptruner) */


