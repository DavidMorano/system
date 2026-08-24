/* ucnameo SUPPORT */
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
	ucnameo

	Description:
	This module serves as a cache (of sorts) for UNIX® information
	that is related to the underlying machine and which does
	not (easily) change during program exection.  Although this
	object can serve as a local cache (cotrolled by this
	object itself), note that a process-wide cache of the
	|uname(2)| information can be had and managed by the
	|ucinfo(3uc)| facility (enumated system call).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucnameo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int ucnameo_ctor(ucnameo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->a = nullptr ;
	    op->sysname = nullptr ;
	    op->nodename = nullptr ;
	    op->release = nullptr ;
	    op->version = nullptr ;
	    op->machine = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucnameo_ctor) */

local inline int ucnameo_dtor(ucnameo *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (ucnameo_dtor) */


/* local variables */

static bufsizevar		nodenamelen(bufsize_nn) ;


/* exported variables */


/* exported subroutines */

int ucnameo_start(ucnameo *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ucnameo_ctor(op)) >= 0) ylikely {
	    if ((rs = nodenamelen) >= 0) ylikely {
	        cint	nlen = rs ;
	        cint	usz = szof(UTSNAME) ;
	        if (void *vp ; (rs = lm_mall(usz,&vp)) >= 0) ylikely {
	            UTSNAME *unp = resumelife<UTSNAME>(vp) ;
	            if ((rs = u_uname(unp)) >= 0) ylikely {
	                int	sz = 0 ;
	                sz += (lenstr(unp->sysname,nlen) + 1) ;
	                sz += (lenstr(unp->nodename,nlen) + 1) ;
	                sz += (lenstr(unp->release,nlen) + 1) ;
	                sz += (lenstr(unp->version,nlen) + 1) ;
	                sz += (lenstr(unp->machine,nlen) + 1) ;
	                if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
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
	                } /* end if (memory-acquire) */
	            } /* end if (uname) */
	            rs1 = lm_free(unp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (nodenamelen) */
	    if (rs < 0) {
		ucnameo_dtor(op) ;
	    } /* end if (error) */
	} /* end if (ucnameo_ctor) */
	return rs ;
} /* end subroutine (ucnameo_start) */

int ucnameo_finish(ucnameo *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->a) ylikely {
	        rs1 = lm_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	        op->sysname = nullptr ;
	        op->nodename = nullptr ;
	        op->release = nullptr ;
	        op->version = nullptr ;
	        op->machine = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = ucnameo_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucnameo_finish) */


