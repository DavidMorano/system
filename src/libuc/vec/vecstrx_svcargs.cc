/* vecstrx_svcargs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load service arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecpstr_svcargs

	Description:
	This subroutine loads service arguments (arguments which
	accompany a service string) into a VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<toxc.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"vecstrx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* forward references */

local int	vecstrx_arger(vecstrx *,int *,cchar *) noex ;
local int	mkterms() noex ;
local int	hasLong(cchar *,int) noex ;


/* local structures */


/* local variables */

static char		terms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int vecstrx::svcargs(int *fp,cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fp && abuf) ylikely {
            rs = SR_INVALID ;
	    if (abuf[0]) ylikely {
	        if (static cint	rst = mkterms() ; (rs = rst) >= 0) ylikely {
		    rs = vecstrx_arger(this,fp,abuf) ;
		    c = rs ;
		} /* end if (terms) */
	    } /* end if (valid) */
	} /* end if (non-null non-zero) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx_svcargs) */


/* private subroutines */

local int vecstrx_arger(vecstrx *vsp,int *fp,cchar *abuf) noex {
    	cnullptr	np{} ;
	cint		alen = lenstr(abuf) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *fbuf ; (rs = libmem.mall((alen+1),&fbuf)) >= 0) ylikely {
	    cint	flen = alen ;
	    if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) ylikely {
	        int	fl ;
		int	f = false ;
	        while ((fl = fsb.sharg(terms,fbuf,flen)) >= 0) {
		    if (c == 0) {
			if (cc *tp ; (tp = strnchr(fbuf,fl,'/')) != np) {
			     fl = intconv(tp - fbuf) ;
			     if (((fbuf + fl) - tp) >= 2) {
				cint	sch = mkchar(tp[1]) ;
				f = (tolc(sch) == 'w') ;
			    }
			}
	                rs = vsp->add(fbuf,fl) ;
			c += 1 ;
		    } else {
			if ((fbuf[0] == '/') && hasLong(fbuf,fl)) {
			    f = true ;
			} else {
	                    rs = vsp->add(fbuf,fl) ;
			    c += 1 ;
			}
		    }
	            if (rs < 0) break ;
	        } /* end while */
		if (fp) *fp = f ;
	        rs1 = fsb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = libmem.free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx_arger) */

local int mkterms() noex {
	return fieldterms(terms,false,'\t',' ') ;
} /* end subroutine (mkterms) */

local int hasLong(cchar *sp,int µsl) noex {
	int		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    if (sp[0] == '/') {
	        if (sl >= 2) {
		    cint	sch = mkchar(sp[1]) ;
		    f = (tolc(sch) == 'w') ;
	        }
	    }
	} /* end if (getlenstr) */
	return f ;
} /* end subroutine (hasLong) */


