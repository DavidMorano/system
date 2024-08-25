/* testintmax SUPPORT */
/* lang=C++20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstdio>
#include	<ndigit.h>

int main() {
	int	n ;
	int	v = INT_MAX ;

	n = ndigit(v,10) ;

	printf("v=%d n=%u\n",v,n) ;

	return 0 ;
}
/* end subroutine (main) */


