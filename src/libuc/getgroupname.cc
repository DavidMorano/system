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
#include	<sys/types.h>
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


/* forward references */

static int	isNotOurs(int) noex ;


/* local variables */

constexpr int	rsnotours[] = {
	SR_SEARCH,
	SR_NOTFOUND,
	0
} ;

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int getgroupname(char *gbuf,int glen,gid_t gid) noex {
	const gid_t	ourgid = getgid() ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*vn = varname.groupname ;
	if (gbuf) {
	    rs = SR_OVERFLOW ;
	    if (gid == gidend) gid = ourgid ;
	    if (glen > 0) {
	        char		*grbuf{} ;
	        static cchar	*vgn = getenv(vn) ;
	        if ((rs = malloc_gr(&grbuf)) >= 0) {
	            GROUP	gr ;
	            cint	grlen = rs ;
		    cchar	*gn = nullptr ;
	            if ((gid == ourgid) && vgn) {
	                if ((rs = getgr_name(&gr,grbuf,grlen,vgn)) >= 0) {
	                    if (gr.gr_gid == gid) {
				gn = vgn ;
			    } else {
			        rs = SR_SEARCH ;
		            }
		        }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	            if (isNotOurs(rs)) {
	                rs = getgr_gid(&gr,grbuf,grlen,gid) ;
		        gn = gr.gr_name ;
	            }
	            if ((rs >= 0) && gn) {
	                rs = sncpy1(gbuf,glen,gn) ;
			len = rs ;
	            } else if (isNotOurs(rs)) {
	                rs = snsd(gbuf,glen,"G",gid) ;
			len = rs ;
	            }
	            rs1 = uc_free(grbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getgroupname) */


/* local subroutines */

static int isNotOurs(int rs) noex {
	return isOneOf(rsnotours,rs) ;
}
/* end subroutine (isNotOurs) */


