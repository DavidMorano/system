/* cfroman SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* convert from a Roman-Numeral representation to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-20, David A­D­ Morano
	For fun, really. Do not have a real use.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	cfroman{x}

	Description:
	Subroutines to convert strings of Roman-Numerals to binary
	integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"cfroman.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	conv(cchar *,int,ulonglong *) noex ;
static int	toval(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cfromani(cchar *sp,int sl,int *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = int(val) ;
	return rs ;
}
/* end subroutine (cfromani) */

int cfromanl(cchar *sp,int sl,long *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = long(val) ;
	return rs ;
}
/* end subroutine (cfromanl) */

int cfromanll(cchar *sp,int sl,longlong *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = longlong(val) ;
	return rs ;
}
/* end subroutine (cfromanll) */

int cfromanui(cchar *sp,int sl,uint *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = uint(val) ;
	return rs ;
}
/* end subroutine (cfromanui) */

int cfromanul(cchar *sp,int sl,ulong *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = ulong(val) ;
	return rs ;
}
/* end subroutine (cfromanul) */

int cfromanull(cchar *sp,int sl,ulonglong *rp) noex {
	ulonglong	val ;
	int		rs ;
	rs = conv(sp,sl,&val) ;
	if (rp) *rp = ulonglong(val) ;
	return rs ;
}
/* end subroutine (cfromanull) */


/* local subroutines */

static int conv(cchar *sbuf,int slen,ulonglong *rp) noex {
	int		val = 0 ;
	cchar		*sp ;
	if (int sl ; (sl = sfshrink(sbuf,slen,&sp)) > 0) {
	    int		cval = toval(sp[0]) ;
	    int		nval ;
	    for (int i = 0 ; i < sl ; i += 1) {
	        nval = (i < (sl - 1)) ? toval(sp[i + 1]) : 0 ;
	        if (cval < nval) {
		    val -= cval ;
	        } else {
		    val += cval ;
	        }
		cval = nval ;
	    } /* end for */
	    *rp = val ;
	} /* end if (positive) */
	return 0 ;
}
/* end subroutine (conv) */

static int toval(int ch) noex {
	int		val = 0 ;
	switch (ch) {
	case 'I': val = 1 ; break ;
	case 'V': val = 5 ; break ;
	case 'X': val = 10 ; break ;
	case 'L': val = 50 ; break ;
	case 'C': val = 100 ; break ;
	case 'D': val = 500 ; break ;
	case 'M': val = 1000 ; break ;
	} /* end switch */
	return val ;
}
/* end subroutine (toval) */


