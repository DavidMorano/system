/* getgroupname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a groupname by GID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine is being written for use by PCS programs,
	but it obviously has wider applications. It is simple, but
	effective!

	= 2019-01-05, David A-D- Morano
	Enhanced error checking for |bufsizeget(3uc)|.
	
*/

/* Copyright © 1998,2019 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getgroupname

	Description:
	Get a groupname given a GID.

	Synopsis:
	int getgroupname(char *rbuf,int rlen,gid_t gid) noex

	Arguements:
	buf		supplied buffer to receive groupname
	buflen		length of supplied buffer
	gid		GID of group to get

	Returns:
	>=0		length of return groupname
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |gid_t| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<grp.h>			/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getgroupname.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct helper ;
    typedef int (helper::*helper_m)() noex ;
    struct helper {
	cchar		*vgn = nullptr ;
	char		*grbuf = nullptr ;
	char		*rbuf ;
	ucentgr		gr ;
	int		grlen ;
	int		rlen ;
	gid_t		gid ;
	gid_t		ourgid ;
	helper(char *b,int l,gid_t g) noex : rbuf(b), rlen(l), gid(g) { 
	    ourgid = getgid() ;
	} ;
	int start(cchar *) noex ;
	int finish() noex ;
	operator int () noex ;
	int tryus() noex ;
	int trygid() noex ;
	int trydef() noex ;
    } ; /* end struct (helper) */
} /* end namespace */


/* forward references */

local bool	isNotOurs(int) noex ;


/* local variables */

constexpr int		rsnotours[] = {
	SR_SEARCH,
	SR_NOTFOUND,
	0
} ; /* end array (rsnotours) */

constexpr helper_m	tries[] = {
	&helper::tryus,
	&helper::trygid,
	&helper::trydef
} ; /* end array (tries) */

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int getgroupname(char *gbuf,int glen,gid_t gid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*vn = varname.groupname ;
	if (gbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (glen > 0) ylikely {
	        static cchar	*vgn = getenver(vn) ;
		if (helper ho(gbuf,glen,gid) ; (rs = ho.start(vgn)) >= 0) {
		    {
			rs = ho ;
			len = rs ;
		    }
		    rs1 = ho.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (helper) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getgroupname) */


/* local subroutines */

int helper::start(cchar *vgp) noex {
	int		rs ;
	if (gid == gidend) gid = ourgid ;
	vgn = vgp ;
	if ((rs = lm_gr(&grbuf)) >= 0) {
	    grlen = rs ;
	}
	return rs ;
} /* end method */

int helper::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (grbuf) {
	    rs1 = lm_free(grbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    grbuf = nullptr ;
	    grlen = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method */

helper::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs) break ;
	} /* end for */
	return rs ;
} /* end method */

int helper::tryus() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if ((gid == ourgid) && vgn) ylikely {
	    if ((rs = getgr_name(&gr,grbuf,grlen,vgn)) >= 0) {
		if (gr.gr_gid == gid) {
	            rs = sncpy(rbuf,rlen,vgn) ;
		    len = rs ;
		}
	    } else if (isNotOurs(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (is us) */
	return (rs >= 0) ? len : rs ;
} /* end method */

int helper::trygid() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = getgr_gid(&gr,grbuf,grlen,gid)) >= 0) {
	    cchar	*gn = gr.gr_name ;
	    rs = sncpy(rbuf,rlen,gn) ;
	    len = rs ;
	} else if (isNotOurs(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
} /* end method */

int helper::trydef() noex {
	return snsd(rbuf,rlen,"G",gid) ;
} /* end method */

local bool isNotOurs(int rs) noex {
	return isOneOf(rsnotours,rs) ;
} /* end subroutine (isNotOurs) */


