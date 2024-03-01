/* getgroupname SUPPORT */
/* lang=C++20 */

/* get a groupname by GID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine is being written for use by PCS programs,
	but it obviously has wider applications. It is simple, but
	effective!

	= 2019-01-05, David A-D- Morano
	Enhanced error checking for |getbufsize(3uc)|.
	
*/

/* Copyright Â© 1998,2019 David AÂ­DÂ­ Morano.  All rights reserved. */

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
#include	<sys/types.h>		/* |gid_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<grp.h>
#include	<usystem.h>
#include	<varnames.hh>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"getgroupname.h"


/* local defines */


/* external subroutines */


/* local structures */

namespace {
    struct helper ;
    typedef int (helper::*helper_m)() noex ;
    struct helper {
	cchar		*vgn = nullptr ;
	char		*grbuf = nullptr ;
	char		*rbuf ;
	GROUP		gr ;
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
	int isus() noex ;
	int getgid() noex ;
	int def() noex ;
    } ; /* end struct (helper) */
}


/* forward references */

static int	isNotOurs(int) noex ;


/* local variables */

constexpr int	rsnotours[] = {
	SR_SEARCH,
	SR_NOTFOUND,
	0
} ;

constexpr helper_m	mems[] = {
	&helper::isus,
	&helper::getgid,
	&helper::def,
	nullptr
} ;

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int getgroupname(char *gbuf,int glen,gid_t gid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*vn = varname.groupname ;
	if (gbuf) {
	    rs = SR_OVERFLOW ;
	    if (glen > 0) {
		helper		ho(gbuf,glen,gid) ;
	        static cchar	*vgn = getenv(vn) ;
		if ((rs = ho.start(vgn)) >= 0) {
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
}
/* end subroutine (getgroupname) */


/* local subroutines */

int helper::start(cchar *vgp) noex {
	int		rs ;
	if (gid == gidend) gid = ourgid ;
	vgn = vgp ;
	if ((rs = malloc_gr(&grbuf)) >= 0) {
	    grlen = rs ;
	}
	return rs ;
}

int helper::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (grbuf) {
	    rs1 = uc_free(grbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    grbuf = nullptr ;
	    grlen = 0 ;
	}
	return rs ;
}

helper::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && mems[i] ; i += 1) {
	    helper_m	m = mems[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}

int helper::isus() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if ((gid == ourgid) && vgn) {
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
}

int helper::getgid() noex {
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
}

int helper::def() noex {
	return snsd(rbuf,rlen,"G",gid) ;
}

static int isNotOurs(int rs) noex {
	return isOneOf(rsnotours,rs) ;
}
/* end subroutine (isNotOurs) */


