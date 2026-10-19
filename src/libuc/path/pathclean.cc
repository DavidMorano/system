/* pathclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* cleanup a path */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathclean

	Description:
	This subroutine takes a path and "cleans" it up by removing
	certain redundant or otherwise unnecessary elements.

	Synopsis:
	int pathclean(char *rbuf,cchar *pbuf,int plen) noex

	Arguments:
	rbuf		buffer to receive cleaned up path string
	pbuf		supplied path to clean up
	plen		length of supplied path to clean up

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<aflag.hh>		/* LIBU */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DPRINTF(3u)| */

#include	"pathbuf.hh"
#include	"pathclean.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import deb ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#define	VARPATHCLEAN	"PATHCLEAN_DEBFN"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	aflag		finit{} ;
	operator int () noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* local structures */


/* forward references */

local int	nextname(cchar *,int,cchar **) noex ;


/* local variables */

static initer		init ;
static bufsizevar	maxpathlen	(bufsize_mp) ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

local int pathncleanx(char *,int,ccp,int) noex ;

int pathclean(char *rbuf,cchar *spathbuf,int spathlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && spathbuf) ylikely {
	    rbuf[0] = eos ;
	    if (static cint rsi = init ; (rs = rsi) >= 0) {
	        if ((rs = maxpathlen) >= 0) ylikely {
	            rs = pathncleanx(rbuf,rs,spathbuf,spathlen) ;
		    len = rs ;
	        } /* end if (maxpathlen) */
	    } /* end if (initer) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathclean) */

int pathnclean(char *rbuf,int rlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && sp) ylikely {
	    rbuf[0] = eos ;
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
	       if (static cint rsi = init ; (rs = rsi) >= 0) {
	           rs = pathnclean(rbuf,rlen,sp,sl) ;
		   len = rs ;
	       } /* end if (initer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathnclean) */


/* local subroutines */

local int pathncleanx(char *rbuf,int rlen,cc *sp,int sl) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
        int     	nc = 0 ;
        bool    	f_prev = false ;
        cchar   	*pp = sp ;
	DEBPRINTF("ent sp=%s\n",sp) ;
        if (int pl ; (pl = lenstr(sp,sl)) > 0) ylikely {
            if (pathbuf pb ; (rs = pb.start(rbuf,rlen)) >= 0) ylikely {
                if (*pp == '/') {
                    pp += 1 ;
                    pl -= 1 ;
                    f_prev = true ;
                    if (*pp == '/') {
                        rs = pb.chr('/') ;
                    } /* end */
                } /* end if */
                if (rs >= 0) ylikely {
                    cchar       *cp{} ;
                    for (int cl ; (cl = nextname(pp,pl,&cp)) > 0 ; ) {
                        if (cp[0] == '.') {
                            if ((cp[1] == '.') && (cl == 2)) {
                                if (nc > 0) {
                                    rs = pb.remslash ;
                                    nc -= 1 ;
                                } else {
                                    if (f_prev) {
                                        pb.chr('/') ;
                                    }
                                    rs = pb.str(cp,2) ;
                                    f_prev = true ;
                                    nc = 0 ;
                                } /* end if */
                            } else if (cl > 1) {
                                if (f_prev) {
                                    pb.chr('/') ;
                                }
                                rs = pb.str(cp,cl) ;
                                f_prev = true ;
                                nc += 1 ;
                            } /* end if */
                        } else {
                            if (f_prev) {
                                pb.chr('/') ;
                            }
                            rs = pb.str(cp,cl) ;
                            f_prev = true ;
                            nc += 1 ;
                        } /* end if */
                        pl -= intconv((cp + cl) - pp) ;
                        pp = (cp + cl) ;
                        if (rs < 0) break ;
                    } /* end for */
                } /* end if (ok) */
                rs1 = pb.finish() ;
                if (rs >= 0) rs = rs1 ;
                len = rs1 ;
            } /* end if (pathbuf) */
        } /* end if (positive) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
        return (rs >= 0) ? len : rs ;
} /* end subroutine (pathncleanx) */

local int nextname(cchar *sp,int sl,cchar **rpp) noex {
    	int		rl = 0 ;
	cchar		*rp = nullptr ;
	bool		f_len = (sl >= 0) ;
	while (((! f_len) || (sl > 0)) && (*sp == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rp = sp ;
	while ((((! f_len) && *sp) || (sl > 0)) && (*sp != '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rl = intconv(sp - rp) ;
	if (rpp) {
	    *rpp = rp ;
	} /* end */
	return rl ;
} /* end subroutine (nextname) */

initer::operator int () noex {
    	int		rs = SR_OK ;
	if_constexpr (f_debug) {
	    if (! finit.testandset) {
	        if (ccharp dfn = getenv(VARPATHCLEAN)) {
		    if (cint rs1 = DEBOPEN(dfn) ; rs1 >= 0) {
		        DEBPRINTF("debinit fd=%d\n",rs1) ;
		    } /* end if (DEBOPEN) */
	        } /* end if (getenv) */
	    } /* end if (needed) */
	} /* end if_constexpr (f_debug) */
	return rs ;
} /* end method (initer::operator) */


