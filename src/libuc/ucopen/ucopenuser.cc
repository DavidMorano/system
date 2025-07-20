/* ucopenuser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a user file */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucopenuser

	Description:
	This subroutine opens a user file.  This is a file that is
	relative (under) a user-home directory.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<getusername.h>
#include	<opensysfs.h>
#include	<opensysdbs.h>
#include	<libmallocxx.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<ischarx.h>
#include	<localmisc.h>

import libutil ;

/* local defines */


/* imported namespaces */

using libuc::opensysfs ;


/* local typedefs */


/* external subroutines */

extern "C" {
    int	uc_openuserbase(ucopeninfo *oip) noex ;
    int	uc_openuserpath(ucopeninfo *oip) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	int		usernamelen ;
	operator int () noex ;
    } ; /* end struct vars) */
}


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int uc_openuser(cchar *un,cchar *upath,int oflags,mode_t operms,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	if (un && upath) {
	    rs = SR_INVALID ;
	    if (un[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            if (char *pwbuf ; (rs = libmalloc_pw(&pwbuf)) >= 0) {
	                ucentpw		pw ;
	                cint		pwlen = rs ;
	                if (un[0] == '-') {
	                    rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	                } else {
	                    rs = getpwx_name(&pw,pwbuf,pwlen,un) ;
	                }
	                if (rs >= 0) {
			    if (char *fbuf ; (rs = libmalloc_mp(&fbuf)) >= 0) {
			        cchar	*udir = pw.pw_dir ;
	                        if ((rs = mkpath(fbuf,udir,upath)) >= 0) {
	                            ucopeninfo	oi{} ;
	                            oi.fname = fbuf ;
	                            oi.oflags = oflags ;
	                            oi.operms = operms ;
	                            oi.to = to ;
	                            rs = uc_openinfo(&oi) ;
			            fd = rs ;
	                        } /* end if (mkpath) */
			        rs = rslibfree(rs,fbuf) ;
			    } /* end if (m-a-f) */
	                } /* end if (ok) */
	                rs1 = uc_free(pwbuf) ;
		        if (rs >= 0) rs = rs1 ;
		        if ((rs < 0) && (fd >= 0)) {
			    u_close(fd) ;
		        } /* end if (error-handling) */
	            } /* end if (m-a-f) */
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openuser) */

int uc_openuserinfo(ucopeninfo *oip) noex {
	int		rs ;
	cchar		*fp = oip->fname ;
	while (fp[0] == '/') {
	    fp += 1 ;
	}
	if (fp[0] == '\0') {
	    rs = uc_openuserbase(oip) ;
	} else {
	    rs = uc_openuserpath(oip) ;
	}
	return rs ;
}
/* end subroutine (uc_openuserinfo) */

int uc_openuserbase(ucopeninfo *oip) noex {
	const opensysdbs	w = opensysdb_userhomes ;
	cint			of = oip->oflags ;
	cint			to = oip->to ;
	return opensysfs(w,of,to) ;
}
/* end subroutine (uc_openuserbase) */

int uc_openuserpath(ucopeninfo *oip) noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		ul = -1 ;
	int		fd = -1 ;
	cchar		*fp = oip->fname ;
	cchar		*tp ;
	cchar		*un = nullptr ;

	while (fp[0] == '/') fp += 1 ;

	un = fp ;
	if ((tp = strchr(fp,'/')) != nullptr) {
	    ul = intconv(tp - fp) ;
	    fp = (tp+1) ;
	} else {
	    fp += lenstr(fp) ;
	}

	if (un[0] != '\0') {
	    cint	sz = ((var.maxpathlen + 1) + (var.usernamelen + 1)) ;
	    if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	        cint	ulen = USERNAMELEN ;
	        char	ubuf[USERNAMELEN + 1] ;
	        if ((rs = snwcpy(ubuf,ulen,un,ul)) >= 0) {
		    if (char *pwbuf ; (rs = libmalloc_pw(&pwbuf)) >= 0) {
		        ucentpw	pw ;
		        cint	pwlen = rs ;
	                if ((rs = getpwx_name(&pw,pwbuf,pwlen,ubuf)) >= 0) {
	                    cchar	*ud = pw.pw_dir ;
	                    if (ud[0] != '\0') {
				char	tbuf[MAXPATHLEN + 1] ;
	                        while (fp[0] == '/') fp += 1 ;
	                        if (fp[0] != '\0') {
	                            if ((rs = mkpath2(tbuf,ud,fp)) >= 0) {
	                                oip->fname = tbuf ;
	                                rs = uc_openinfo(oip) ;
					fd = rs ;
	                            }
	                        } else {
	                            const int	of = oip->oflags ;
	                            rs = u_open(ud,of,0666) ;
				    fd = rs ;
	                        }
	                    } else {
	                        rs = SR_NOENT ;
			    }
	                } /* end if (getpw-name) */
		        rs1 = uc_free(pwbuf) ;
			if (rs >= 0) rs = rs1 ;
			if ((rs < 0) && (fd >= 0)) u_close(fd) ;
		    } /* end if (m-a-f) */
	        } /* end if (sncpy) */
	        rs = rslibfree(rs,a) ;
	    } /* end if (m-a-f) */
	} else {
	    rs = SR_NOENT ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openuserpath) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_un)) >= 0) {
	        usernamelen = rs ;
	    }
	}
	return rs ;
}


