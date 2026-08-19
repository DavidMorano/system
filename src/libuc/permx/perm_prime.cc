/* perm_prime SUPPORT */
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
	permids
	permidf
	perm
	permf

	Description:
	This module is sort of the "effective_user" version of
	|access(2)|.  Implemented within this module are the following
	interfaces:
 	+ perm(3uc)
	+ permf(3uc)
	+ permids(3uc)
	+ permidf(3uc)

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
#include	<sys/types.h>		/* POSIX UNIX® system types */
#include	<sys/stat.h>		/* POSIX |S_{x}| */
#include	<unistd.h>		/* POSIX |get{x}{y}id(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsysconf.h>		/* LIBUC |ucmaxgroups(3uc)| */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"permx.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef const gid_t	cgid ;


/* external subroutines */


/* external variables */


/* local structures */

enum pfields {
	pfield_oth,
	pfield_grp,
	pfield_usr,
	pfield_overlast
} ; /* end enum (pfields) */

/* permissoin bits per field */
constexpr int		pbits = pfield_overlast ;	
constexpr int		pmask = S_IRWXO ;

constexpr local int	mkperms(int) noex ;

namespace {
    constexpr int	tablen = (1 << pbits) ;
    struct permtaber {
	uchar	tab[tablen] = {} ;
	constexpr permtaber() noex {
	    for (int i = 0 ; i < tablen ; i += 1) {
		if (i & R_OK) tab[i] |= S_IROTH ;
		if (i & W_OK) tab[i] |= S_IWOTH ;
		if (i & X_OK) tab[i] |= S_IXOTH ;
	    } /* end for */
	} ; /* end ctor */
	constexpr int operator [] (int a) const noex {
	    return tab[a] ;
	} ; /* end method */
    } ; /* end struct (permtaber) */
} /* end namespace */

namespace {
    struct tryer ;
    typedef int (tryer::*tryer_m)(const ustat *) noex ;
    struct tryer {
	gid_t		*gids ; /* <- possibly allocated */
	uid_t		euid ;
	gid_t		egid ;
	int		pm ;		/* permissions-mask */
	bool		f_gidalloc = false ;
	bool		fdone = false ;
	int start	() noex ;
	int checkpm	(const ustat *,int) const noex ;
	int root	(const ustat *) noex ;
	int usr		(const ustat *) noex ;
	int grp		(const ustat *) noex ;
	int oth		(const ustat *) noex ;
	int finish	() noex ;
	tryer(uid_t,gid_t,const gid_t *,int) noex ;
    } ; /* end struct (tryer) */
} /* end namespace */


/* forward references */

local int permer(custat *,uid_t,gid_t,const gid_t *,int) noex ;


/* local variables */

constexpr tryer_m	tries[] = {
	&tryer::root,
	&tryer::usr,
	&tryer::grp,
	&tryer::oth
} ; /* end array (tries) */

constexpr uid_t		uidend = (-1) ;
constexpr gid_t		gidend = (-1) ;

constexpr permtaber	perms ;


/* exported variables */


/* exported subroutines */

int perm(cchar *fn,uid_t euid,gid_t egid,const gid_t *gids,int am) noex {
	int		rs = SR_FAULT ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
	        if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	            rs = permer(&sb,euid,egid,gids,am) ;
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (perm) */

int permf(int fd,uid_t euid,gid_t egid,const gid_t *gids,int am) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) ylikely {
	    if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
	        rs = permer(&sb,euid,egid,gids,am) ;
	    } /* end if (stat) */
	}
	return rs ;
} /* end subroutine (permf) */

int permids(ids *idp,custat *sbp,int am) noex {
	int		rs = SR_FAULT ;
	if (idp && sbp) ylikely {
	    const uid_t		euid = idp->euid ;
	    const gid_t		egid = idp->egid ;
	    const gid_t		*gids = idp->gids ;
	    rs = permer(sbp,euid,egid,gids,am) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (permids) */

extern int permidf(ids *idp,cchar *fname,int am) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        rs = permids(idp,&sb,am) ;
	    }
	} /* end if (u_stat) */
	return rs ;
} /* end subroutine (peridf) */


