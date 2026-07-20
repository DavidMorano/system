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
#include	<sys/types.h>		/* POSIX® */
#include	<pthread.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD |uintptr_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<usysconf.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uptprime.h"
#include	"uptspawn.h"

#pragma		GCC dependency		"mod/usigblock.ccm"

import usigblock ;

/* local defines */


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* local structures */

struct ourargs {
	objsub_t	start ;
	void		*op ;
	void		*ap ;
} ; /* end struct */


/* forward references */

local int	uptcreator(pthread_t *,pthread_attr_t *,void *) noex ;
local void	*uptruner(void *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uptspawn(pthread_t *rp,pthread_attr_t *ptap,objsub_t start,
		void *op,void *arg) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (rp && start) {
	    cint	osz = szof(ourargs) ;
	    if (ourargs *oap ; (rs = umem.mall(osz,&oap)) >= 0) {
		if (usigblock b ; (rs = b.start) >= 0) {
		    {
	                oap->start = start ;
	                oap->op = op ;
	                oap->ap = arg ;
		        if ((rs = uptcreator(rp,ptap,oap)) >= 0) {
			    uintptr_t	v = uintptr_t(*rp) ;
		            rv = (v & INT_MAX) ;
		        }
		    } /* end block */
		    rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (uptspawn) */


/* local subroutines */

local int uptcreator(pthread_t *rp,pthread_attr_t *ptap,void *arg) noex {
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
} /* end subroutine (uptcreator) */

local void *uptruner(void *vp) noex {
	ourargs		*oap = (ourargs *) vp ;
	void		*vrp ;
	int		rs = SR_NOEXEC ;
	if (oap) {
	    int		(*start)(void *,void *) = oap->start ;
	    void	*op = oap->op ;
	    void	*ap = oap->ap ;
	    umem.free(oap) ;
	    rs = (*start)(op,ap) ;
	} /* end if (non-null) */
	vrp = (void *) uintptr_t(rs) ;
	return vrp ;
} /* end subroutine (uptruner) */


