/* unameo */
/* lang=C++20 */

/* UNIX® information (a cache for |uname(2)|) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module serves as a cache (of sorts) for UNIX® information
	that is related to the underlying machine and which does
	not (easily) change during program exection.

	Although this object can service as a local cache (cotrolled
	by this object itself), note that a process-wide cache of
	the |uname(2)| information can be had and managed by the
	|uinfo(3uc)| facility (enumated system call).

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"unameo.h"


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
	if (op && (args && ...)) {
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


/* local variables */


/* exported subroutines */

int unameo_start(unameo *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = unameo_ctor(op)) >= 0) {
	    cint	usz = sizeof(UTSNAME) ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(usz,&vp)) >= 0) {
	        UTSNAME		*unp = (UTSNAME *) vp ;
	        if ((rs = u_uname(unp)) >= 0) {
	            cint	nlen = NODENAMELEN ;
	            int		size = 0 ;
	            char	*bp ;
	            size += (strnlen(unp->sysname,nlen) + 1) ;
	            size += (strnlen(unp->nodename,nlen) + 1) ;
	            size += (strnlen(unp->release,nlen) + 1) ;
	            size += (strnlen(unp->version,nlen) + 1) ;
	            size += (strnlen(unp->machine,nlen) + 1) ;
	            if ((rs = uc_malloc(size,&bp)) >= 0) {
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
	} /* end if (unameo_ctor) */
	return rs ;
}
/* end subroutine (unameo_start) */

int unameo_finish(unameo *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	        op->sysname = nullptr ;
	        op->nodename = nullptr ;
	        op->release = nullptr ;
	        op->version = nullptr ;
	        op->machine = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (unameo_finish) */


