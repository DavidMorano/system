/* ctbin */
/* lang=C++20 */

/* subroutines to convert an integer to a binary-digit string */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines that perform conversions which also have an integral
	power-of-two base are much faster than anything that uses 'lltostr(3c)'
	(or friends) because there are no division operations needed in the
	algorithm used.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cvtdig.hh>
#include	<localmisc.h>

#include	"ctbin.h"


/* local defines */

#define	OURBASE		2


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported subroutines */

int ctbini(char *dp,int dl,int val) noex {
	uint		uv = val ;
	const int	n = sizeof(int) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbini) */

int ctbinl(char *dp,int dl,long val) noex {
	ulong		uv = val ;
	const int	n = sizeof(long) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinl) */

int ctbinll(char *dp,int dl,longlong val) noex {
	ulonglong	uv = val ;
	const int	n = sizeof(longlong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinll) */

int ctbinui(char *dp,int dl,uint uv) noex {
	const int	n = sizeof(uint) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinui) */

int ctbinul(char *dp,int dl,ulong uv) noex {
	const int	n = sizeof(ulong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinul) */

int ctbinull(char *dp,int dl,ulonglong uv) noex {
	const int	n = sizeof(ulonglong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctbinull) */


