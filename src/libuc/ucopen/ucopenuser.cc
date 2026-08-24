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
#include	<sys/types.h>		/* POSIX® system types */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC |ucpwcache_name(3uc)| */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<opensysfs.hh>		/* LIBUC */
#include	<opensysdbs.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucopeninfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::opensysfs ;		/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    int	uc_openuserbase(ucopeninfo *oip) noex ;
    int	uc_openuserpath(ucopeninfo *oip) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	int		usernamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int uc_openuser(cchar *un,cchar *upath,int oflags,mode_t operms,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	if (un && upath) ylikely {
	    rs = SR_INVALID ;
	    if (un[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	                ucentpw		pw ;
	                cint		pwlen = rs ;
	                if (un[0] == '-') {
	                    rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	                } else {
	                    rs = getpwx_name(&pw,pwbuf,pwlen,un) ;
	                }
	                if (rs >= 0) ylikely {
			    if (char *fbuf ; (rs = lm_mp(&fbuf)) >= 0) {
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
			        rs1 = lm_free(fbuf) ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (m-a-f) */
	                } /* end if (ok) */
	                rs1 = lm_free(pwbuf) ;
		        if (rs >= 0) rs = rs1 ;
		        if ((rs < 0) && (fd >= 0)) {
			    u_close(fd) ;
		        } /* end if (error-handling) */
	            } /* end if (m-a-f) */
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (uc_openuser) */

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
} /* end subroutine (uc_openuserinfo) */

int uc_openuserbase(ucopeninfo *oip) noex {
	const opensysdbs	w = opensysdb_userhomes ;
	cint			of = oip->oflags ;
	cint			to = oip->to ;
	return opensysfs(w,of,to) ;
} /* end subroutine (uc_openuserbase) */

int uc_openuserpath(ucopeninfo *oip) noex {
	cint		maxuser = var.usernamelen ;
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
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	        cint	ulen = maxuser ;
	        charp	ubuf = (a + (0 * (maxuser + 1))) ;
	        if ((rs = snwcpy(ubuf,ulen,un,ul)) >= 0) {
		    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) {
		        ucentpw	pw ;
		        cint	pwlen = rs ;
	                if ((rs = getpwx_name(&pw,pwbuf,pwlen,ubuf)) >= 0) {
	                    cchar	*ud = pw.pw_dir ;
	                    if (ud[0] != '\0') {
				charp	tbuf = (a + (maxuser + 1)) ;
	                        while (fp[0] == '/') fp += 1 ;
	                        if (fp[0] != '\0') {
	                            if ((rs = mkpath2(tbuf,ud,fp)) >= 0) {
	                                oip->fname = tbuf ;
	                                rs = uc_openinfo(oip) ;
					fd = rs ;
	                            }
	                        } else {
	                            cint	of = oip->oflags ;
	                            rs = u_open(ud,of,0666) ;
				    fd = rs ;
	                        }
	                    } else {
	                        rs = SR_NOENT ;
			    }
	                } /* end if (getpw-name) */
		        rs1 = lm_free(pwbuf) ;
			if (rs >= 0) rs = rs1 ;
			if ((rs < 0) && (fd >= 0)) u_close(fd) ;
		    } /* end if (m-a-f) */
	        } /* end if (sncpy) */
	        rs1 = lm_free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = SR_NOENT ;
	}
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (uc_openuserpath) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	    if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
	        usernamelen = rs ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */


