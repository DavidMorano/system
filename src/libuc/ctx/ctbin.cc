/* ctbin SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines to convert an integer to a binary-digit string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ctbin

	Description:
	These subroutines that perform conversions which also have
	an integral power-of-two base are much faster than anything
	that uses |lltostr(3c)| (or friends) because there are no
	division operations needed in the algorithm used.

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

#include	"ctbin.h"


import cvtdig ;

/* local defines */

#define	OURBASE		2


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported variables */


/* exported subroutines */

int ctbini(char *dp,int dl,int val) noex {
	uint		uv = uint(val) ;
	cint		n = szof(int) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbini) */

int ctbinl(char *dp,int dl,long val) noex {
	ulong		uv = ulong(val) ;
	cint		n = szof(long) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinl) */

int ctbinll(char *dp,int dl,longlong val) noex {
	ulonglong	uv = ulonglong(val) ;
	cint		n = szof(longlong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinll) */

int ctbinui(char *dp,int dl,uint uv) noex {
	cint		n = szof(uint) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinui) */

int ctbinul(char *dp,int dl,ulong uv) noex {
	cint		n = szof(ulong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinul) */

int ctbinull(char *dp,int dl,ulonglong uv) noex {
	cint		n = szof(ulonglong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinull) */


