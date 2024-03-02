/* getxid SUPPORT */
/* lang=C++20 */

/* get various IDs by various means */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

	= 2017-11-22, David A­D­ Morano
	I finally (finally) removed (compiled out) the global
	function symbol |getuser_uid()|.  Good riddance.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuid_name

	Description:
	This subroutine will retrieve a UID by looking up a given
	name, assumed to be a user-name.

	Synopsis:
	int getuid_name(cchar *np,int nl) noex

	Arguments:
	np		user-name to lookup
	nl		length of name string

	Returns:
	>=0		retrieved UID
	<0		error (system-return)


	Name:
	getgid_name

	Description:
	This subroutine returns a GID for a specified group-name. 

	Synopsis:
	int getgid_name(cchar *sp,int sl) noex

	Arguments:
	sp		name of group to look up 
	sl		length of given string

	Returns:
	>=0		GID of given group name
	<0		error (system-return)


	Name:
	getgid_def

	Description:
	This subroutine returns a GID for a specified groupname.
	A default GID is returned if the groupname does not exist.

	Synopsis:
	int getgid_def(cchar *gname,gid_t gid) noex

	Arguments:
	gname		groupname to lookup
	gid		default GID if lookup fails

	Returns:
	>=0		GID found or the default GID
	<0		error (system-return)


	Name:
	getpjid_name

	Description:
	This subroutine returns a project-ID for a specified
	program-name.

	Synopsis:
	int getpjid_name(cchar *np,int nl) noex

	Arguments:
	np		name of project to look up 
	nl		length of name (in bytes)

	Returns:
	>=0		PJID of given project name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<nulstr.h>
#include	<getax.h>
#include	<ucpwcache.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"getxid.h"


/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int getuid_name(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		uid = 0 ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        nulstr	n ;
	        cchar	*name{} ;
	        if ((rs = nulstr_start(&n,sp,sl,&name)) >= 0) {
	            char	*pwbuf{} ;
	            if ((rs = malloc_pw(&pwbuf)) >= 0) {
	                cint	pwlen = rs ;
		        {
	                    PASSWD	pw ;
		            rs = GETPW_NAME(&pw,pwbuf,pwlen,name) ;
	                    uid = pw.pw_uid ;
		        }
	                rs1 = uc_free(pwbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = nulstr_finish(&n) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? uid : rs ;
}
/* end subroutine (getuid_name) */

int getuid_user(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        if (hasalldig(sp,sl)) {
	            if (int v{} ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
	            rs = getuid_name(sp,sl) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getuid_user) */

int getgid_name(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		gid = 0 ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0] && (sl > 0)) {
	        nulstr	ns ;
	        cchar	*name{} ;
	        if ((rs = ns.start(sp,sl,&name)) >= 0) {
	            char	*grbuf{} ;
		    if ((rs = malloc_gr(&grbuf)) >= 0) {
	                cint	grlen = rs ;
		        {
	                    GROUP	gr{} ;
	                    rs = getgr_name(&gr,grbuf,grlen,name) ;
	                    gid = gr.gr_gid ;
		        }
	                rs1 = uc_free(grbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = ns.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? gid : rs ;
}
/* end subroutine (getgid_name) */

int getgid_group(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0] && (sl > 0)) {
	        if (hasalldig(sp,sl)) {
	            if (int v{} ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
	            rs = getgid_name(sp,sl) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getgid_group) */

int getgid_def(cchar *gname,gid_t gid) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	if (gname) {
	    if ((rs = getgid_group(gname,-1)) == rsn) {
	        if (gid == gidend) {
	            rs = getgid() ;
	        } else {
	            rs = gid ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getgid_def) */

int getpjid_name(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		pjid = 0 ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        nulstr	n ;
	        cchar	*name{} ;
	        if ((rs = nulstr_start(&n,sp,sl,&name)) >= 0) {
	            if ((rs = getbufsize(getbufsize_pj)) >= 0) {
	                PROJECT	pj ;
	                cint	pjlen = rs ;
	                char	*pjbuf{} ;
	                if ((rs = uc_malloc((pjlen+1),&pjbuf)) >= 0) {
		            {
	                        rs = getpj_name(&pj,pjbuf,pjlen,name) ;
	                        pjid = pj.pj_projid ;
		            }
	                    rs1 = uc_free(pjbuf) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
	            } /* end if (getbufsize) */
	            rs1 = nulstr_finish(&n) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pjid : rs ;
}
/* end subroutine (getpjid_name) */

int getpjid_proj(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        if (hasalldig(sp,sl)) {
	            if (int v{} ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
	            rs = getpjid_name(sp,sl) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpjid_proj) */


