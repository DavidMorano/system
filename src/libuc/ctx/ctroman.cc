/* ctroman SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutines to convert an integer to a Roman-Numeral representation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctroman

	Description:
	These subroutines convert an integer (signed or unsigned)
	into a Roman Numeral representation.

	Synopsis:
	int ctromanXX(char *rbuf,int rlen,int v) noex

	Arguments:
	rbuf		caller supplied buffer
	rlen		caller supplied buffer length
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sbuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	ictroman(char *,int,ulonglong) noex ;


/* local variables */

static constexpr cchar	*hundreds[] = {
	"",
	"C",
	"CC",
	"CCC",
	"CD",
	"D",
	"DC",
	"DCC",
	"DCCC",
	"CM"
} ;

static constexpr cchar	*tens[] = {
	"",
	"X",
	"XX",
	"XXX",
	"XL",
	"L",
	"LX",
	"LXX",
	"LXXX",
	"XC"
} ;

static constexpr cchar	*ones[] = {
	"",
	"I",
	"II",
	"III",
	"IV",
	"V",
	"VI",
	"VII",
	"VIII",
	"IX"
} ;


/* exported variables */


/* exported subroutines */

int ctromani(char *dbuf,int dlen,int v) noex {
	ulonglong	ulv = (ulonglong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromani) */

int ctromanl(char *dbuf,int dlen,long v) noex {
	ulonglong	ulv = (ulonglong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromanl) */

int ctromanll(char *dbuf,int dlen,longlong v) noex {
	ulonglong	ulv = (longlong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromanll) */

/* unsigned */
int ctromanui(char *dbuf,int dlen,uint v) noex {
	ulonglong	ulv = (ulonglong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromanui) */

/* unsigned */
int ctromanul(char *dbuf,int dlen,ulong v) noex {
	ulonglong	ulv = (ulonglong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromanul) */

/* unsigned */
int ctromanull(char *dbuf,int dlen,ulonglong v) noex {
	ulonglong	ulv = (longlong) v ;
	return ictroman(dbuf,dlen,ulv) ;
}
/* end subroutine (ctromanull) */


/* local subroutines */

static int ictroman(char *dbuf,int dlen,ulonglong v) noex {
	sbuf		b ;
	int		rs ;
	int		rs1 ;
	mainv		tabs[] = { hundreds, tens, ones } ;
	if ((rs = sbuf_start(&b,dbuf,dlen)) >= 0) {
	    cint	ntabs = nelem(tabs) ;
	    ulonglong	n = 1000 ;
	    if (v >= n) {
	        cint		i = (v/n) ;
	        rs = sbuf_chrs(&b,'M',i) ;
	        v = (v%n) ;
	    }
	    n /= 10 ;
	    for (int r = 0 ; (rs >= 0) && (r < ntabs) ; r += 1) {
	        if (v >= n) {
	            cint	i = (v/n) ;
	            rs = sbuf_strw(&b,tabs[r][i],-1) ;
	            v = (v%n) ;
	        }
	        n /= 10 ;
	    } /* end for */
	    rs1 = sbuf_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	return rs ;
}
/* end subroutine (ictroman) */


