/* cfnum SUPPORT */
/* lang=C++20 */

/* convert from a number to an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfnum

	Description:
	These subroutines convert numeric strings in a variety of
	formats or bases into the integer-binary form.  These
	subroutines handle some cases that the |atox(3c)| family
	do not.  Also, we handle Latin-1 characters correctly;
	specifically with our use of:

		tolc(3uc)
		isdigitlatin(3uc)
		isalphalatin(3uc)

	instead of:

		tolower(3c)
		isdigit(3c)
		islapha(3c)

	The standard subroutines often go crazy when confronted with Latin-1
	characters with values >= 128.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* for |strnlen(3c)| */
#include	<concepts>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<cfbin.h>
#include	<cfoct.h>
#include	<cfdec.h>
#include	<cfhex.h>
#include	<char.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| */


/* local defines */


/* local namespaces */

using std::integral ;			/* concept */
using std::signed_integral ;		/* concept */
using std::unsigned_integral ;		/* concept */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

static int sfsign(bool *bp,cchar *sp,int sl,cchar **rpp) noex {
	bool		fn = false ;
	if (sl < 0) sl = strlen(sp) ;
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if ((sl > 0) && isplusminus(*sp)) {
	    fn = (*sp == '-') ;
	    sp += 1 ;
	    sl -= 1 ;
	}
	*rpp = sp ;
	*bp = fn ;
	return sl ;
}
/* end subroutine (sfsign) */


/* subroutine-templates */

template<unsigned_integral T>
int cfnumx(cchar *sp,int sl,T *rp) noex {
	int		rs = SR_DOM ;
	cchar		*bp{} ;
	bool		fneg = false ;
	if (int bl ; (bl = sfsign(&fneg,sp,sl,&bp)) > 0) {
	    int		ch ;
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 1) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdec(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhex(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoct(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbin(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdec(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhex(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoct(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbin(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfoct((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdec(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdec(bp,bl,rp) ;
		}
	    } /* end if */
	    if (fneg) *rp = (- *rp) ;
	} /* end if (sfsign) */
	return rs ;
}
/* end subroutine-template (cfnumx) */

template<unsigned_integral UT,signed_integral T>
int cfnumsx(cchar *bp,int bl,T *rp) noex {
	int		rs = SR_DOM ;
	cchar		*sp{} ;
	bool		fneg{} ;
	if (int sl ; (sl = sfsign(&fneg,bp,bl,&sp)) > 0) {
	    UT		uval{} ;
	    if ((rs = cfnumx(sp,sl,&uval)) >= 0) {
		if (fneg) uval = (- uval) ;
		*rp = static_cast<T>(uval) ;
	    } /* end if (cfnumx) */
	} /* end if (sfsign) */
	return rs ;
}
/* end subroutine-template (cfnumsx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cfnumi(cchar *bp,int bl,int *rp) noex {
	return cfnumsx<uint>(bp,bl,rp) ;
}
/* end subroutine (cfnumi) */

int cfnuml(cchar *bp,int bl,long *rp) noex {
	return cfnumsx<ulong>(bp,bl,rp) ;
}
/* end subroutine (cfnuml) */

int cfnumll(cchar *bp,int bl,longlong *rp) noex {
	return cfnumsx<ulonglong>(bp,bl,rp) ;
}
/* end subroutine (cfnumll) */

int cfnumui(cchar *bp,int bl,uint *rp) noex {
	return cfnumx(bp,bl,rp) ;
}
/* end subroutine (cfnumui) */

int cfnumul(cchar *bp,int bl,ulong *rp) noex {
	return cfnumx(bp,bl,rp) ;
}
/* end subroutine (cfnumul) */

int cfnumull(cchar *bp,int bl,ulonglong *rp) noex {
	return cfnumx(bp,bl,rp) ;
}
/* end subroutine (cfnumull) */


