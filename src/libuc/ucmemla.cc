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
#include	<sys/types.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysval.hh>
#include	<intceil.h>
#include	<localmisc.h>

#include	"ucmemla.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* local variables */

constexpr int		uslarge = UCMEMLA_LARGE	;

static sysval		pagesize(sysval_ps) ;


/* local structures */


/* forward references */

static int	ucmemla_map(ucmemla *,size_t,void **) noex ;


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
		    if ((rs = uc_libmalloc(am,&vp)) >= 0) {
		        op->ma = vp ;
		        op->ms = us ;
		    } /* end if (memory-allocation) */
		} /* end if (pagesize) */
		*rpp = vp ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucmemla_acquire) */

int ucmemla_release(ucmemla *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (op->f_large) {
		rs = u_mmapend(op->ma,op->ms) ;
	    } else {
		rs = uc_libfree(op->ma) ;
	    }
	    op->ma = nullptr ;
	    op->ms = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucmemla_release) */


/* local subroutines */

static int ucmemla_map(ucmemla *op,size_t us,void **rpp) noex {
	cnullptr	np{} ;
	size_t		ms ;
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    cint	ps = rs ;
	    cint	fd = -1 ;
	    cint	mp = (PROT_READ | PROT_WRITE) ;
	    cint	mf = (MAP_ANON | MAP_PRIVATE) ;
	    void	*md ;
	    ms = szceil(us,ps) ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
		op->ma = md ;
		op->ms = ms ;
		*rpp = md ;
	    } /* end if (u_mmapbegin) */
	} /* end if (pagesize) */
	return rs ;
}
/* end subroutine (ucmemla_map) */


