/* unameo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® information (a cache for |uname(2)|) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	unameo

	Description:
	This module serves as a cache (of sorts) for UNIX® information
	that is related to the underlying machine and which does
	not (easily) change during program exection.  Although this
	object can serve as a local cache (cotrolled by this
	object itself), note that a process-wide cache of the
	|uname(2)| information can be had and managed by the
	|uinfo(3uc)| facility (enumated system call).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"unameo.h"

import libutil ;

/* local defines */

#ifndef	NODENAMELEN
#define	NODENAMELEN	256
#endif


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int unameo_ctor(unameo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->a = nullptr ;
	    op->sysname = nullptr ;
	    op->nodename = nullptr ;
	    op->release = nullptr ;
	    op->version = nullptr ;
	    op->machine = nullptr ;
	}
	return rs ;
}

static inline int unameo_dtor(unameo *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (unameo_dtor) */


/* local variables */

static bufsizevar		nodenamelen(getbufsize_nn) ;


/* exported variables */


/* exported subroutines */

int unameo_start(unameo *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = unameo_ctor(op)) >= 0) ylikely {
	    if ((rs = nodenamelen) >= 0) ylikely {
	        cint	nlen = rs ;
	        cint	usz = szof(UTSNAME) ;
	        if (void *vp ; (rs = uc_malloc(usz,&vp)) >= 0) ylikely {
	            UTSNAME	*unp = (UTSNAME *) vp ;
	            if ((rs = u_uname(unp)) >= 0) ylikely {
	                int	sz = 0 ;
	                sz += (lenstr(unp->sysname,nlen) + 1) ;
	                sz += (lenstr(unp->nodename,nlen) + 1) ;
	                sz += (lenstr(unp->release,nlen) + 1) ;
	                sz += (lenstr(unp->version,nlen) + 1) ;
	                sz += (lenstr(unp->machine,nlen) + 1) ;
	                if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) ylikely {
	                    op->a = bp ;
	                    op->sysname = bp ;
	                    bp = (strwcpy(bp,unp->sysname,nlen) + 1) ;
	                    op->nodename = bp ;
	                    bp = (strwcpy(bp,unp->nodename,nlen) + 1) ;
	                    op->release = bp ;
	                    bp = (strwcpy(bp,unp->release,nlen) + 1) ;
	                    op->version = bp ;
	                    bp = (strwcpy(bp,unp->version,nlen) + 1) ;
	                    op->machine = bp ;
	                    bp = (strwcpy(bp,unp->machine,nlen) + 1) ;
	                } /* end if (memory-allocation) */
	            } /* end if (uname) */
	            rs1 = uc_free(unp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (nodenamelen) */
	    if (rs < 0) {
		unameo_dtor(op) ;
	    }
	} /* end if (unameo_ctor) */
	return rs ;
}
/* end subroutine (unameo_start) */

int unameo_finish(unameo *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->a) ylikely {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	        op->sysname = nullptr ;
	        op->nodename = nullptr ;
	        op->release = nullptr ;
	        op->version = nullptr ;
	        op->machine = nullptr ;
	    }
	    {
	        rs1 = unameo_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (unameo_finish) */


