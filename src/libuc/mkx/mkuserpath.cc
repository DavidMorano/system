/* mkuserpath SUPPORT */
/* lang=C++20 */

/* make a user-path */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	1		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkuserpath

	Description:
	This subroutine creates a resolved filename path from the
	coded username-prefix form.

	Synopsis:
	int mkuserpath(char *rbuf,cchar *un,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer (should be MAXPATHLEN+1 sized)
	un		username
	pp		source path pointer
	pl		source path length

	Returns:
	>0		expansion
	==0		no expansion
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<algorithm>
#include	<usystem.h>
#include	<clanguage.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getusername.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<localmisc.h>


/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    int mkuserpath(char *,cchar *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	mkpathsquiggle(char *,cchar *,cchar *,int) noex ;
static int	mkpathuserfs(char *,cchar *,int) noex ;
static int	mkpathusername(char *,cchar *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkuserpath(char *rbuf,cchar *un,cchar *pp,int pl) noex {
	int		rs = SR_FAULT ;
	if (rbuf && pp) {
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    if (pl < 0) pl = strlen(pp) ;
	    while ((pl > 0) && (pp[0] == '/')) {
	        pp += 1 ;
	        pl -= 1 ;
	    }
	    if (pl > 0) {
	        if (pp[0] == '~') {
	            pp += 1 ;
	            pl -= 1 ;
	            rs = mkpathsquiggle(rbuf,un,pp,pl) ;
	        } else if (pp[0] == 'u') {
	            rs = mkpathuserfs(rbuf,pp,pl) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkuserpath) */


/* local subroutines */

static int mkpathsquiggle(char *rbuf,cchar *un,cchar *pp,int pl) noex {
	int		rs ;
	int		ul = pl ;
	cchar		*up = pp ;
	if (pl < 0) pl = strlen(pp) ;
	if (cchar *tp{} ; (tp = strnchr(pp,pl,'/')) != nullptr) {
	    ul = (tp-pp) ;
	    pl -= ((tp+1)-pp) ;
	    pp = (tp+1) ;
	} else {
	    pp += pl ;
	    pl = 0 ;
	}
	if ((ul == 0) && un) {
	    up = un ;
	    ul = -1 ;
	}
	rs = mkpathusername(rbuf,up,ul,pp,pl) ;
	return rs ;
}
/* end subroutine (mkpathsqiggle) */

static int mkpathuserfs(char *rbuf,cchar *pp,int pl) noex {
	const nullptr_t	np{} ;
	int		rs = SR_OK ;
	if ((pl >= 2) && (strncmp("u/",pp,2) == 0)) {
	    pp += 2 ;
	    pl -= 2 ;
	    if (pl > 0) {
	        while (pl && (pp[0] == '/')) {
	            pp += 1 ;
	            pl -= 1 ;
	        }
	        if (pl > 0) {
	            cchar	*up = pp ;
	            int		ul = pl ;
	            if (cchar *tp{} ; (tp = strnchr(pp,pl,'/')) != np) {
	                ul = (tp - pp) ;
	                pl -= ((tp+1)-pp) ;
	                pp = (tp+1) ;
	            } else {
	                pp += pl ;
	                pl = 0 ;
	            }
	            rs = mkpathusername(rbuf,up,ul,pp,pl) ;
	        } /* end if (positive) */
	    } /* end if (positive) */
	} /* end if (user-fs called for) */
	return rs ;
}
/* end subroutine (mkpathuserfs) */

static int mkpathusername(char *rbuf,cchar *up,int ul,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    cint	ulen = rs ;
	    cchar	*un = up ;
	    char	ubuf[ulen+1] ;		/* <- VLA */
	    if (ul >= 0) {
	        rs = strwcpy(ubuf,up,min(ul,ulen)) - ubuf ;
	        un = ubuf ;
	    }
	    if (rs >= 0) {
	        if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	            PASSWD	pw ;
	            cint	pwlen = rs ;
	            char	*pwbuf{} ;
	            if ((rs = uc_libmalloc((pwlen+1),&pwbuf)) >= 0) {
	                if ((un[0] == '\0') || (un[0] == '-')) {
	                    rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	                } else {
	                    rs = GETPW_NAME(&pw,pwbuf,pwlen,un) ;
	                }
	                if (rs >= 0) {
		            cchar	*dir = pw.pw_dir ;
	                    if (sl > 0) {
	                        rs = mkpath2w(rbuf,dir,sp,sl) ;
			        rl = rs ;
	                    } else {
	                        rs = mkpath1(rbuf,dir) ;
			        rl = rs ;
	                    }
	                } /* end if (ok) */
	                rs1 = uc_libfree(pwbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (getbufsize) */
	    } /* end if (ok) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkpathusername) */


