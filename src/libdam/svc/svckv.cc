/* svckv SUPPORT */
/* lang=C++20 */

/* key-value type functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-13, David A­D­ Morano
	This was split out of the HOMEPAGE program (where it was
	originally local).

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We perform some light key-value type management.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<nleadstr.h>
#include	<matstr.h>
#include	<localmisc.h>

#include	"svckv.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	ourmat(cchar *,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	isexecs[] = {
	"program",
	"args",
	nullptr
} ;

enum svcopts {
	svcopt_termout,
	svcopt_overlast
} ;

constexpr cpcchar	svcopts[] = {
	"termout",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int svckv_val(cchar *(*kv)[2],int n,cchar *sp,cchar **vpp) noex {
	int		vl = 0 ;
	if (vpp) *vpp = nullptr ;
	for (int i = 0 ; i < n ; i += 1) {
	    cint	kl = lenstr(kv[i][0]) ;
	    cchar	*kp = kv[i][0] ;
	    if (ourmat(sp,kp,kl)) {
	        if (vpp) *vpp = kv[i][1] ;
	        vl = lenstr(kv[i][1]) ;
	        break ;
	    }
	} /* end for */
	return vl ;
}
/* end subroutine (svckv_val) */

int svckv_dequote(cchar *(*kv)[2],int n,cchar *sp,cchar **vpp) noex {
	int		cl = 0 ;
	cchar		*vp{} ;
	cchar		*cp = nullptr ;
	if (int vl ; (vl = svckv_val(kv,n,sp,&vp)) > 0) {
	    cl = sfdequote(vp,vl,&cp) ;
	}
	if (vpp) *vpp = cp ;
	return cl ;
}
/* end subroutine (svckv_dequote) */

int svckv_isfile(cchar *(*kv)[2],int n,cchar **vpp) noex {
	int		vl ;
	cchar		*sp = "file" ;
	vl = svckv_val(kv,n,sp,vpp) ;
	return vl ;
}
/* end subroutine (svckv_isfile) */

int svckv_ispass(cchar *(*kv)[2],int n,cchar **vpp) noex {
	int		vl ;
	cchar		*sp = "passfile" ;
	vl = svckv_val(kv,n,sp,vpp) ;
	return vl ;
}
/* end subroutine (svckv_ispass) */

#ifdef	COMMENT
int svckv_islib(cchar *(*kv)[2],int n,cchar **vpp) noex {
	int		vl ;
	cchar		*sp = "so" ;
	vl = svckv_val(kv,n,sp,vpp) ;
	return vl ;
}
/* end subroutine (svckv_islib) */
#endif /* COMMENT */

int svckv_isprog(cchar *(*kv)[2],int n,cchar **vpp) noex {
	int		vl = 0 ;
	for (int i = 0 ; isexecs[i] != nullptr ; i += 1) {
	    vl = svckv_val(kv,n,isexecs[i],vpp) ;
	    if (vl > 0) break ;
	}
	return vl ;
}
/* end subroutine (svckv_isprog) */

/* return (as the integer return value) a bit-set of options from the SVCENT */
int svckv_svcopts(cchar *(*kv)[2],int n) noex {
	int		ow = 0 ; /* return-value */
	cchar		*k = "opts" ;
	cchar		*vp ;
	if (int vl ; (vl = svckv_val(kv,n,k,&vp)) > 0) {
	    int		cl ;
	    cchar	*cp ;
	    for (cc *tp ; (tp = strnbrk(vp,vl," ,")) != nullptr ; ) {
		cint tl = intconv(tp - vp) ;
	        if ((cl = sfshrink(vp,tl,&cp)) > 0) {
	            if (int ii ; (ii = matstr(svcopts,cp,cl)) >= 0) {
	                ow |= (1 << ii) ;
	            }
	        }
	        vl -= intconv((tp + 1)-vp) ;
	        vp = (tp + 1) ;
	    } /* end for */
	    if (vl > 0) {
	        if ((cl = sfshrink(vp,vl,&cp)) > 0) {
	            if (int ii ; (ii = matstr(svcopts,cp,cl)) >= 0) {
	                ow |= (1 << ii) ;
	            }
	        }
	    }
	} /* end if (svckv) */
	return ow ;
}
/* end subroutine (svckv_svcopts) */


/* local subrouties */

static bool ourmat(cchar *sk,cchar *sp,int nl) noex {
	bool		f = false ;
	if (int m ; (m = nleadstr(sk,sp,nl)) >= 1) {
	    f = (m == nl) ;
	}
	return f ;
}
/* end subroutine (ourmat) */


