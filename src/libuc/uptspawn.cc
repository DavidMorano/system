/* uptspawn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® POSIX Thread manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David AÂ­DÂ­ Morano
        This is a complete rewrite of the trash that performed this function
        previously.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uptspawn

	Description:
        This module is an extension of the UPT (UNIX® POSIX thread) code 
	module.  This module contains the subroutine |utpspawn()|.

	Name:
	int uptspawn(pthread_t *rp,pthread_attr_t *ptap,objsub_t *start,
		void *op,void *arg) noex

	Arguments:
	rp		pinter to hold resulting thread-id
	ptap 		pointer to pthread attributes
	tart		starting thread address
	op		thread object pointer (arg1)
	arg		argument (arg2)

	Returns:
	>=0		some value
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<unistd.h>
#include	<csignal>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<usystem.h>
#include	<localmisc.h>

#include	"upt.h"


/* local defines */

#ifndef	OURARGS
#define	OURARGS		struct ourargs
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int (*objsub_t)(void *,void *) noex ;
}


/* external subroutines */


/* local structures */

struct ourargs {
	objsub_t	start ;
	void		*op ;
	void		*ap ;
} ;


/* forward references */

static int	uptcreator(pthread_t *,pthread_attr_t *,void *) noex ;

static void	*uptruner(void *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uptspawn(pthread_t *rp,pthread_attr_t *ptap,objsub_t start,
		void *op,void *arg) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (rp && start) {
	    cint	osz = szof(OURARGS) ;
	    if (OURARGS *oap ; (rs = libmem.mall(osz,&oap)) >= 0) {
	        sigset_t	nsm ;
	        sigset_t	osm ;
	        uc_sigsetfill(&nsm) ;
	        if ((rs = u_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
		    {
	                oap->start = start ;
	                oap->op = op ;
	                oap->ap = arg ;
		        if ((rs = uptcreator(rp,ptap,oap)) >= 0) {
			    uintptr_t	v = uintptr_t(*rp) ;
		            rv = (v & INT_MAX) ;
		        }
		    }
		    rs1 = u_sigmask(SIG_SETMASK,&osm,NULL) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uptspawn) */


/* local subroutines */

static int uptcreator(pthread_t *rp,pthread_attr_t *ptap,void *arg) noex {
	int		to_nomem = utimeout[uto_nomem] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    rs = SR_OK ;
	    if (errno_t ec ; (ec = pthread_create(rp,ptap,uptruner,arg)) > 0) {
		rs = (- ec) ;
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
	OURARGS		*oap = (OURARGS *) vp ;
	void		*vrp ;
	int		rs = SR_NOEXEC ;
	if (oap) {
	    int		(*start)(void *,void *) = oap->start ;
	    void	*op = oap->op ;
	    void	*ap = oap->ap ;
	    libmem.free(oap) ;
	    rs = (*start)(op,ap) ;
	} /* end if (non-null) */
	vrp = (void *) uintptr_t(rs) ;
	return vrp ;
}
/* end subroutine (uptruner) */


