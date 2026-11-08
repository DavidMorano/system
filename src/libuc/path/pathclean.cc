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
	rbuf		result buffer pointer
	pbuf		source path c-string pointer
	plen		source path c-string length

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

	Notes:
	1. This subroutine has been coerced to compile and link
	using only the LIBU library.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<aflag.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DPRINTF(3u)| */

#include	"pathnext.h"
#include	"pathbuf.hh"
#include	"pathclean.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */
import deb ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#define	HASSL(pp,pl)		(((pl) > 0) && (*(pp) == '/'))


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	int		maxpathlen ;
	aflag		finit ;
	operator int () noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* local structures */


/* forward references */


/* local variables */

static initer		init ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

local int pathncleanx(char *,int,ccp,int) noex ;

int pathclean(char *rbuf,cchar *spathbuf,int spathlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if (rbuf && spathbuf) ylikely {
	    rbuf[0] = eos ;
	    if (static cint rsi = init ; (rs = rsi) >= 0) {
		rs = pathncleanx(rbuf,rs,spathbuf,spathlen) ;
		len = rs ;
	    } /* end if (initer) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathclean) */

int pathnclean(char *rbuf,int rlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if (rbuf && sp) ylikely {
	    rbuf[0] = eos ;
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
		rs = pathncleanx(rbuf,rlen,sp,sl) ;
		len = rs ;
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
	DEBPRINTSTR("ent s=",sp,sl) ;
        if (int pl ; (pl = lenstr(sp,sl)) > 0) ylikely {
            ccharp pp = sp ;
            if (pathbuf rb ; (rs = rb.start(rbuf,rlen)) >= 0) ylikely {
        	bool f_prev = false ;
                if (*pp == '/') {
                    pp += 1 ;
                    pl -= 1 ;
                    f_prev = true ;
                    if (HASSL(pp,pl)) {
                        rs = rb.chr('/') ;
                    } /* end */
		    DEBPRINTF("leading slash rbuf=%s\n",rbuf) ;
                } /* end if */
                if (rs >= 0) ylikely {
                    cchar       *cp{} ;
		    DEBPRINTSTR("for-before pbuf=",pp,pl) ;
                    for (int cl ; (cl = pathnext(pp,pl,&cp)) > 0 ; ) {
		    	DEBPRINTSTR("for-top c=",cp,cl) ;
                        if (cp[0] == '.') {
		    	    DEBPRINTF("dot-yes\n") ;
                            if ((cp[1] == '.') && (cl == 2)) {
                                if (nc > 0) {
                                    rs = rb.remslash ;
                                    nc -= 1 ;
                                } else {
                                    if (f_prev) {
                                        rb.chr('/') ;
                                    }
                                    rs = rb.str(cp,2) ;
                                    f_prev = true ;
                                    nc = 0 ;
                                } /* end if */
                            } else if (cl > 1) {
                                if (f_prev) {
                                    rb.chr('/') ;
                                }
                                rs = rb.str(cp,cl) ;
                                f_prev = true ;
                                nc += 1 ;
                            } /* end if */
                        } else {
		    	    DEBPRINTF("dot-no fprev=%u\n",int(f_prev)) ;
                            if (f_prev) {
                                rb.chr('/') ;
                            }
                            rs = rb.str(cp,cl) ;
		    	    DEBPRINTF("rb.str() rs=%d\n",rs) ;
                            f_prev = true ;
                            nc += 1 ;
                        } /* end if */
                        pl -= intconv((cp + cl) - pp) ;
                        pp = (cp + cl) ;
		        DEBPRINTF("for-bot rs=%d rbuf=%s\n",rs,rbuf) ;
                        if (rs < 0) break ;
                    } /* end for */
		    DEBPRINTF("for-after rs=%d rbuf=%s\n",rs,rbuf) ;
                } /* end if (ok) */
                rs1 = rb.finish() ;
                if (rs >= 0) rs = rs1 ;
		DEBPRINTF("rb.fin() rs=%d\n",rs1) ;
                len = rs1 ;
            } /* end if (pathbuf) */
        } /* end if (positive) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
        return (rs >= 0) ? len : rs ;
} /* end subroutine (pathncleanx) */

initer::operator int () noex {
    	int		rs = maxpathlen ;
	if (! finit) {
	    if (! finit.testandset) {
		if ((rs = ulibval.maxpathlen) >= 0) {
		    maxpathlen = rs ;
		}
	    } /* end if (needed) */
	} /* end if */
	return rs ;
} /* end method (initer::operator) */


