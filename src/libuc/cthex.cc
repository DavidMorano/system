/* cthex SUPPORT */
/* lang=C++20 */

/* subroutines to convert an integer to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines that perform conversions which also have
	an integral power-of-two base are much faster than anything
	that uses 'lltostr(3c)' (or friends) because there are no
	division operations needed in the algorithm used.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<cvtdig.hh>
#include	<localmisc.h>

#include	"cthex.h"


/* local defines */

#define	OURBASE		16


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported subroutines */

int cthexc(char *dp,int dl,int val) noex {
	uint		uv = val ;
	const int	n = sizeof(char) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexc) */

int cthexs(char *dp,int dl,int val) noex {
	uint		uv = val ;
	const int	n = sizeof(short) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexs) */

int cthexi(char *dp,int dl,int val) noex {
	uint		uv = val ;
	const int	n = sizeof(int) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexi) */

int cthexl(char *dp,int dl,long val) noex {
	ulong		uv = val ;
	const int	n = sizeof(long) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexl) */

int cthexll(char *dp,int dl,longlong val) noex {
	ulonglong	uv = val ;
	const int	n = sizeof(longlong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexll) */

int cthexuc(char *dp,int dl,uint val) noex {
	uint		uv = val ;
	const int	n = sizeof(uchar) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexuc) */

int cthexus(char *dp,int dl,uint val) noex {
	uint		uv = val ;
	const int	n = sizeof(ushort) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexus) */

int cthexui(char *dp,int dl,uint uv) noex {
	const int	n = sizeof(uint) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexui) */

int cthexul(char *dp,int dl,ulong uv) noex {
	const int	n = sizeof(ulong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexul) */

int cthexull(char *dp,int dl,ulonglong uv) noex {
	const int	n = sizeof(ulonglong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexull) */


