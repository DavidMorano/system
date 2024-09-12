/* cthex SUPPORT */
/* lang=C++20 */

/* subroutines to convert an integer to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines that perform conversions which also have
	an integral power-of-two base are much faster than anything
	that uses |lltostr(3c)| (or friends) because there are no
	division operations needed in the algorithm used.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<cvtdig.hh>

#include	"cthex.h"


/* local defines */

#define	OURBASE		16


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported variables */


/* exported subroutines */

int cthexc(char *dp,int dl,int val) noex {
	uint		uv = uint(val) ;
	cint		n = sizeof(char) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexc) */

int cthexs(char *dp,int dl,int val) noex {
	uint		uv = uint(val) ;
	cint		n = sizeof(short) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexs) */

int cthexi(char *dp,int dl,int val) noex {
	uint		uv = uint(val) ;
	cint		n = sizeof(int) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexi) */

int cthexl(char *dp,int dl,long val) noex {
	ulong		uv = ulong(val) ;
	cint		n = sizeof(long) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexl) */

int cthexll(char *dp,int dl,longlong val) noex {
	ulonglong	uv = ulonglong(val) ;
	cint		n = sizeof(longlong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexll) */

int cthexuc(char *dp,int dl,uint uv) noex {
	cint		n = sizeof(uchar) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexuc) */

int cthexus(char *dp,int dl,uint uv) noex {
	cint		n = sizeof(ushort) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexus) */

int cthexui(char *dp,int dl,uint uv) noex {
	cint		n = sizeof(uint) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexui) */

int cthexul(char *dp,int dl,ulong uv) noex {
	cint		n = sizeof(ulong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexul) */

int cthexull(char *dp,int dl,ulonglong uv) noex {
	cint		n = sizeof(ulonglong) ;
	return cvtdig(dp,dl,uv,n,b) ;
}
/* end subroutine (cthexull) */


