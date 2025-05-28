/* varithmetic MODULE (primary-module-interface) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* Vector-Arithmetic */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Family:
	varithmetic

	Description:
	These subroutines are used in performing extended precision
	integer arithmetic.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>

export module varithmetic ;

typedef ulong	digit ;


constexpr int addone(digit &r,digit v) noex {
	r = v + 1 ;
	return int((r < v) || (r == 0)) ;
}

constexpr int addc(digit &r,digit v0,digit v1,int ci) noex {
	digit		co{} ;
	digit		dci = cast_static<digit>(ci) ;
	r = mach_addc(v0,v1,dci,&co) ;
	return int(co) ;
}

constexpr int vadd(int n,digit *r,digit *v0,digit *v1,int ci) noex {
    	int		co = ci ;
	for (int i = 0 ; i < n ; i += 1) {
	    co = addc(r[i],v0[i],v1[i],co) ;
	}
	return co ;
}

constexpr int vaddone(int n,digit *r,digit *v) noex {
    	int		co = 1 ;
	for (int i = 0 ; i < n ; i += 1) {
	    co = addc(r[i],v[i],0u,co) ;
	}
	return co ;
}

constexpr void vcomp(int n,digit *r,digit *v1) noex {
	for (int i = 0 ; i < n ; i += 1) {
	    r[i] = (compl v1[i]) ;
	}
} /* end subroutine (vcomp) */

constexpr void vneg(int n,digit *r,digit *v) noex {
    	digit t[n] ;
	vcomp(n,t,v) ;
	vaddone(n,r,t) ;
} /* end subroutine (vneg) */

constexpr int vcmp(int n,digit *v1,digit *v2) noex {
	int		rc = 0 ;
	for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	    if (v1[i] != v2[i]) {
	        rc = (v1[i] > v2[i]) ? +1 : -1 ;
	        break ;
	    }
	}
	return rc ;
} /* end subroutine (vcmp) */


