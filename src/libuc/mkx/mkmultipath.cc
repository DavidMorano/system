/* mkmultipath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to make a prefix-variable path */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkmultipath

	Description:
	This subroutine possibly expands the given path string if
	it contains a multi-path environment variable.

	Synopsis:
	int mkmultipath(char *rbuf,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer (should be MAXPATHLEN+1 long)
	pp		source path pointer
	pl		source path length

	Returns:
	>0		expansion
	==0		no expansion
	<0		error (system-return)

	Example:
	Given a variable w/ name 'CDPATH' containing:
		${HOME}:${HOME}/repos
	and given an input string of:
		/%cpath/bin/daytime
	this subroutine expands that input string to:
		${HOME}/bin/daytime:${HOME}/repos/bin/daytime

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecstr.h>
#include	<storebuf.h>
#include	<bufsizevar.hh>
#include	<getbufsize.h>
#include	<hasx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mkx.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MKVARPATH_MP	(4*1024)

#undef	CHX_EXPAND
#define	CHX_EXPAND	'%'


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern cchar	*getenver(cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	mkmultipath_list(char *,cchar *,cchar *) noex ;
static int	mkmultipath_one(char *,vecstr *,cchar *,int,cchar *) noex ;
static int	mkmultipath_join(char *,cchar *,int,cchar *) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKVARPATH_MP) ;


/* exported variables */


/* exported subroutines */

int mkmultipath(char *rbuf,cchar *fp,int fl) noex {
	cint		chx_ec = CHX_EXPAND ;
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (rbuf && fp) ylikely {
	    bool f = false ;
            rbuf[0] = '\0' ;
            if (fl < 0) fl = lenstr(fp) ;
            f = f || (fp[0] == chx_ec) ;
	    f = f || ((fp[0] == '/') && fl && (fp[1] == chx_ec)) ;
	    if (f) {
                int         vl = (fl - 1) ;
                cchar       *vp = (fp + 1) ;
                cchar       *rp = nullptr ;
                cchar       *cp ;
                if (vl && (vp[0] == chx_ec)) { /* check for prefix character */
                    vp += 1 ;
                    vl -= 1 ;
                }
                if (cchar *tp ; (tp = strnchr(vp,vl,'/')) != nullptr) {
                    vl = intconv(tp - vp) ;
                    rp = tp ;
                }
                if (vl > 0) ylikely {
                    char    *vbuf = nullptr ; /* writable (will be) */
                    if ((cp = getenver(vp,vl)) == nullptr) {
                        if (haslc(vp,vl)) {
                            if (char *p ; (rs = uc_malloc((vl+1),&p)) >= 0) {
                                vbuf = p ;
                                strwcpyuc(vbuf,vp,vl) ;
                                cp = getenver(vbuf,vl) ;
                            }
                        }
                    } /* end if (getenver) */
                    if (rs >= 0) ylikely {
                        if (cp != nullptr) {
                            if (strchr(cp,':') != nullptr) {
                                rs = mkmultipath_list(rbuf,cp,rp) ;
                                pl = rs ;
                            } else {
                                rs = mkmultipath_join(rbuf,cp,-1,rp) ;
                                pl = rs ;
                            }
                            if (rs == SR_OVERFLOW) rs = SR_NAMETOOLONG ;
                        } else {
                            rs = (rp != nullptr) ? SR_NOTDIR : SR_NOENT ;
                        }
                    } /* end if */
                    if (vbuf) {
                        uc_free(vbuf) ;
                    }
                } else {
                    rs = SR_NOTDIR ;
                }
            } /* end if (go) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkmultipath) */


/* local subroutines */

static int mkmultipath_list(char *rbuf,cchar *pathlist,cchar *rp) noex {
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if (vecstr paths ; (rs = paths.start(2,0)) >= 0) ylikely {
	    int		sl ;
	    int		f_zero = false ;
	    cchar	*sp = pathlist ;
	    for (cchar *tp ; (tp = strchr(sp,':')) != nullptr ; ) {
	        sl = intconv(tp - sp) ;
	        if (sl || (! f_zero)) {
	            if ((! f_zero) && (sl == 0)) f_zero = true ;
	            rs = mkmultipath_one(rbuf,&paths,sp,sl,rp) ;
	            pl = rs ;
	        }
	        sp = (tp + 1) ;
	        if (((rs >= 0) && (pl > 0)) || (! isNotPresent(rs))) break ;
	    } /* end for */
	    if ((rs >= 0) && (pl == 0) && ((sp[0] != '\0') || (! f_zero))) {
	        rs = mkmultipath_one(rbuf,&paths,sp,-1,rp) ;
	        pl = rs ;
	    }
	    rs1 = paths.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paths) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkmultipath_list) */

static int mkmultipath_one(char *rbuf,vecstr *plp,cc *sp,int sl,cc *rp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	if ((rs = plp->findn(sp,sl)) == rsn) ylikely {
	    if ((rs = plp->add(sp,sl)) >= 0) ylikely {
	        rs1 = mkmultipath_join(rbuf,sp,sl,rp) ;
	        pl = rs1 ;
	        if ((rs1 == SR_OVERFLOW) || (rs1 == SR_NAMETOOLONG)) {
	            rs1 = SR_OK ;
	            pl = 0 ;
	        }
	        if ((rs1 >= 0) && (pl > 0) && (rbuf[0] != '\0')) ylikely {
	            if (ustat sb ; (rs = u_lstat(rbuf,&sb)) >= 0) {
			rs = 0 ; /* <- dummy action */
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
	                pl = 0 ;
		    }
	        }
	    } /* end if (vecstr_add) */
	} /* end if (not-found) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkmultipath_one) */

static int mkmultipath_join(char *rbuf,cchar *sp,int sl,cchar *rp) noex {
	int		rs ;
	int		pl = 0 ;
	if ((rs = maxpathlen) >= 0) ylikely {
	    if (storebuf sb(rbuf,rs) ; (rs = sb.strw(sp,sl)) >= 0) ylikely {
	        if (rp) {
	            rs = sb.strw(rp) ;
		}
		pl = sb.idx ;
	    } /* end if (storebuf) */
	    if (rs == SR_OVERFLOW) rs = SR_NAMETOOLONG ;
	} /* end if (maxpathlen) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkmultipath_join) */