/* local subroutines */

local int permer(custat *sbp,uid_t euid,gid_t egid,cgid *gids,int am) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sbp) ylikely {
	    rs = SR_OK ;
	    if (am) ylikely {
	        if (tryer t(euid,egid,gids,am) ; (rs = t.start()) >= 0) {
		    {
	                for (cauto &m : tries) {
	                    rs = (t.*m)(sbp) ;
	                    if (rs || t.fdone) break ;
	                } /* end for */
	                if (rs == 0) rs = SR_ACCESS ;
		    }
	            rs1 = t.finish() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (tryer) */
	    } /* end if (non-zero postive access-mode) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (permer) */

tryer::tryer(uid_t eu,gid_t eg,const gid_t *gs,int am) noex {
	if (eu == uidend) eu = geteuid() ;
	if (eg == gidend) eg = getegid() ;
	gids = cast_const<gid_t *>(gs) ;
	euid = eu ;
	egid = eg ;
	pm = mkperms(am) ;
} /* end ctor (tryer::ctor) */

int tryer::start() noex {
	int		rs  ;
	if ((rs = ucmaxgroups) >= 0) ylikely {
	    cint	ng = rs ;
	    if (gids == nullptr) {
	        cint	gsz = ((ng + 1) * szof(gid_t)) ;
	        if (void *vp ; (rs = lm_mall(gsz,&vp)) >= 0) ylikely {
		    gids = (gid_t *) vp ;
		    f_gidalloc = true ;
	            if ((rs = u_getgroups(ng,gids)) >= 0) ylikely {
		        gids[rs] = gidend ;
		    }
		    if (rs < 0) {
		        lm_free(gids) ;
		        gids = nullptr ;
		        f_gidalloc = false ;
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (empty GIDs) */
	} /* end if (getngroups) */
	return rs ;
} /* end method (tryer::start) */

int tryer::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (f_gidalloc && gids) ylikely {
	    f_gidalloc = false ;
	    rs1 = lm_free(gids) ;
	    if (rs >= 0) rs = rs1 ;
	    gids = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (tryer::finish) */

int tryer::checkpm(const ustat *sbp,int pfield) const noex {
	return (((sbp->st_mode >> (pfield * pbits)) & pm) == pm) ;
} /* end method (tryer::checkpm) */

int tryer::root(const ustat *) noex {
	return (euid == 0) ;
} /* end method (tryer::root) */

int tryer::usr(const ustat *sbp) noex {
	int		rs = SR_OK ;
	if (euid == sbp->st_uid) {
	    cint	f = checkpm(sbp,pfield_usr) ;
	    rs = (f) ? 1 : SR_ACCES ;
	    fdone = true ;
	}
	return rs ;
} /* end method (tryer::usr) */

int tryer::grp(const ustat *sbp) noex {
	int		rs = SR_OK ;
	if (egid == sbp->st_gid) {
	    rs = checkpm(sbp,pfield_grp) ? 1 : SR_ACCES ;
	    fdone = true ;
	} else if (gids != nullptr) {
	    bool	f = false ;
	    for (int i = 0 ; gids[i] != gidend ; i += 1) {
	        f = (sbp->st_gid == gids[i]) ;
		if (f) break ;
	    } /* end for */
	    if (f) {
	        rs = checkpm(sbp,pfield_grp) ? 1 : SR_ACCES ;
		fdone = true ;
	    }
	} /* end if */
	return rs ;
} /* end method (tryer::grp) */

int tryer::oth(const ustat *sbp) noex {
	cint f = checkpm(sbp,pfield_oth) ;
	return (f) ? 1 : SR_ACCESS ;
} /* end method (tryer::oth) */

constexpr local int mkperms(int am) noex {
    	am &= pmask ;
    	return perms[am] ;
} /* end subroutine (mkperms) */


