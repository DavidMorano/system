/* mkvarpath SUPPORT */
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
	mkvarpath

	Description:
	This subroutine possibly expands the given path string if
	it contains a multi-path environment variable.

	Synopsis:
	int mkvarpath(char *rbuf,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer (should be MAXPATHLEN+1 long)
	pp		source path pointer
	pl		source path length

	Returns:
	<0		error
	==0		no expansion
	>0		expansion

	Example:
	Given a CDPARH variable of:
		${HOME}:${HOME}/repos
	and given an input string of:
		/%cpath/bin/daytime
	this subroutine expands that input string to:
		${HOME}/bin/daytime:${HOME}/repos/bin/daytime

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
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


/* local defines */

#define	MKVARPATH_MP	(4*1024)

#undef	CH_EXPAND
#define	CH_EXPAND	'%'


/* local typedefs */


/* external subroutines */

extern "C" {
    extern cchar	*getenver(cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	mkvarpath_list(char *,cchar *,cchar *) noex ;
static int	mkvarpath_one(char *,vecstr *,cchar *,int,cchar *) noex ;
static int	mkvarpath_join(char *,cchar *,int,cchar *) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKVARPATH_MP) ;


/* exported subroutines */

int mkvarpath(char *rbuf,cchar *fp,int fl) noex {
	cint		ec = CH_EXPAND ;
	int		rs = SR_OK ;
	int		pl = 0 ;
	if (rbuf && fp) {
            rbuf[0] = '\0' ;
            if (fl < 0) fl = strlen(fp) ;
            if ((fp[0] == ec) || ((fp[0] == '/') && fl && (fp[1] == ec))) {
                int         vl = (fl - 1) ;
                cchar       *vp = (fp + 1) ;
                cchar       *rp = nullptr ;
                cchar       *tp ;
                cchar       *cp ;
                if (vl && (vp[0] == ec)) { /* check for prefix character */
                    vp += 1 ;
                    vl -= 1 ;
                }
                if ((tp = strnchr(vp,vl,'/')) != nullptr) {
                    vl = (tp-vp) ;
                    rp = tp ;
                }
                if (vl > 0) {
                    char    *vbuf = nullptr ; /* writable (will be) */
                    if ((cp = getenver(vp,vl)) == nullptr) {
                        if (haslc(vp,vl)) {
                            char    *p ;
                            if ((rs = uc_malloc((vl+1),&p)) >= 0) {
                                vbuf = p ;
                                strwcpyuc(vbuf,vp,vl) ;
                                cp = getenver(vbuf,vl) ;
                            }
                        }
                    }
                    if (rs >= 0) {
                        if (cp != nullptr) {
                            if (strchr(cp,':') != nullptr) {
                                rs = mkvarpath_list(rbuf,cp,rp) ;
                                pl = rs ;
                            } else {
                                rs = mkvarpath_join(rbuf,cp,-1,rp) ;
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
/* end subroutine (mkvarpath) */


/* local subroutines */

static int mkvarpath_list(char *rbuf,cchar *pathlist,cchar *rp) noex {
	vecstr		paths ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if ((rs = vecstr_start(&paths,2,0)) >= 0) {
	    int		sl ;
	    int		f_zero = false ;
	    cchar	*sp = pathlist ;
	    cchar	*tp ;
	    while ((tp = strchr(sp,':')) != nullptr) {
	        sl = (tp-sp) ;
	        if (sl || (! f_zero)) {
	            if ((! f_zero) && (sl == 0)) f_zero = true ;
	            rs = mkvarpath_one(rbuf,&paths,sp,sl,rp) ;
	            pl = rs ;
	        }
	        sp = (tp+1) ;
	        if (((rs >= 0) && (pl > 0)) || (! isNotPresent(rs))) break ;
	    } /* end while */
	    if ((rs >= 0) && (pl == 0) && ((sp[0] != '\0') || (! f_zero))) {
	        rs = mkvarpath_one(rbuf,&paths,sp,-1,rp) ;
	        pl = rs ;
	    }
	    rs1 = vecstr_finish(&paths) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paths) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkvarpath_list) */

static int mkvarpath_one(char *rbuf,vecstr *plp,cc *sp,int sl,cc *rp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	if ((rs = vecstr_findn(plp,sp,sl)) == rsn) {
	    if ((rs = vecstr_add(plp,sp,sl)) >= 0) {
	        USTAT	sb ;
	        rs1 = mkvarpath_join(rbuf,sp,sl,rp) ;
	        pl = rs1 ;
	        if ((rs1 == SR_OVERFLOW) || (rs1 == SR_NAMETOOLONG)) {
	            rs1 = SR_OK ;
	            pl = 0 ;
	        }
	        if ((rs1 >= 0) && (pl > 0) && (rbuf[0] != '\0')) {
	            rs = u_lstat(rbuf,&sb) ;
	            if (rs < 0) pl = 0 ;
	        }
	    } /* end if (vecstr_add) */
	} /* end if (not-found) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkvarpath_one) */

static int mkvarpath_join(char *rbuf,cchar *sp,int sl,cchar *rp) noex {
	int		rs ;
	int		pl = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	maxpl = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,maxpl,pl,sp,sl) ;
	        pl += rs ;
	    }
	    if ((rs >= 0) && rp) {
	        rs = storebuf_strw(rbuf,maxpl,pl,rp,-1) ;
	        pl += rs ;
	    }
	    if (rs == SR_OVERFLOW) rs = SR_NAMETOOLONG ;
	} /* end if (maxpathlen) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkvarpath_join) */


