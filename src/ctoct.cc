/* ctoct */
/* lang=C++20 */

/* subroutines to convert an integer to a HEX string */


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

#include	"ctoct.h"


/* local defines */

#define	OURBASE		8


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported subroutines */

int ctocti(char *dp,int dl,int val) noex {
	uint		uv = val ;
	const int	n = sizeof(int) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctocti) */

int ctoctl(char *dp,int dl,long val) noex {
	ulong		uv = val ;
	const int	n = sizeof(long) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctoctl) */

int ctoctll(char *dp,int dl,longlong val) noex {
	ulonglong	uv = val ;
	const int	n = sizeof(longlong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctoctll) */

int ctoctui(char *dp,int dl,uint uv) noex {
	const int	n = sizeof(uint) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctoctui) */

int ctoctul(char *dp,int dl,ulong uv) noex {
	const int	n = sizeof(ulong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctoctul) */

int ctoctull(char *dp,int dl,ulonglong uv) noex {
	const int	n = sizeof(ulonglong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (ctoctull) */


