/* userid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this object implements something of a user-identity */
/* version %I% last-modified %G% */


/* revision history:

	- 1996-11-21, Dave morano
	This subroutine code was started by copying from some other
	program (one of the other PCS remote host access programs
	or subroutines).

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	userid

	Description:
	This object implment a user-identity.

	Synopsis:
	int userid_start(userid *op,cc *un,cc *gn,uid_t uid,gid_t gid) noex
	int userid_finish() noex ;
	
	Arguments:
	un		c-string of user-name
	gn		c-string of group-name
	uid		user-ID
	gid		group-ID

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |stpcpy(3c)| */
#include	<string>
#include	<usystem.h>
#include	<getusername.h>
#include	<getgroupname.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"userid.hh"

import libutil ;

/* local defines */


/* imported namespaces */

using std::string ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct submgr_fl {
	uint		tun:1 ;
	uint		tgn:1 ;
    } ; /* end struct (submgr_fl) */
    struct submgr {
	string		tun ;
	string		tgn ;
	userid		*op ;
	cchar		*un ;
	cchar		*gn ;
	uid_t		uid ;
	gid_t		gid ;
	submgr_fl	fl{} ;
	submgr(userid *p,cc *aun,cc *agn,uid_t u,gid_t g) noex : op(p) {
	    un = aun ;
	    gn = agn ;
	    uid = u ;
	    gid = g ;
	} ;
	operator int () noex ;
	int getun() noex ;
	int getgn() noex ;
	int loadgn(gid_t) noex ;
	int alloc() noex ;
	void loadids(const ucentpw *) noex ;
    } ; /* end struct(submgr) */
} /* end namespace */


/* forward subroutines */


/* local variables */

const uid_t		uidend = -1 ;
const gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int userid_start(userid *op,cc *un,cc *gn,uid_t uid,gid_t gid) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    submgr so(op,un,gn,uid,gid) ;
	    rs = so ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userid_start) */

int userid_finish(userid *op) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->username) {
		char *bp = cast_const<charp>(op->username) ;
		rs1 = uc_free(bp) ;
		if (rs >= 0) rs = rs1 ;
		op->username = nullptr ;
		op->groupname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userid_finish) */


/* private subroutines */

submgr::operator int () noex {
    	int		rs = SR_OK ;
	if (un) {
	    op->username = un ;
	    if (gn) {
		op->groupname = gn ;
	    } else {
		rs = getgn() ;
	    }
	} else {
	    rs = getun() ;
	}
	if (rs >= 0) {
	    rs = alloc() ;
	}
	return rs ;
} /* end method (submgr::operator) */

int submgr::getgn() noex {
    	int		rs ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    cint	pwlen = rs ;
	    if (ucentpw pw ; (rs = pw.getnam(pwbuf,pwlen,un)) >= 0) {
		if (gn == nullptr) {
		    rs = loadgn(pw.pw_gid) ;
		}
		loadids(&pw) ;
	    } /* end if (pw.getname) */
	    rs = rsfree(rs,pwbuf) ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (submgr::getgn) */

int submgr::getun() noex {
    	int		rs ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    cint	pwlen = rs ;
	    if (ucentpw pw ; (rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
		{
		    tun = pw.pw_name ;
		    fl.tun = true ;
		}
		if (gn == nullptr) {
		    rs = loadgn(pw.pw_gid) ;
		}
		loadids(&pw) ;
	    } /* end if (getpwusername) */
	    rs = rsfree(rs,pwbuf) ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (submgr::getun) */

int submgr::loadgn(gid_t pwgid) noex {
    	int		rs ;
        if (char *gnbuf ; (rs = malloc_gn(&gnbuf)) >= 0) {
            if ((rs = getgroupname(gnbuf,rs,pwgid)) >= 0) {
                try {
                    tgn = gnbuf ;
                    fl.tgn = true ;
                } catch (...) {
                    rs = SR_NOMEM ;
                }
            } /* end if (getgroupname) */
            rs = rsfree(rs,gnbuf) ;
        } /* end if (m-a-f) */
	return rs ;
} /* end method (submgr::loadgn) */

int submgr::alloc() noex {
    	int		rs = SR_OK ;
	int		sz = 0 ;
	cchar		*ucp{} ;
	cchar		*gcp{} ;
	if (un) {
	    sz = (lenstr(un) + 1) ;
	    ucp = un ;
	} else {
	    sz += int(tun.size() + 1) ;
	    ucp = tun.c_str() ;
	}
	if (gn) {
	    sz += (lenstr(gn) + 1) ;
	    gcp = gn ;
	} else {
	    sz += int(tgn.size() + 1) ;
	    gcp = tgn.c_str() ;
	}
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    op->username = a ;
	    a = (stpcpy(a,ucp) + 1) ;
	    op->groupname = a ;
	    a = (stpcpy(a,gcp) + 1) ;
	} /* end if (memory-allocation) */
	return rs ;
} /* end method (submgr::alloc) */

void submgr::loadids(const ucentpw *pwp) noex {
	op->uid = (uid == uidend) ? pwp->pw_uid : uid ;
	op->gid = (gid == gidend) ? pwp->pw_gid : gid ;
} /* end method (submgr::loadids) */

void userid::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("userid",rs,"dtor-finish") ;
	}
} /* end method (submgr::dtor) */

userid_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case useridmem_finish:
	        rs = userid_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (userid_co::operator) */


