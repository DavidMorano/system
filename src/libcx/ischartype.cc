/* ischartype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* charcter classification tests */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<bitset>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libcx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isdigex(int ch) noex {
	bool	f = false ;
	f = f || ((ch >= '0') && (ch <= '9')) ;
	f = f || ((ch >= 'a') && (ch <= 'f')) ;
	f = f || ((ch >= 'A') && (ch <= 'F')) ;
	return f ;
} /* end subroutine (isdigex) */

bool iswhite(int ch) noex {
    	constexpr cchar cs[] = " \t\b\v\f\r" ;
	return (strchr(cs,ch) != nullptr) ;
} /* end subroutine (isdigit) */


/* unused subroutines */

#ifdef	COMMENT
bool isalpha(int ch) noex {
	cint	mch = (ch & UCHAR_MAX) ;
	int	f = false ;
	f = f || ((mch >= 'a') && (mch <= 'z')) ;
	f = f || ((mch >= 'A') && (mch <= 'Z')) ;
	f = f || ((mch >= UC('À')) && (mch <= UC('Ö'))) ;
	f = f || ((mch >= UC('Ø')) && (mch <= UC('æ'))) ;
	f = f || ((mch >= UC('è')) && (mch <= UC('ö'))) ;
	f = f || ((mch >= UC('ø')) && (mch <= UC('ÿ'))) ;
	return f ;
} /* end subroutine (isalpha) */
#endif /* COMMENT */

#ifdef	COMMENT
bool isalnum(int ch) noex {
	cint	mch = (ch & UCHAR_MAX) ;
	int	f = false ;
	f = f || isalpha(mch) ;
	f = f || isdigit(mch) ;
	return f ;
} /* end subroutine (isalnum) */
#endif /* COMMENT */

#ifdef	COMMENT
bool isdigit(int ch) noex {
	return ((ch >= '0') && (ch <= '9')) ;
} /* end subroutine (isdigit) */
#endif /* COMMENT */


