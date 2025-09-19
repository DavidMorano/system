/* permx_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test the permissions on a file -- similar to |access(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	perm
	permf
	permid

	Description:
	This module is sort of the "effective_user" version of
	|access(2)|.  Implemented within this module are the following
	interfaces:
 	+ perm(3uc)
	+ permf(3uc)
	+ permid(3uc)

	Synopsis:
	int perm(cchar *fname,uid_t uid,gid_t gid,cgid_t *groups,int am) noex

	Arguments:
	fname	filename to check
	uid	UID to use for the check
	gid	GID to use for the check
	groups	the secondary GIDs to use for check
	am	the access-mode as specified like with |open(2)| but only
		the lower 3 bits are used, like with |access(2)|

	Returns:
	>0	unused at present
	==0	access allowed
	<0	access denied for specified error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* UNIX® system types */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<usystem.h>
#include	<ids.h>
#include	<localmisc.h>

#include	"permx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef const gid_t		cgid ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct tryer ;
    typedef int (tryer::*tryer_m)(USTAT *,int) noex ;
    struct tryer {
	gid_t		*gids ; /* <- possibly allocated */
	uid_t		euid ;
	gid_t		egid ;
	bool		f_gidalloc = false ;
	int start() noex ;
	int root(USTAT *,int) noex ;
	int user(USTAT *,int) noex ;
	int grp(USTAT *,int) noex ;
	int other(USTAT *,int) noex ;
	int finish() noex ;
	tryer(uid_t,gid_t,const gid_t *) noex ;
    } ; /* end struct (tryer) */
}


/* forward references */

static int permer(USTAT *,uid_t,gid_t,const gid_t *,int) noex ;


/* local variables */

constexpr tryer_m	tries[] = {
	&tryer::root,
	&tryer::user,
	&tryer::grp,
	&tryer::other
} ;

constexpr uid_t		uidend = (-1) ;
constexpr gid_t		gidend = (-1) ;


/* exported variables */


/* exported subroutines */

int perm(cchar *fn,uid_t euid,gid_t egid,const gid_t *gids,int am) noex {
	int		rs = SR_FAULT ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        if (USTAT sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	            rs = permer(&sb,euid,egid,gids,am) ;
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (perm) */

int permf(int fd,uid_t euid,gid_t egid,const gid_t *gids,int am) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    if (USTAT sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
	        rs = permer(&sb,euid,egid,gids,am) ;
	    } /* end if (stat) */
	}
	return rs ;
}
/* end subroutine (permf) */

int permid(ids *idp,USTAT *sbp,int am) noex {
	int		rs = SR_FAULT ;
	if (idp && sbp) {
	    const uid_t		euid = idp->euid ;
	    const gid_t		egid = idp->egid ;
	    const gid_t		*gids = idp->gids ;
	    rs = permer(sbp,euid,egid,gids,am) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (permid) */


/* local subroutines */

static int permer(USTAT *sbp,uid_t euid,gid_t egid,cgid *gids,int am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sbp) {
	    rs = SR_OK ;
	    if (am != 0) {
	        tryer	t(euid,egid,gids) ;
	        am &= 007 ;
	        if ((rs = t.start()) >= 0) {
	            for (const auto m : tries) {
	                rs = (t.*m)(sbp,am) ;
	                if (rs != 0) break ;
	            } /* end for */
	            if (rs == 0) rs = SR_ACCESS ;
	            rs1 = t.finish() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (try) */
	    } /* end if (access mode) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (permer) */

tryer::tryer(uid_t eu,gid_t eg,const gid_t *gs) noex {
	if (eu == uidend) eu = geteuid() ;
	if (eg == gidend) eg = getegid() ;
	gids = cast_const<gid_t *>(gs) ;
	euid = eu ;
	egid = eg ;
}
/* end ctor (tryer::ctor) */

int tryer::start() noex {
	int		rs  ;
	if ((rs = ucmaxgroups) >= 0) {
	    cint	ng = rs ;
	    if (gids == nullptr) {
	        cint	gsize = ((ng + 1) * szof(gid_t)) ;
	        if (void *vp{} ; (rs = lm_mall(gsize,&vp)) >= 0) {
		    gids = (gid_t *) vp ;
		    f_gidalloc = true ;
	            if ((rs = u_getgroups(ng,gids)) >= 0) {
		        gids[rs] = gidend ;
		    }
		    if (rs < 0) {
		        lm_free(gids) ;
		        gids = nullptr ;
		        f_gidalloc = false ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (empty GIDs) */
	} /* end if (getngroups) */
	return rs ;
}
/* end method (tryer::start) */

int tryer::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (f_gidalloc && gids) {
	    f_gidalloc = false ;
	    rs1 = lm_free(gids) ;
	    if (rs >= 0) rs = rs1 ;
	    gids = nullptr ;
	}
	return rs ;
}
/* end method (tryer::finish) */

int tryer::root(USTAT *,int) noex {
	return (euid == 0) ;
}
/* end method (tryer::root) */

int tryer::user(USTAT *sbp,int am) noex {
	int		rs = SR_OK ;
	if (euid == sbp->st_uid) {
	    cint	um = (sbp->st_mode >> 6) ;
	    rs = ((um & am) == am) ? 1 : SR_ACCES ;
	}
	return rs ;
}
/* end method (tryer::user) */

int tryer::grp(USTAT *sbp,int am) noex {
	int		rs = SR_OK ;
	int		gm ;
	gm = (sbp->st_mode >> 3) ;
	if (egid == sbp->st_gid) {
	    rs = ((gm & am) == am) ? 1 : SR_ACCES ;
	} else if (gids != nullptr) {
	    bool	f = false ;
	    for (int i = 0 ; gids[i] != gidend ; i += 1) {
	        f = (sbp->st_gid == gids[i]) ;
		if (f) break ;
	    } /* end for */
	    if (f) {
	        rs = ((gm & am) == am) ? 1 : SR_ACCES ;
	    }
	}
	return rs ;
}
/* end method (tryer::grp) */

int tryer::other(USTAT *sbp,int am) noex {
	cint		om = sbp->st_mode ;
	return ((om & am) == am) ;
}
/* end method (tryer::other) */


