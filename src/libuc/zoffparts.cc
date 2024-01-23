/* zoffparts SUPPORT */
/* lang=C++20 */

/* manage time-zone offsets */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1995 David A�D� Morano.  All rights reserved. */

/*******************************************************************************
 
	Name:
	zoffparts_set

	Description:
	These two small subroutine manipulate zone-offsets for use
	in time strings.  Set the value of the object from the
	number of seconds the current timezone is west of GMT.

	Symopsis:
	int zoffparts_set(XOFFPARTS *zop,int v) noex

	Arguments:
	aop		pointer to object
	v		offset from GMT (seconds west of GMT)

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
	int zoffparts_get(ZOFFPARTS *zop,int *vp) noex

	Arguments:
	zop		pointer to object
	vp		pointer to hold result (seoconds west of GMT)

	Returns:
	0		always succeeds

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstdlib>		/* for |abs(3c)| */
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"zoffparts.h"


/* local namespaces */


/* local typedefs */


/* forward references */

static int	storebuf_twodig(char *,int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int zoffparts_set(ZOFFPARTS *zop,int v) noex {
	int		rs = SR_FAULT ;
	if (zop) {
	    rs = SR_OK ;
	    zop->zoff = v ;
	    v = abs(v) / 60 ;
	    zop->hours = (v / 60) ;
	    zop->mins = (v % 60) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (zoffparts_set) */

int zoffparts_get(ZOFFPARTS *zop,int *vp) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (zop) {
	    rs = SR_OK ;
	    v = ((zop->hours * 60) + zop->mins) * 60 ;
	    if (zop->zoff < 0) v = (-v) ;
	    if (vp) *vp = v ;
	    v = abs(v) ;
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (zoffparts_get) */

int zoffparts_mkstr(ZOFFPARTS *zop,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (zop) {
	    rs = SR_OK ;
	    if (rs >= 0) {
	        cint	ch = ((zop->zoff >= 0) ? '-' : '+') ;
	        rs = storebuf_char(rbuf,rlen,i,ch) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_twodig(rbuf,rlen,i,zop->hours) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_twodig(rbuf,rlen,i,zop->mins) ;
	        i += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (zoffparts_mkstr) */


/* private subroutines */

static int storebuf_twodig(char *rbuf,int rlen,int i,int v) noex {
	int		rs = SR_OVERFLOW ;
	if ((i+2) <= rlen) {
	    rbuf[i++] = (v / 10) + '0' ;
	    rbuf[i++] = (v % 10) + '0' ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (storebuf_twodig) */

