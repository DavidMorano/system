/* snwcpyclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a source c-string to a destination while cleaning it up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This was written to clean up some problems with printing
	garbage characters in a few places in some PCS utilities.

	= 2023-01-22, David A-D- Morano
	I changed this to use the new C++23 constant-expression
	capability of |bitset(3c++)|.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyclean

	Description:
	This is essentially the same as the |snwcpy(3uc)| subroutine
	except that garbage characters are replaced with a specified
	substitute character.  Only printable characters along with
	Bell, Tab, BackSpace, and NewLine are passed through.  All
	other characters are replaced with the substitution character.

	Synopsis:
	int snwcpyclean(char *dp,int dl,int sch,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sch		optional substitution character
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* |bitset(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    constexpr cint	chtabsz = (UCHAR_MAX + 1) ;
    struct ourmgr {
	bitset<chtabsz>	isour ;
	constexpr void mkprint(bitset<chtabsz> &s) noex {
	    for (int ch = 0 ; ch < chtabsz ; ch += 1) {
		if (((ch & 0x7F) >= 0x20) && (ch != CH_DEL)) {
		    s.set(ch,true) ;
		}
	    } /* end for */
	} ;
	constexpr void mkextra(bitset<chtabsz> &s) noex {
	    s.set(CH_BEL,true) ;
	    s.set(CH_TAB,true) ;
	    s.set(CH_BS,true) ;
	    s.set(CH_NL,true) ;
	} ;
	constexpr ourmgr() noex {
	    mkprint(isour) ;
	    mkextra(isour) ;
	} ;
    } ; /* end struct (ourmgr) */
} /* end namespace */


/* forwards references */

static inline bool	isour(int) noex ;


/* local variables */

constexpr ourmgr	our_data ;


/* exported variables */


/* exported subroutines */

int snwcpyclean(char *dbuf,int dlen,int sch,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		dl = 0 ; /* return-value */
	if (dbuf && sp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        rs = SR_OK ;
	        while (dlen-- && sl && *sp) {
	            if (cint ch = mkchar(*sp) ; isour(ch)) {
		        dbuf[dl++] = char(ch) ;
	            } else if (sch != 0) {
		        dbuf[dl++] = char(sch) ;
	            }
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        if (sl && *sp) rs = SR_OVERFLOW ;
	    } /* end if (valid) */
	    dbuf[dl] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyclean) */


/* local subroutines */

static bool isour(int ch) noex {
    	return our_data.isour[ch] ;
}
/* end subroutine (isour) */


