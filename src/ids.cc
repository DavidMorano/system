/* ids */
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
	<0		error
	>=0		number of group IDs returned

	Notes:
	We allocate dynamically rather than use NGROUPS_MAX.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>		/* for |getgroups(2)| */
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<gidend.hh>
#include	<localmisc.h>

#include	"ids.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct idsreserve {
	int		ngroups ;
	operator int () noex ;
    } ; /* end struct (idsreserve) */
}


/* forward references */

static int	ids_ctor(ids *) noex ;
static int	ids_ngids(const ids *) noex ;


/* local variables */

static idsreserve	ids_data ;

static constexpr gid_t	gidend = mkgidend() ;


/* exported subroutines */

int ids_load(ids *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = ids_ctor(op)) >= 0) {
	    if ((rs = ids_data) >= 0) {
	        cint	size = ((rs+1)*sizeof(gid_t)) ;
	        void	*vp{} ;
	        n = rs ;
	        if ((rs = uc_libmalloc(size,&vp)) >= 0) {
		    op->gids = (gid_t *) vp ;
		    if ((rs = u_getgroups(n,op->gids)) >= 0) {
		        op->gids[n] = gid_t(-1) ;
		    }
		    if (rs < 0) {
		        uc_libfree(op->gids) ;
		        op->gids = nullptr ;
		    }
	        } /* end if (m-a) */
	    } /* end if (ids_data) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ids_load) */

int ids_release(ids *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->gids) {
	        rs1 = uc_libfree(op->gids) ;
	        if (rs >= 0) rs = rs1 ;
	        op->gids = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ids_release) */

int ids_ngroups(ids *op) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->gids) {
	        for (n = 0 ; op->gids[n] != gidend ; n += 1) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ids_ngroups) */

int ids_refresh(ids *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->gids) {
	        rs1 = uc_libfree(op->gids) ;
	        if (rs >= 0) rs = rs1 ;
	        op->gids = nullptr ;
	    }
	    if (rs >= 0) rs = ids_load(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ids_refresh) */

int ids_copy(ids *op,const ids *otherp) noex {
	int		rs = SR_FAULT ;
	if (op && otherp) {
	    op->uid = otherp->uid ;
	    op->euid = otherp->euid ;
	    op->gid = otherp->gid ;
	    op->egid = otherp->egid ;
	    if ((rs = ids_ngids(otherp)) >= 0) {
	        cint	n = rs ;
	        int	size = 0 ;
	        void	*p ;
	        size += ((n+1)*sizeof(gid_t)) ;
	        if ((rs = uc_libmalloc(size,&p)) >= 0) {
	            int		i ;
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
/* end subroutine (ids_refresh) */


/* private subroutines */

static int ids_ctor(ids *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
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

static int ids_ngids(const ids *op) noex {
	int		n = 0 ;
	if (op->gids) {
	    for (n = 0 ; op->gids[n] != gidend ; n += 1) ;
	}
	return n ;
}
/* end subroutine (ids_ngids) */

idsreserve::operator int () noex {
	int		rs ;
	if ((rs = ngroups) == 0) {
	    gid_t	dummy[1] ;
	    rs = u_getgroups(0,dummy) ;
	    ngroups = rs ;
	}
	return rs ;
}
/* end subroutine (idsreserve::operator) */

ids_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
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
	return rs ;
}
/* end method (ids_co::operator) */

int ids::copy(const ids *oip) noex {
	return ids_copy(this,oip) ;
}


