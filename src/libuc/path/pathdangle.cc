/* pathdangle SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the sub-path of a dangling soft-link */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathdangle

	Description:
	This subroutine finds a sub-path within a larger path
	(consisting of a counted c-string) that is a dangling
	soft-link.

	Synopsis:
	int pathdangle(cchar *pbuf,int plen) noex

	Arguments:
	pbuf		source path to examine pointer
	plen		source path to examine length

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

	Notes:
	1. Implementation: This subroutine has been coerced to compile
	and link using only the lIBU library.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"pathnext.h"
#include	"pathadd.h"
#include	"pathdangle.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |isnotpresent(3u)| */
import deb ;				/* debugging (LIBU) */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#define	ISNOTPRES(rs)	isnotpresent(rs)


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local structures */


/* forward references */

local int pathdangler(cchar *sp,int sl) noex ;


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pathdangle(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (sl > 0) ylikely {
		rs = SR_NOMEM ;
		if (strnul ss(sp,sl) ; ss.fok) ylikely {
		    if (ustat sb ; (rs = u_stat(ss,&sb)) >= 0) {
			rs = SR_OK ;
		    } else if (ISNOTPRES(rs)) {
	                rs = pathdangler(sp,sl) ;
		        len = rs ;
		    } /* end if */
		} /* end if (strnul) */
	    } /* end if (non-empty) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathdangle) */


/* local subroutines */

local int pathdangler(cchar *sp,int sl) noex {
    	cint		plen = sl ;
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	DEBPRINTF("ent\n") ;
	DEBPRINTSTR("s=",sp,sl) ;
	if (char *pbuf ; (rs = umem.mall((plen + 1),&pbuf)) >= 0) ylikely {
	    int pl = 0 ;
	    int	cl ;
	    cchar *src = sp ;
	    cchar *cp ;
	    while ((sl > 0) && ((cl = pathnext(sp,sl,&cp)) >= 0)) {
		DEBPRINTF("pathnext() cl=%d\n",cl) ;
		DEBPRINTSTR("c=",cp,cl) ;
		if (cl > 0) {
		    DEBPRINTF("pathnaddw plen=%d pl=%d\n",plen,pl) ;
		    if ((rs = pathnaddw(pbuf,plen,pl,cp,cl)) >= 0) ylikely {
		        DEBPRINTF("pathnaddw() rs=%d\n",rs) ;
		        pl = rs ;
			DEBPRINTF("pl=%d testpath=%s\n",pl,pbuf) ;
		        if (ustat sb ; (rs = u_stat(pbuf,&sb)) >= 0) {
			    rs = SR_OK ;
		        } else if (isNotPresent(rs)) {
			    len = conv<int>((cp + cl) - src) ;
			    rs = SR_OK ;
			    DEBPRINTF("ret_on len=%d\n",len) ;
			    break ;
		        } /* end if */
		    } /* end if (pathnaddw) */
		    DEBPRINTF("pathnaddw-out rs=%d\n",rs) ;
		} /* end if (non-empty) */
		sl -= conv<int>((cp + cl) - sp) ;
		sp = (cp + cl) ;
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = umem.free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathdangler) */


