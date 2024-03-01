/* getgroupname SUPPORT */
/* lang=C++20 */

/* get a groupname by GID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	This subroutine is being written for use by PCS programs,
	but it obviously has wider applications. It is simple, but
	effective!

	= 2019-01-05, David A-D- Morano
	Enhanced error checking for |getbufsize(3uc)|.
	
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<grp.h>
#include	<usystem.h>
#include	<varnames.hh>
#include	<getbufsize.h>
#include	<getax.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"getgroupname.h"


/* local defines */

#ifndef	USERNAMELEN
#ifndef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

#ifndef	GROUPNAMELEN
#define	GROUPNAMELEN	USERNAMELEN
#endif

#ifndef	VARGROUPNAME
#define	VARGROUPNAME	"GROUPNAME"
#endif


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
	const gid_t	gid_our = getgid() ;
	int		rs ;
	int		rs1 ;

	if (gbuf == NULL) return SR_FAULT ;

	if (glen < 0) glen = GROUPNAMELEN ;

	if (gid == gidend) gid = gid_our ;

	if ((rs = getbufsize(getbufsize_gr)) >= 0) {
	    GROUP	gr ;
	    cint	grlen = rs ;
	    char	*grbuf{} ;
	    if ((rs = uc_malloc((grlen+1),&grbuf)) >= 0) {
	        cchar	*vn = VARGROUPNAME ;
	        cchar	*gn = NULL ;
	        if ((gid == gid_our) && ((gn = getenv(vn)) != NULL)) {
	            if ((rs = getgr_name(&gr,grbuf,grlen,gn)) >= 0) {
	                if (gr.gr_gid != gid) {
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
	        if (rs >= 0) {
	            rs = sncpy1(gbuf,glen,gn) ;
	        } else if (isNotOurs(rs)) {
	            rs = snsd(gbuf,glen,"G",(uint) gid) ;
	        }
	        rs1 = uc_free(grbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (getbufsize) */

	return rs ;
}
/* end subroutine (getgroupname) */


/* local subroutines */

static int isNotOurs(int rs) noex {
	return isOneOf(rsnotours,rs) ;
}
/* end subroutine (isNotOurs) */


