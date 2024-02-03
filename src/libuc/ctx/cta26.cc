/* cta26 SUPPORT */
/* lang=C++20 */

/* subroutines to convert an integer to a base-26 string */
/* version %I% last-modified %G% */

#define	CF_REMAINDER	0		/* use remainder-division */

/* revision history:

	= 1998-03-01, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cta26x

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a base-26 string that is placed into the caller supplied
	buffer (of specified length).

	Synopsis:
	int cta26XX(char *rbuf,int rlen,int type,int prec,XX v) noex

	Arguments:
	buf		caller supplied buffer
	buflen		caller supplied buffer length
	type		which alphabet 'a' or 'A'
	prec		minimum precision
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>
#include	<stdintx.h>
#include	<sncpyx.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| */

#include	"cta26.h"


/* local defines */

#define	DIGBASE		26		/* digit base */

#ifndef	CF_REMAINDER
#define	CF_REMAINDER	0		/* use remainder-division */
#endif


/* external subroutines */

extern "C" {
    int		cta26ll(char *,int,int,int,longlong) noex ;
    int		cta26ull(char *,int,int,int,ulonglong) noex ;
}


/* forward references */


static int	icta26(char *,int,int,int,ulonglong) noex ;


/* local variables */

constexpr bool	f_remainder = CF_REMAINDER ;


/* exported variables */


/* exported subroutines */

int cta26i(char *rbuf,int rlen,int type,int prec,int v) noex {
	ulonglong	ulv = ulonglong(v) ;
	return cta26ll(rbuf,rlen,type,prec,ulv) ;
}
/* end subroutine (cta26i) */

int cta26l(char *rbuf,int rlen,int type,int prec,long v) noex {
	ulonglong	ulv = ulonglong(v) ;
	return cta26ll(rbuf,rlen,type,prec,ulv) ;
}
/* end subroutine (cta26l) */

int cta26ll(char *rbuf,int rlen,int type,int prec,longlong v) noex {
	ulonglong 	ulv = ulonglong(v) ;
	cint		diglen = DIGBUFLEN ;
	int		len ;
	char		digbuf[DIGBUFLEN + 1] ;
	if (v < 0) ulv = (- ulv) ;
	len = icta26(digbuf,diglen,type,prec,ulv) ;
	if (v < 0) digbuf[diglen-(++len)] = '-' ;
	return sncpy1(rbuf,rlen,(digbuf + diglen - len)) ;
}
/* end subroutine (cta26ll) */

int cta26ui(char *rbuf,int rlen,int type,int prec,uint v) noex {
	ulonglong 	ulv = ulonglong(v) ;
	return cta26ull(rbuf,rlen,type,prec,ulv) ;
}
/* end subroutine (cta26ui) */

int cta26ul(char *rbuf,int rlen,int type,int prec,ulong v) noex {
	ulonglong 	ulv = ulonglong(v) ;
	return cta26ull(rbuf,rlen,type,prec,ulv) ;
}
/* end subroutine (cta26ul) */

int cta26ull(char *rbuf,int rlen,int type,int prec,ulonglong v) noex {
	ulonglong 	ulv = ulonglong(v) ;
	cint		diglen = DIGBUFLEN ;
	int		len ;
	char		digbuf[DIGBUFLEN + 1] ;
	len = icta26(digbuf,diglen,type,prec,ulv) ;
	return sncpy1(rbuf,rlen,(digbuf + diglen - len)) ;
}
/* end subroutine (cta26ull) */


/* local subroutines */

static int icta26(char *rbuf,int rlen,int type,int prec,ulonglong v) noex {
	constexpr uint	base = DIGBASE ;
	char		*rp = (rbuf + rlen) ;
	*rp = '\0' ;
	if (v != 0) {
	    while (v != 0) {
	        if constexpr (f_remainder) {
	            *--rp = (char) ((v % base) + type) ;
	            v = (v / base) ;
		} else {
		    const ulonglong	nv = (v / base) ;
	            *--rp = (char) ((v - (nv * base)) + type) ;
	            v = nv ;
		} /* end if (f_remainder) */
	    } /* end while */
	} else {
	    *--rp = type ;
	}
	{
	    int		n = ((rbuf + rlen) - rp) ;
	    while (n++ < prec) *--rp = type ;
	}
	return (rbuf + rlen - rp) ;
}
/* end subroutine (icta26) */


