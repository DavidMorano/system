/* ucmemla SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* Memory-Large-Allocation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucmemla

	Description:
	Memory allocation and deallocation object.  The allocation is
	managed by the object itself.

	Synosis:
	int ucmemla_acquire(ucmemla *op,size_t us,void **rpp) noex ;
	int ucmemla_release(ucmemla *op) noex ;

	Arguments:
	op		object pointer
	us		memory allocation size required
	rpp		pointer to hold resulting pointer

	Return:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<intceil.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucmemla.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* local variables */

constexpr int		uslarge = UCMEMLA_LARGE	;
static sysval		pagesz(sysval_ps) ;


/* local structures */


/* forward references */

local int	ucmemla_map(ucmemla *,size_t,void **) noex ;


/* exported variables */


/* exported subroutines */

int ucmemla_acquire(ucmemla *op,size_t us,void *rp) noex {
	void		**rpp = (void **) rp ;
	int		rs = SR_FAULT ;
	if (op && rpp) ylikely {
	    rs = SR_INVALID ;
	    op->ma = nullptr ;
	    op->ms = 0 ;
	    op->f_large = false ;
	    if (us > 0) ylikely {
		void	*vp{} ; /* used-multiple */
		if (us >= uslarge) {
		    op->f_large = true ;
		    rs = ucmemla_map(op,us,&vp) ;
		} else {
		    cint	am = int(us) ;
		    if ((rs = libmem.mall(am,&vp)) >= 0) {
		        op->ma = vp ;
		        op->ms = us ;
		    } /* end if (memory-acquire) */
		} /* end if (pagesz) */
		*rpp = vp ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucmemla_acquire) */

int ucmemla_release(ucmemla *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    if (op->f_large) {
		rs1 = u_mmapend(op->ma,op->ms) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
		rs1 = libmem.free(op->ma) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->ma = nullptr ;
	    op->ms = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucmemla_release) */


/* local subroutines */

local int ucmemla_map(ucmemla *op,size_t us,void **rpp) noex {
	cnullptr	np{} ;
	size_t		ms ;
	int		rs ;
	if ((rs = pagesz) >= 0) ylikely {
	    cint	ps = rs ;
	    cint	fd = -1 ;
	    cint	mp = (PROT_READ | PROT_WRITE) ;
	    cint	mf = (MAP_ANON | MAP_PRIVATE) ;
	    void	*md ;
	    ms = szceil(us,ps) ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) ylikely {
		op->ma = md ;
		op->ms = ms ;
		*rpp = md ;
	    } /* end if (u_mmapbegin) */
	} /* end if (pagesz) */
	return rs ;
} /* end subroutine (ucmemla_map) */


