/* mkpathuser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a user-path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkpathuser

	Description:
	This subroutine creates a resolved filename path from the
	coded username-prefix form.  The following forms are
	recognized:
		~<user>/<remainder>
		µ/<user>/<remainder>

	Synopsis:
	int mkpathuser(char *rbuf,cchar *un,cchar *pp,int pl) noex

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkx.h"
#include	"mkpathxx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CHX_MICRO
#define	CHX_MICRO	'µ'
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	mkpathsquiggle	(char *,cchar *,cchar *,int)		noex ;
local int	mkpathuserfs	(char *,cchar *,int)			noex ;
local int	mkpathusername	(char *,cchar *,int,cchar *,int)	noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkpathuser(char *rbuf,cchar *un,cchar *pp,int µpl) noex {
	int		rs = SR_FAULT ;
	if (int pl ; rbuf && ((pl = getlenstr(pp,µpl)) >= 0)) {
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    while ((pl > 0) && (pp[0] == '/')) {
	        pp += 1 ;
	        pl -= 1 ;
	    } /* end while */
	    if (pl > 0) ylikely {
	        if (pp[0] == CH_TILDE) {
	            pp += 1 ;
	            pl -= 1 ;
	            rs = mkpathsquiggle(rbuf,un,pp,pl) ;
	        } else if (pp[0] == CHX_MICRO) {
	            rs = mkpathuserfs(rbuf,pp,pl) ;
	        }
	    } /* end if */
	} /* end if (getlenstr) */
	return rs ;
} /* end subroutine (mkpathuser) */


/* local subroutines */

local int mkpathsquiggle(char *rbuf,cchar *un,cchar *pp,int pl) noex {
	int		rs = SR_FAULT ;
	int		ul = pl ;
	cchar		*up = pp ;
	if (pl < 0) pl = lenstr(pp) ;
	if (cchar *tp ; (tp = strnchr(pp,pl,'/')) != nullptr) {
	    ul = intconv(tp - pp) ;
	    pl -= intconv((tp + 1) - pp) ;
	    pp = (tp + 1) ;
	} else {
	    pp += pl ;
	    pl = 0 ;
	}
	if ((ul == 0) && un) ylikely {
	    up = un ;
	    ul = -1 ;
	}
	rs = mkpathusername(rbuf,up,ul,pp,pl) ;
	return rs ;
} /* end subroutine (mkpathsqiggle) */

local int mkpathuserfs(char *rbuf,cchar *pp,int pl) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	if ((pl >= 2) && (strncmp("µ/",pp,2) == 0)) {
	    pp += 2 ;
	    pl -= 2 ;
	    if (pl > 0) ylikely {
	        while (pl && (pp[0] == '/')) {
	            pp += 1 ;
	            pl -= 1 ;
	        }
	        if (pl > 0) ylikely {
	            cchar	*up = pp ;
	            int		ul = pl ;
	            if (cchar *tp ; (tp = strnchr(pp,pl,'/')) != np) {
	                ul = intconv(tp - pp) ;
	                pl -= intconv((tp + 1) - pp) ;
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
} /* end subroutine (mkpathuserfs) */

local int mkpathusername(char *rbuf,cchar *up,int ul,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
	    cint	ulen = rs ;
	    cchar	*un = up ;
	    char	ubuf[ulen+1] ;		/* <- VLA */
	    if (ul >= 0) {
		cint mul = min(ul,ulen) ;
	        rs = intconv(strwcpy(ubuf,up,mul) - ubuf) ;
	        un = ubuf ;
	    }
	    if (rs >= 0) ylikely {
	        if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) {
	            ucentpwx	pw ;
	            cint	pwlen = rs ;
	            if ((un[0] == '\0') || (un[0] == '-')) {
	                rs = getpwusername(&pw,pwbuf,pwlen,-1) ;
	            } else {
	                rs = pw.nam(pwbuf,pwlen,un) ;
	            }
	            if (rs >= 0) ylikely {
		        cchar	*dir = pw.pw_dir ;
	                if (sl > 0) {
	                    rs = mkpath2w(rbuf,dir,sp,sl) ;
			    rl = rs ;
	                } else {
	                    rs = mkpath(rbuf,dir) ;
			    rl = rs ;
	                }
	            } /* end if (ok) */
	            rs1 = lm_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (ok) */
	} /* end if (bufsizeget) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mkpathusername) */


