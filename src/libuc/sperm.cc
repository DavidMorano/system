/* sperm */
/* lang=C++20 */

/* test the permissions on a file -- similar to |access(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sperm

	Description:
	This subroutine is sort of the "effective_user" version of
	|access(2)|.  It has more features that enable it to perform
	the function of |access(2)| as well as other variations.

	Synopsis:
	int sperm(ids *idp,USTAT *sbp,int am) noex

	Arguments:
	idp	pointer to ids object
	sbp	pointer to file status structure (see |stat(2)|)
	am	the access-mode as specified like with }open(2)| 
		but only the lower 3 bits are used, like with 
		|access(2)|

	Returns:
	0	access is allowed
	<0	access is denied for specified error code

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<ids.h>
#include	<localmisc.h>

#include	"xperm.h"


/* local namespaces */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct tryer {
	USTAT		*sbp ;
	ids		*idp ;
	int		am ;
} ;


/* forward references */

static int try_start(tryer *,ids *,USTAT *,int) noex ;
static int try_finish(tryer *) noex ;
static int try_filetype(tryer *) noex ;
static int try_root(tryer *) noex ;
static int try_user(tryer *) noex ;
static int try_group(tryer *) noex ;
static int try_other(tryer *) noex ;

consteval gid_t mkgidend() noex {
	gid_t	m = 0 ;
	m = (~m) ;
	return m ;
}


/* local variables */

static int	(*tries[])(tryer *) = {
	try_filetype,
	try_root,
	try_user,
	try_group,
	try_other,
	nullptr
} ;

static constexpr gid_t	gidend = mkgidend() ;


/* exported subroutines */

int sperm(ids *idp,USTAT *sbp,int am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (idp && sbp) {
	    rs = SR_OK ;
	    if (am != 0) {
	        tryer	t ;
	        if ((rs = try_start(&t,idp,sbp,am)) >= 0) {
		    {
	                for (int i = 0 ; tries[i] ; i += 1) {
	                    rs = (*tries[i])(&t) ;
	                    if (rs != 0) break ;
	                } /* end for */
	                if (rs == 0) rs = SR_ACCESS ;
		    }
	            rs1 = try_finish(&t) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (try) */
	    } /* end if (access mode) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sperm) */


/* local subroutines */

static int try_start(tryer *tip,ids *idp,USTAT *sbp,int am) noex {
	tip->idp = idp ;
	tip->sbp = sbp ;
	tip->am = am ;
	return SR_OK ;
}
/* end subroutine (try_start) */

static int try_finish(tryer *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (try_finish) */

static int try_filetype(tryer *tip) noex {
	USTAT		*sbp = tip->sbp ;
	cint		ft = (tip->am & S_IFMT) ;
	int		rs = SR_OK ;
	if ((ft != 0) && ((sbp->st_mode & S_IFMT) != ft)) {
	    rs = SR_NOMSG ;
	}
	return rs ;
}
/* end subroutine (try_filetype) */

static int try_root(tryer *tip) noex {
	ids		*idp = tip->idp ;
	return (idp->euid == 0) ;
}
/* end subroutine (try_root) */

static int try_user(tryer *tip) noex {
	USTAT		*sbp = tip->sbp ;
	ids		*idp = tip->idp ;
	cint		am = (tip->am & 0007) ;
	int		rs = SR_OK ;
	if (idp->euid == sbp->st_uid) {
	    const int	um = (sbp->st_mode >> 6) ;
	    rs = ((um&am) == am) ? 1 : SR_ACCES ;
	}
	return rs ;
}
/* end subroutine (try_user) */

static int try_group(tryer *tip) noex {
	USTAT		*sbp = tip->sbp ;
	ids		*idp = tip->idp ;
	cint		am = (tip->am & 0007) ;
	int		rs = SR_OK ;
	int		gm ;
	gm = (sbp->st_mode >> 3) ;
	if (idp->egid == sbp->st_gid) {
	    rs = ((gm&am) == am) ? 1 : SR_ACCES ;
	} else if (idp->gids) {
	    bool	f = false ;
	    for (int i = 0 ; idp->gids[i] != gidend ; i += 1) {
	        f = (sbp->st_gid == idp->gids[i]) ;
		if (f) break ;
	    } /* end for */
	    if (f) {
	        rs = ((gm & am) == am) ? 1 : SR_ACCES ;
	    }
	}
	return rs ;
}
/* end subroutine (try_group) */

static int try_other(tryer *tip) noex {
	USTAT		*sbp = tip->sbp ;
	cint		am = (tip->am & 0007) ;
	int		rs = SR_OK ;
	{
	    cint	om = sbp->st_mode ;
	    rs = ((om & am) == am) ;
	}
	return rs ;
}
/* end subroutine (try_other) */


