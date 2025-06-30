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
	int userid(cc *username,cc *groupname,uid_t uid,gid_t gid) noex
	
	Arguments:
	username	c-string of user-name
	groupname	c-string of group-name
	uid		user-ID
	gid		group-ID

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"userid.hh"


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
	int getgn() noex ;
	int alloc() noex ;
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
		rs = 0 ;
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
		rs = 0 ;
	    } /* end if (pw.getname) */
	    rs = rsfree(rs,pwbuf) ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (submgr::getgn) */

int submgr::alloc() noex {
    	int		rs = SR_OK ;
	int		sz = 0 ;
	(void) sz ;


	return rs ;
} /* end method (submgr::alloc) */

