/* librandom SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* librandom object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A­D­ Morano
	I made this object to mimic the behavior of the UNIX® System
	RNG |random()| but to make it thread safe (and object
	oriented)!  So many stupid subroutines in UNIX® are not
	thread safe and there is no need for it all!  But those
	Computer Science types who were so instrumental in developing
	UNIX® and its libraries in the first place were not the
	smartest when it came to production-grade computer software!
	We now have to live with their mistakes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a knock-off of the UNIX® System |librandom(3)|
	library RNG.  I support the same "types" as it did ; namely
	five in all with type zero being the old Linear-Congruent
	(LC) RNG.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<randlc.h>
#include	<localmisc.h>

#include	"librandom.h"


/* local defines */

#define	TYPE_0		0		/* linear congruential */
#define	DEG_0		0
#define	SEP_0		0

#define	TYPE_1		1		/* x**7 + x**3 + 1 */
#define	DEG_1		7
#define	SEP_1		3

#define	TYPE_2		2		/* x**15 + x**1 + 1 */
#define	DEG_2		15
#define	SEP_2		1

#define	TYPE_3		3		/* x**31 + x**3 + 1 */
#define	DEG_3		31
#define	SEP_3		3

#define	TYPE_4		4		/* x**63 + x**1 + 1 */
#define	DEG_4		63
#define	SEP_4		1


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr uint randtbl[] = {
	0x991539b1, 0x16a5bce3, 0x6774a4cd, 0x3e01511e, 
	0x4e508aaa, 0x61048c05, 0xf5500617, 0x846b7115, 
	0x6a19892c, 0x896a97af, 0xdb48f936, 0x14898454,
	0x37ffd106, 0xb58bff9c, 0x59e17104, 0xcf918a49, 
	0x09378c83, 0x52c7a471, 0x8d293ea9, 0x1f4fc301, 
	0xc3db71be, 0x39b44e1c, 0xf8a44ef9, 0x4c8b80b1,
	0x19edc328, 0x87bf4bdd, 0xc9b240e5, 0xe9ee4b1b, 
	0x4382aee7, 0x535b6b41, 0xf3bec5da, 0
} ;

constexpr int degrees[LIBRANDOM_NTYPES] = {
	DEG_0, DEG_1, DEG_2, DEG_3, DEG_4 
} ;

constexpr int seps[LIBRANDOM_NTYPES] = {
	SEP_0, SEP_1, SEP_2, SEP_3, SEP_4 
} ;


/* exported variables */


/* exported subroutines */

int librandom_start(librandom *rp,int type,uint seed) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    rs = SR_NOTSUP ;
	    if (type < LIBRANDOM_NTYPES) {
	        ulong	hi, lo ;
	        uint	ihi, ilo ;
	        rs = memclear(rp) ;
	        if (type < 0) {
	            type = TYPE_4 ;
	        }
	        rp->rand_type = type ;
	        rp->rand_deg = degrees[type] ;
	        rp->rand_sep = seps[type] ;
	        rp->fptr = rp->state + rp->rand_sep ;
	        rp->rptr = rp->state ;
	        rp->end_ptr = rp->state + rp->rand_deg ;
		{
	            for (int i = 0 ; randtbl[i] != 0 ; i += 1) {
	                hi = uint(randlc(randtbl[i])) ;
	                lo = randtbl[i] ;
	                rp->state[i] = (hi << 32) | lo ;
	                if (type == TYPE_0) break ;
	            } /* end for */
		} /* end block */
		if (seed != 0) {
	            for (int i = 0 ; i < 64 ; i += 1) {
	                ihi = seed = uint(randlc(seed)) ;
	                ilo = seed = uint(randlc(seed)) ;
	                hi = ihi ;
	                lo = ilo ;
	                rp->state[i] += ((hi << 32) | lo) ;
	                if (type == TYPE_0) break ;
	            } /* end for */
	        } /* end if (seeding) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (librandom_start) */

int librandom_finish(librandom *rp) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (librandom_finish) */

int librandom_getuint(librandom *rp,uint *uip) noex {
	int		rs = SR_FAULT ;
	if (rp && uip) {
	    uint	rv ;
	    rs = SR_OK ;
	    if (rp->rand_type == TYPE_0) {
	        rv = rp->state[0] ;
	        rv = uint(randlc(rv)) ;
	        rp->state[0] = rv ;
	    } else {
	        *rp->fptr += *rp->rptr ;
	        rv = uint(*rp->fptr) ;
	        if (++rp->fptr >= rp->end_ptr) {
	            rp->fptr = rp->state ;
	            rp->rptr += 1 ;
	        } else if (++rp->rptr >= rp->end_ptr) {
	            rp->rptr = rp->state ;
	        }
	    } /* end if */
	    *uip = rv ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (librandom_getuint) */

int librandom_getint(librandom *rp,int *ip) noex {
	int		rs = SR_FAULT ;
	if (rp && ip) {
	    int		rv ;
	    rs = SR_OK ;
	    if (rp->rand_type == TYPE_0) {
	        rv = int(rp->state[0]) ;
	        rv = int(randlc(rv)) ;
	        rp->state[0] = ulong(rv) ;
	    } else {
	        *rp->fptr += *rp->rptr ;
	        rv = int((*rp->fptr >> 1) & INT_MAX) ;
	        if (++rp->fptr >= rp->end_ptr) {
	            rp->fptr = rp->state ;
	            rp->rptr += 1 ;
	        } else if (++rp->rptr >= rp->end_ptr) {
	            rp->rptr = rp->state ;
	        }
	    } /* end if */
	    *ip = int(rv) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (librandom_getint) */

int librandom_getulong(librandom *rp,ulong *ulp) noex {
	int		rs = SR_FAULT ;
	if (rp && ulp) {
	    ulong	rv, hi, lo ;
	    uint	ihi, ilo ;
	    rs = SR_OK ;
	    if (rp->rand_type == TYPE_0) {
	        ihi = uint(rp->state[0]) ;
	        ilo = uint(rp->state[0] >> 32) ;
	        ihi = uint(randlc(ihi)) ;
	        ilo = uint(randlc(ilo)) ;
	        hi = ihi ;
	        lo = ilo ;
	        rv = (hi << 32) | lo ;
	        rp->state[0] = rv ;
	    } else {
	        *rp->fptr += *rp->rptr ;
	        rv = *rp->fptr ;
	        if (++rp->fptr >= rp->end_ptr) {
	            rp->fptr = rp->state ;
	            rp->rptr += 1 ;
	        } else if (++rp->rptr >= rp->end_ptr) {
	            rp->rptr = rp->state ;
	        }
	    } /* end if */
	    *ulp = rv ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (librandom_getulong) */


