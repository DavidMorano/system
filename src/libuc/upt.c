/* upt */
/* lang=C20 */

/* UNIX� POSIX Thread manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A�D� Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This module provides an informal way to abstract some of
	the junk needed to work with threads.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<pthread.h>
#include	<signal.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"upt.h"


/* local defines */

#define	TO_NOMEM	5

#ifndef	VARNCPU
#define	VARNCPU		"NCPU"
#endif

#ifndef	PTA
#define	PTA		pthread_xattr_t
#endif


/* local namespaces */


/* local typedefs */

typedef	void	(*voidfunc)(void) noex ;


/* external subroutines */


/* local structures */

struct ourarg {
	int		(*start)(void *) noex ;
	void		*ap ;
} ;


/* forward references */

static int	uptcreator(pthread_t *,PTA *,void *) noex ;

static void	*uptruner(void *) noex ;


/* exported subroutines */

int uptcreate(pthread_t *rp,PTA *ptap,uptsub_t start,void *arg) noex {
{
	struct ourarg	*oap ;
	const int	osize = sizeof(struct ourarg) ;
	int		rs ;
	int		rs1 ;
	int		tid = -1 ;

#if	CF_DEBUGS
	debugprintf("uptcreate: ent start=%p arg=%p\n",start,arg) ;
#endif

	if (rp == NULL) return SR_FAULT ;
	if (start == NULL) return SR_FAULT ;

	if ((rs = uc_libmalloc(osize,&oap)) >= 0) {
	    sigset_t	nsm, osm ;
	    uc_sigsetfill(&nsm) ;
	    if ((rs = pt_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
	        oap->ap = arg ;
	        oap->start = start ;
		if ((rs = uptcreator(rp,ptap,oap)) >= 0) {
		    tid = (*rp & INT_MAX) ;
		}
		rs1 = pt_sigmask(SIG_SETMASK,&osm,NULL) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs < 0) && (tid < 0)) {
		uc_libfree(oap) ;
	    }
	} /* end if (memory-allocation) */

	return (rs >= 0) ? tid : rs ;
}
/* end subroutine (uptcreate) */


int uptexit(int rs)
{
	void		*vrp = (void *) rs ;
	pthread_exit(vrp) ;
	return SR_OK ;
}
/* end subroutine (uptexit) */


int uptonce(pthread_once_t *op,void (*initsub)())
{
	int		rs ;

	repeat {
	    rs = pthread_once(op,initsub) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptonce) */


int uptjoin(pthread_t tid,int *rsp)
{
	void		*vp, **vrpp ;
	int		rs ;

	vrpp = (rsp != NULL) ? &vp : NULL ;

	repeat {
	    rs = pthread_join(tid,vrpp) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	if (rsp != NULL) *rsp = (int) vp ;

	return rs ;
}
/* end subroutine (uptjoin) */


int uptdetach(pthread_t tid)
{
	int		rs ;

	repeat {
	    rs = pthread_detach(tid) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptdetach) */


int uptcancel(pthread_t tid)
{
	int		rs ;

	repeat {
	    rs = pthread_cancel(tid) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptcancel) */


int uptkill(pthread_t tid,int sig)
{
	int		rs ;

	repeat {
	    rs = pthread_kill(tid,sig) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptkill) */


int uptsetschedparam(pthread_t tid,int policy,struct sched_param *pp)
{
	int		rs ;

	repeat {
	    rs = pthread_setschedparam(tid,policy,pp) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptsetschedparam) */


int uptgetschedparam(pthread_t tid,int *policyp,struct sched_param *pp)
{
	int		rs ;

	repeat {
	    rs = pthread_getschedparam(tid,policyp,pp) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptgetschedparam) */


int uptsetcancelstate(int intstate,int *oldstate)
{
	int		rs ;

	repeat {
	    rs = pthread_setcancelstate(intstate,oldstate) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptsetcancelstate) */


int uptsetcanceltype(int intstate,int *oldstate)
{
	int		rs ;

	repeat {
	    rs = pthread_setcanceltype(intstate,oldstate) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptsetcanceltype) */


int upttestcancel()
{
	pthread_testcancel() ;
	return SR_OK ;
}
/* end subroutine (upttestcancel) */


int uptequal(pthread_t t1,pthread_t t2)
{
	return pthread_equal(t1,t2) ;
}
/* end subroutine (uptequal) */


int uptself(pthread_t *rp)
{
	const pthread_t	tid = pthread_self() ;
	int		rc ;

	if (rp != NULL) *rp = tid ;

	rc = (tid & INT_MAX) ;
	return rc ;
}
/* end subroutine (uptself) */


int uptgetconcurrency()
{
	int		rs ;

	repeat {
	    errno = 0 ;
	    rs = pthread_getconcurrency() ;
	    if (errno != 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (uptgetconcurrency) */


int uptsetconcurrency(int c)
{
	int		rs ;

	if (c < 1) c = 1 ;

	repeat {
	    rs = pthread_setconcurrency(c) ;
	    if (rs > 0) rs = (- rs) ;
	} until (rs != SR_INTR) ;

	return c ;
}
/* end subroutine (uptsetconcurrency) */


int uptatfork(voidfunc pre,voidfunc par,voidfunc chi)
{
	return uc_atfork(pre,par,chi) ;
}
/* end subroutine (uptatfork) */


/* ARGSUSED */
int uptncpus(int w)
{
	int		rs = SR_OK ;
	int		n = 0 ;

	if (n == 0) {
	    cchar	*vn = VARNCPU ;
	    cchar	*cp ;
	    if ((cp = getenv(vn)) != NULL) {
	        if (hasalldig(cp,-1)) {
	            int	v ;
	            if ((cfdeci(cp,-1,&v)) >= 0) n = v ;
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

static int uptcreator(pthread_t *rp,PTA *ptap,void *arg) noex {
	int		to_nomem = TO_NOMEM ;
	int		rs ;
	int		f_exit = FALSE ;
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
		        f_exit = TRUE ;
		    }
		    break ;
	        case SR_INTR:
		    break ;
	        default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (uptcreator) */

static void *uptruner(void *vp) noex {
	struct ourarg	*oap = (struct ourarg *) vp ;
	void		*vrp ;
	int		rs = SR_NOEXEC ;
	if (oap) {
	    int		(*start)(void *) = oap->start ;
	    void	*arg = oap->ap ;
	    if ((rs = uc_libfree(oap)) >= 0) {
	        rs = (*start)(arg) ;
	    }
	} /* end if (non-null) */
	vrp = (void *) rs ;
	return vrp ;
}
/* end subroutine (uptruner) */


