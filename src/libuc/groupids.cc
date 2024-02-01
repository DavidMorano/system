/* groupids SUPPORT */
/* lang=C++20 */

/* group-ID fetch (UNIX® group-IDs) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	groupids

	Description:
	This subroutine will load up the supplementary group IDs.

	Synopsis:
	int groupids_start(gid_t **gpp = nullptr) noex

	Arguments:
	gpp		pointer to hold pointer-to-gid array

	Returns:
	<0		error
	>=0		number of group IDs returned

	Notes:
	We eschew the use of NGROUPS_MAX and rather allocate
	dynamically.

	Example-usage:
	{
	    groupids	g ;
	    int		rs ;
	    int		rs1 ;
	    if ((rs = g.start) >= 0) {
	        {
		    gid_t	*gids = g.gids ;

		}
		rs1 = g.finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	}

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>		/* for |getgroups(2)| */
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"groupids.hh"


/* local defines */


/* local namespaces */

using std::nullptr_t ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr gid_t	gidend = gid_t(-1) ;


/* exported subroutines */

int groupids::get(gid_t **gpp) noex {
	int		rs = SR_FAULT ;
	if (gpp) {
	    *gpp = gids ;
	    rs = ng ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (groupids::get) */


/* local subroutines */

int groupids::istart(gid_t **gpp) noex {
	int		rs ;
	nullptr_t	np{} ;
	if ((rs = u_getgroups(0,np)) >= 0) {
	        cint	size = ((rs+1)*sizeof(gid_t)) ;
	        void	*vp{} ;
	        ng = rs ;
	        if ((rs = uc_libmalloc(size,&vp)) >= 0) {
		    gids = (gid_t *) vp ;
		    if ((rs = u_getgroups(ng,gids)) >= 0) {
		        gids[ng] = gidend ;
			if (gpp) {
			    *gpp = gids ;
			}
		    }
		    if (rs < 0) {
		        uc_libfree(gids) ;
		        gids = nullptr ;
		    }
	        } /* end if (m-a) */
	} /* end if (u_getgroups) */
	return (rs >= 0) ? ng : rs ;
}
/* end method (groupids::istart) */

int groupids::ifinish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (gids) {
	    rs1 = uc_libfree(gids) ;
	    if (rs >= 0) rs = rs1 ;
	    gids = nullptr ;
	} /* end if (non-null) */
	ng = 0 ;
	return rs ;
}
/* end subroutine (groupids::ifinish) */

groupids_ster::operator int () noex {
	return op->istart(nullptr) ;
}
/* end method (groupids_ster::operator) */

int groupids_ster::operator () (gid_t **gpp) noex {
	return op->istart(gpp) ;
}
/* end method (groupids_ster::operator) */

groupids_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	switch (w) {
	case groupidsmem_ngroups:
	    rs = op->ng ;
	    break ;
	case groupidsmem_finish:
	    rs = op->ifinish() ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (groupids_co::operator) */


