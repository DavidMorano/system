/* ids SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load up the process identification information (IDs) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ids

	Description:
	This subroutine will load up the various process user-group
	IDs, and the supplementary group IDs.

	Synopsis:
	int ids_load(ids *op) noex

	Arguments:
	op		pointer to object

	Returns:
	>=0		number of group IDs returned
	<0		error (system-return)

	Notes:
	We allocate dynamically rather than use NGROUPS_MAX.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<unistd.h>		/* for |getgroups(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ids.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxgroups ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

static int	ids_ctor(ids *) noex ;
static int	ids_dtor(ids *) noex ;
static int	ids_ngids(const ids *) noex ;


/* local variables */

static vars		var ;

static constexpr gid_t	gidend = gid_t(-1) ;


/* exported variables */


/* exported subroutines */

int ids_load(ids *op) noex {
	int		rs ;
	int		ng = 0 ; /* return-value */
	if ((rs = ids_ctor(op)) >= 0) ylikely {
	    if ((rs = var) >= 0) ylikely {
		cint	nmax = rs ;
	        cint	sz = ((rs + 1) * szof(gid_t)) ;
	        if (void *vp ; (rs = uc_libmalloc(sz,&vp)) >= 0) ylikely {
		    op->gids = (gid_t *) vp ;
		    if ((rs = u_getgroups(nmax,op->gids)) >= 0) ylikely {
			ng = rs ;
		        op->gids[ng] = gidend ;
		    }
		    if (rs < 0) {
		        uc_libfree(op->gids) ;
		        op->gids = nullptr ;
		    } /* end if (error) */
	        } /* end if (m-a) */
	    } /* end if (vars) */
	    if (rs < 0) {	
		ids_dtor(op) ;
	    }
	} /* end if (ids_ctor) */
	return (rs >= 0) ? ng : rs ;
}
/* end subroutine (ids_load) */

int ids_release(ids *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->gids) {
	        rs1 = uc_libfree(op->gids) ;
	        if (rs >= 0) rs = rs1 ;
	        op->gids = nullptr ;
	    }
	    {
	        rs1 = ids_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ids_release) */

int ids_ngroups(ids *op) noex {
	int		rs = SR_FAULT ;
	int		ng = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->gids) ylikely {
	        for (ng = 0 ; op->gids[ng] != gidend ; ng += 1) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ng : rs ;
}
/* end subroutine (ids_ngroups) */

int ids_refresh(ids *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->gids) {
	        rs1 = uc_libfree(op->gids) ;
	        if (rs >= 0) rs = rs1 ;
	        op->gids = nullptr ;
	    }
	    if (rs >= 0) {
		rs = ids_load(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ids_refresh) */

int ids_copy(ids *op,const ids *otherp) noex {
	int		rs = SR_FAULT ;
	if (op && otherp) ylikely {
	    op->uid = otherp->uid ;
	    op->euid = otherp->euid ;
	    op->gid = otherp->gid ;
	    op->egid = otherp->egid ;
	    if ((rs = ids_ngids(otherp)) >= 0) ylikely {
	        cint	n = rs ;
	        int	sz = 0 ;
	        sz += intconv((n + 1) * szof(gid_t)) ;
	        if (void *p ; (rs = uc_libmalloc(sz,&p)) >= 0) ylikely {
	            int		i = 0 ; /* <- used-afterwards */
		    op->gids = (gid_t *) p ;
		    for (i = 0 ; otherp->gids[i] ; i += 1) {
		        op->gids[i] = otherp->gids[i] ;
		    } /* end for */
		    op->gids[i] = -1 ;
	        } /* end if (m-a) */
	    } /* end if (ids_ngids) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ids_copy) */


/* private subroutines */

static int ids_ctor(ids *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->gids = nullptr ;
	    op->uid = getuid() ;
	    op->euid = geteuid() ;
	    op->gid = getgid() ;
	    op->egid = getegid() ;
	}
	return rs ;
}
/* end subroutine (ids_ctor) */

static int ids_dtor(ids *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->gids = nullptr ;
	}
	return rs ;
}
/* end subroutine (ids_dtor) */

static int ids_ngids(const ids *op) noex {
	int		n = 0 ;
	if (op->gids) ylikely {
	    for (n = 0 ; op->gids[n] != gidend ; n += 1) ;
	}
	return n ;
}
/* end subroutine (ids_ngids) */

ids_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case idsmem_load:
	        rs = ids_load(op) ;
	        break ;
	    case idsmem_ngroups:
	        rs = ids_ngroups(op) ;
	        break ;
	    case idsmem_release:
	        rs = ids_release(op) ;
	        break ;
	    case idsmem_refresh:
	        rs = ids_refresh(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ids_co::operator) */

int ids::copy(const ids *oip) noex {
	return ids_copy(this,oip) ;
}

void ids::dtor() noex {
	if (cint rs = release ; rs < 0) {
	    ulogerror("ids",rs,"fini-release") ;
	}
}

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxgroups) == 0) ylikely {
	    cint	cmd = _SC_NGROUPS_MAX ;
	    if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
	        maxgroups = rs ;
	    } /* end if */
	} /* end if (needed) */
	return rs ;
} /* end method (vars::operator) */


