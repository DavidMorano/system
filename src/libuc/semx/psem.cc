/* psem SUPPORT (POSIX® unnamed semaphores) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX© unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	psem

	Description:
	This module provides a sanitized version of the standard
	POSIX® semaphore facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!

	Enjoy!

	Important-note:
	The Apple-Darwin operating system has the unnamed POSIX®
	semaphores depracated. This produces bad and very annoying
	error messages to appear from the compiler when attempting
	to use the |init| and |destroy| subroutines (methods) of
	the POSIX® semaphore object.  There is no easy fix for this
	that I know of.  So to get around this,  I redefine (using
	the C/C++ language preprocessor (CPP) to redefine the
	names for those subroutines to something else (which are
	defined elsewhere) so that at least we get a clear ocmpile
	for code test purposes.  Not that for real run-time, the
	native Apple-Darwin implementation of POSIX® is completely
	bypassed with my own implementation.  See the pre-processor
	definitions under the section below named "local defines"
	to see how I deal with this (crap)!  

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	<rpsem.h>		/* POSIX® semaphore -- real */
#include	<epsem.h>		/* POSIX® semaphore -- enumulated */

#include	"psem.h"


/* local defines */

#if	defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) 
#define	F_PSEM		1
#else
#define	F_PSEM		0
#endif /* defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_psem = F_PSEM ;


/* exported variables */


/* exported subroutines */

int psem_create(psem *op,int pshared,int acnt) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    if_constexpr (f_psem) {
		if (rpsem *semp ; (semp = new(nothrow) rpsem) != np) {
		    if ((rs = rpsem_create(semp,pshared,acnt)) >= 0) {
			op->subobj = semp ;
		    } /* end if (rpsem_create) */
		    if (rs < 0) {
			delete semp ;
			op->subobj = nullptr ;
		    }
		} /* end if (new-rpsem) */
	    } else {
		if (epsem *semp ; (semp = new(nothrow) epsem) != np) {
		    if ((rs = epsem_create(semp,pshared,acnt)) >= 0) {
			op->subobj = semp ;
		    } /* end if (rpsem_create) */
		    if (rs < 0) {
			delete semp ;
			op->subobj = nullptr ;
		    }
		} /* end if (new-epsem) */
	    } /* end if_constexpr (f_psem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_create) */

int psem_destroy(psem *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
		rs = SR_OK ;
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    {
		        rs1 = rpsem_destroy(semp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
			delete semp ;
			op->subobj = nullptr ;
		    }
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    {
		        rs1 = epsem_destroy(semp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
			delete semp ;
			op->subobj = nullptr ;
		    }
	        } /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_destroy) */

int psem_wait(psem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    rs = rpsem_wait(semp) ;
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    rs = epsem_wait(semp) ;
	        } /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_wait) */

int psem_trywait(psem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    rs = rpsem_trywait(semp) ;
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    rs = epsem_trywait(semp) ;
	        } /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_trywait) */

int psem_waiter(psem *op,int to) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    rs = rpsem_waiter(semp,to) ;
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    rs = epsem_waiter(semp,to) ;
	        } /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_waiter) */

int psem_post(psem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    rs = rpsem_post(semp) ;
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    rs = epsem_post(semp) ;
	        } /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_post) */

int psem_count(psem *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->subobj) {
	        if_constexpr (f_psem) {
		    rpsem *semp = (rpsem *) op->subobj ;
		    rs = rpsem_count(semp) ;
	        } else {
		    epsem *semp = (epsem *) op->subobj ;
		    rs = epsem_count(semp) ;
	        } /* end if_constexpr (f_psem) */
		c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (psem_count) */


/* local subroutines */

int psem::create(int pshared,int acnt) noex {
	return psem_create(this,pshared,acnt) ;
}

psem::operator int () noex {
	return psem_count(this) ;
}

void psem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("psem",rs,"fini-destroy") ;
	}
}

int psem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case psemmem_wait:
	        rs = psem_wait(op) ;
	        break ;
	    case psemmem_waiter:
	        rs = psem_waiter(op,a) ;
	        break ;
	    case psemmem_trywait:
	        rs = psem_trywait(op) ;
	        break ;
	    case psemmem_post:
	        rs = psem_post(op) ;
	        break ;
	    case psemmem_count:
	        rs = psem_count(op) ;
	        break ;
	    case psemmem_destroy:
	        rs = psem_destroy(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (psem_co::operator) */


