/* perm */
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
	perm

	Description:
	This module is sort of the "effective_user" version of
	|access(2)|.  Implemented within this module are the following
	interfaces:
 	+ perm(3uc)
	+ fperm(3uc)
	+ sperm(3uc)

	Synopsis:
	int perm(cchar *fname,uid_t uid,gid_t gid,gid_t *groups,int am) noex

	Arguments:
	fname	filename to check
	uid	UID to use for the check
	gid	GID to use for the check
	groups	the secondary GIDs to use for check
	am	the access-mode as specified like with |open(2)| but only
		the lower 3 bits are used, like with |access(2)|

	Returns:
	0	access if allowed
	<0	access if denied for specified error code

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<ids.h>
#include	<localmisc.h>

#include	"xperm.h"


/* local defines */

#define	TRYER	struct tryer


/* external subroutines */

extern int	getngroups() noex ;


/* external variables */


/* local structures */

struct tryer {
	gid_t		*gids ;
	uid_t		euid ;
	gid_t		egid ;
	int		f_gidalloc ;
} ;


/* forward references */

static int permer(USTAT *,uid_t,gid_t,gid_t *,int) noex ;

static int try_start(TRYER *,uid_t,gid_t,gid_t *) noex ;
static int try_finish(TRYER *) noex ;
static int try_root(TRYER *,USTAT *,int) noex ;
static int try_user(TRYER *,USTAT *,int) noex ;
static int try_group(TRYER *,USTAT *,int) noex ;
static int try_other(TRYER *,USTAT *,int) noex ;


/* local variables */

static int	(*tries[])(TRYER *,USTAT *,int) = {
	try_root,
	try_user,
	try_group,
	try_other,
	nullptr
} ;

static constexpr uid_t	uidany = (-1) ;
static constexpr gid_t	gidany = (-1) ;


/* exported subroutines */

int perm(cchar *fn,uid_t euid,gid_t egid,gid_t *gids,int am) noex {
	int		rs = SR_FAULT ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        USTAT	sb ;
	        if ((rs = uc_stat(fn,&sb)) >= 0) {
	            rs = permer(&sb,euid,egid,gids,am) ;
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (perm) */

int fperm(int fd,uid_t euid,gid_t egid,gid_t *gids,int am) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    USTAT	sb ;
	    if ((rs = uc_fstat(fd,&sb)) >= 0) {
	        rs = permer(&sb,euid,egid,gids,am) ;
	    } /* end if (stat) */
	}
	return rs ;
}
/* end subroutine (fperm) */

int sperm(ids *idp,USTAT *sbp,int am) noex {
	int		rs = SR_FAULT ;
	if (idp && sbp) {
	    const uid_t	euid = idp->euid ;
	    const gid_t	egid = idp->egid ;
	    gid_t	*gids = idp->gids ;
	    rs = permer(sbp,euid,egid,gids,am) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sperm) */


/* local subroutines */

static int permer(USTAT *sbp,uid_t euid,gid_t egid,gid_t *gids,int am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sbp) {
	    rs = SR_OK ;
	    if (am != 0) {
	        TRYER	t{} ;
	        am &= 007 ;
	        if ((rs = try_start(&t,euid,egid,gids)) >= 0) {
	            for (int i = 0 ; tries[i] ; i += 1) {
	                rs = (*tries[i])(&t,sbp,am) ;
	                if (rs != 0) break ;
	            } /* end for */
	            if (rs == 0) rs = SR_ACCESS ;
	            rs1 = try_finish(&t) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (try) */
	    } /* end if (access mode) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (perm) */

static int try_start(TRYER *tip,uid_t eu,gid_t eg,gid_t *gids) noex {
	int		rs  ;
	if (eu == uidany) eu = geteuid() ;
	if (eg == gidany) eg = getegid() ;
	tip->euid = eu ;
	tip->egid = eg ;
	tip->gids = gids ;
	if ((rs = getngroups()) >= 0) {
	    cint	n = rs ;
	    if (gids == nullptr) {
	        cint	gsize = ((n+1)*sizeof(gid_t)) ;
	        void	*p = nullptr ;
	        if ((rs = uc_libmalloc(gsize,&p)) >= 0) {
		    tip->gids = (gid_t *) p ;
		    tip->f_gidalloc = true ;
	            if ((rs = u_getgroups(n,tip->gids)) >= 0) {
		        tip->gids[rs] = -1 ;
		    }
		    if (rs < 0) {
		        uc_libfree(tip->gids) ;
		        tip->gids = nullptr ;
		        tip->f_gidalloc = false ;
	            }
	        }
	    } /* end if (empty GIDs) */
	} /* end if (getngroups) */
	return rs ;
}
/* end subroutine (try_start) */

static int try_finish(TRYER *tip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tip->f_gidalloc) {
	    tip->f_gidalloc = false ;
	    rs1 = uc_libfree(tip->gids) ;
	    if (rs >= 0) rs = rs1 ;
	    tip->gids = nullptr ;
	}
	return rs ;
}
/* end subroutine (try_finish) */

static int try_root(TRYER *tip,USTAT *,int) noex {
	return (tip->euid == 0) ;
}
/* end subroutine (try_root) */

static int try_user(TRYER *tip,USTAT *sbp,int am) noex {
	int		rs = SR_OK ;
	if (tip->euid == sbp->st_uid) {
	    cint	um = (sbp->st_mode >> 6) ;
	    rs = ((um & am) == am) ? 1 : SR_ACCES ;
	}
	return rs ;
}
/* end subroutine (try_user) */

static int try_group(TRYER *tip,USTAT *sbp,int am) noex {
	int		rs = SR_OK ;
	int		gm ;
	gm = (sbp->st_mode >> 3) ;
	if (tip->egid == sbp->st_gid) {
	    rs = ((gm & am) == am) ? 1 : SR_ACCES ;
	} else if (tip->gids != nullptr) {
	    bool	f = false ;
	    for (int i = 0 ; tip->gids[i] != gidany ; i += 1) {
	        f = (sbp->st_gid == tip->gids[i]) ;
		if (f) break ;
	    } /* end for */
	    if (f) {
	        rs = ((gm & am) == am) ? 1 : SR_ACCES ;
	    }
	}
	return rs ;
}
/* end subroutine (try_group) */

static int try_other(TRYER *,USTAT *sbp,int am) noex {
	cint		om = sbp->st_mode ;
	return ((om & am) == am) ;
}
/* end subroutine (try_other) */


