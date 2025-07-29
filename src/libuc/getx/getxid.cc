/* getxid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get various IDs by various means */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

	= 2017-11-22, David A­D­ Morano
	I finally (finally) removed (compiled out) the global
	function symbol |getuser_uid(3uc)|.  Good riddance.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuid_name

	Description:
	This subroutine will retrieve a UID by looking up a given
	name, assumed to be a user-name.

	Synopsis:
	int getuid_name(cchar *sp,int sl) noex

	Arguments:
	sp		user-name to lookup
	sl		length of name string

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
	int getpjid_name(cchar *sp,int sl) noex

	Arguments:
	sp		name of project to look up 
	sl		length of name (in bytes)

	Returns:
	>=0		PJID of given project name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<mallocxx.h>
#include	<nulstr.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"getxid.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern projid_t	getprojid() noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr gid_t		gidend = -1 ;
constexpr projid_t	pjidend = -1 ;


/* exported variables */


/* exported subroutines */

int getuid_name(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		uid = 0 ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
	        cchar	*nn{} ;
	        if (nulstr ns ; (rs = ns.start(sp,sl,&nn)) >= 0) ylikely {
	            if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) ylikely {
	                ucentpw	pw{} ; 
	                cint	pwlen = rs ;
			cauto	getpw = getpwx_name ;
			if ((rs = getpw(&pw,pwbuf,pwlen,nn)) >= 0) {
	                    uid = pw.pw_uid ;
		        }
	                rs1 = uc_free(pwbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = ns.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? uid : rs ;
}
/* end subroutine (getuid_name) */

int getuid_user(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
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
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
	        cchar	*name{} ;
	        if (nulstr ns ; (rs = ns.start(sp,sl,&name)) >= 0) ylikely {
	            if (char *grbuf ; (rs = malloc_gr(&grbuf)) >= 0) ylikely {
	                cint	grlen = rs ;
	                ucentgr	gr{} ;
			if ((rs = getgr_name(&gr,grbuf,grlen,name)) >= 0) {
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
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
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
	if (gname) ylikely {
	    if ((rs = getgid_group(gname,-1)) == rsn) ylikely {
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
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
	        cchar	*name{} ;
	        if (nulstr ns ; (rs = ns.start(sp,sl,&name)) >= 0) ylikely {
	            if (char *pjbuf ; (rs = malloc_pj(&pjbuf)) >= 0) ylikely {
	                cint	pjlen = rs ;
		        ucentpj pj ;
		        if ((rs = getpj_name(&pj,pjbuf,pjlen,name)) >= 0) {
	                    pjid = pj.pj_projid ;
		        }
	                rs1 = uc_free(pjbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = ns.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pjid : rs ;
}
/* end subroutine (getpjid_name) */

int getpjid_proj(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
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

int getpjid_def(cchar *name,projid_t pjid) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	if (name) ylikely {
	    if ((rs = getpjid_proj(name,-1)) == rsn) {
	        if (pjid == pjidend) {
	            rs = getprojid() ;
	        } else {
	            rs = pjid ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpjid_def) */


