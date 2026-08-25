/* zoffparts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage time-zone offsets */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	Name:
	zoffparts_set

	Description:
	These two small subroutines manipulate zone-offsets for use
	in time strings.  Set the value of the object from the
	number of (signed) seconds the current timezone is west of
	GMT.

	Symopsis:
	int zoffparts_set(XOFFPARTS *zop,int v) noex

	Arguments:
	aop		pointer to object
	v		offset from GMT (signed seconds west of GMT)

	Returns:
	0		always succeeds


	Name:
	zoffparts_get

	Description:
	Get the number of seconds that the current timezone is west
	of GMT from the object.  We do not care about whether the
	offset (from GMT) is positive or negative.  Someone else,
	someplace else, cares about that.

	Sysnopsis:
	int zoffparts_get(zoffparts *zop,int *vp) noex

	Arguments:
	zop		pointer to object
	vp		pointer to hold result (seoconds west of GMT)

	Returns:
	0		always succeeds

	Note:
	1. The member variable |zoff| is in SECONDS.
	2. The "zone-offset" (member variable |zoff|) for these
	subroutines is in the time-units of SECONDS.  It is not in
	minutes, as it is in other interfaces (which will remain
	nameless -- for now).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |abs(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<storebuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"zoffparts.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* forward references */

local int	storebuf_twodig(char *,int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int zoffparts_set(zoffparts *zop,int zo) noex {
	int		rs = SR_FAULT ;
	if (zop) ylikely {
	    cint	v = abs(zo) / 60 ;	/* discard seconds */
	    zop->zoff = zo ;
	    rs = SR_OK ;
	    zop->hours	= (v / 60) ;
	    zop->mins	= (v % 60) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (zoffparts_set) */

int zoffparts_get(zoffparts *zop,int *vp) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (zop) ylikely {
	    rs = SR_OK ;
	    v = ((zop->hours * 60) + zop->mins) * 60 ; /* create seconds */
	    if (zop->zoff < 0) v = (neg v) ;
	    if (vp) *vp = v ;
	    v = abs(v) ;
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
} /* end subroutine (zoffparts_get) */

int zoffparts_mkstr(zoffparts *zop,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zop) ylikely {
	    rs = SR_OK ;
	    if (rs >= 0) ylikely {
	        cint	ch = ((zop->zoff >= 0) ? '-' : '+') ;
	        rs = storebuf_chr(rbuf,rlen,i,ch) ;
	        i += rs ;
	    }
	    if (rs >= 0) ylikely {
	        rs = storebuf_twodig(rbuf,rlen,i,zop->hours) ;
	        i += rs ;
	    }
	    if (rs >= 0) ylikely {
	        rs = storebuf_twodig(rbuf,rlen,i,zop->mins) ;
	        i += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (zoffparts_mkstr) */


/* private subroutines */

local int storebuf_twodig(char *rbuf,int rlen,int i,int v) noex {
	cint		n = 2 ;
	int		rs = SR_OVERFLOW ;
	if ((i + n) <= rlen) ylikely {
	    rbuf[i++] = charconv((v / 10) + '0') ;
	    rbuf[i++] = charconv((v % 10) + '0') ;
	    rs = n ;
	} /* end if */
	return rs ;
} /* end subroutine (storebuf_twodig) */

int zoffparts::set(int zo) noex {
	return zoffparts_set(this,zo) ;
} /* end method */

int zoffparts::get(int *vp) noex {
	return zoffparts_get(this,vp) ;
} /* end method */

int zoffparts::mkstr(char *rbuf,int rlen) noex {
	return zoffparts_mkstr(this,rbuf,rlen) ;
} /* end method */


