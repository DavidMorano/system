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
	coded username-prefix form.

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
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<clanguage.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<getusername.h>
#include	<mallocxx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"mkx.h"
#include	"mkpathxx.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    int mkpathuser(char *,cchar *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	mkpathsquiggle(char *,cchar *,cchar *,int) noex ;
static int	mkpathuserfs(char *,cchar *,int) noex ;
static int	mkpathusername(char *,cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr cauto		mall = uc_libmalloc ;
constexpr cauto		mfre = uc_libfree ;


/* exported variables */


/* exported subroutines */

int mkpathuser(char *rbuf,cchar *un,cchar *pp,int pl) noex {
	int		rs = SR_FAULT ;
	if (rbuf && pp) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    if (pl < 0) pl = lenstr(pp) ;
	    while ((pl > 0) && (pp[0] == '/')) {
	        pp += 1 ;
	        pl -= 1 ;
	    }
	    if (pl > 0) ylikely {
	        if (pp[0] == CH_TILDE) {
	            pp += 1 ;
	            pl -= 1 ;
	            rs = mkpathsquiggle(rbuf,un,pp,pl) ;
	        } else if (pp[0] == 'µ') {
	            rs = mkpathuserfs(rbuf,pp,pl) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkpathuser) */


/* local subroutines */

static int mkpathsquiggle(char *rbuf,cchar *un,cchar *pp,int pl) noex {
	int		rs ;
	int		ul = pl ;
	cchar		*up = pp ;
	if (pl < 0) pl = lenstr(pp) ;
	if (cchar *tp{} ; (tp = strnchr(pp,pl,'/')) != nullptr) {
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
}
/* end subroutine (mkpathsqiggle) */

static int mkpathuserfs(char *rbuf,cchar *pp,int pl) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	if ((pl >= 2) && (strncmp("µ/",pp,2) == 0)) ylikely {
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
}
/* end subroutine (mkpathuserfs) */

static int mkpathusername(char *rbuf,cchar *up,int ul,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) ylikely {
	    cint	ulen = rs ;
	    cchar	*un = up ;
	    char	ubuf[ulen+1] ;		/* <- VLA */
	    if (ul >= 0) {
	        rs = intconv(strwcpy(ubuf,up,min(ul,ulen)) - ubuf) ;
	        un = ubuf ;
	    }
	    if (rs >= 0) ylikely {
	        if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
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
	            rs1 = mfre(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (ok) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkpathusername) */


