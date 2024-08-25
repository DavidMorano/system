/* testhdrextnum SUPPORT */
/* lang=C99 */

/* test of |hdrextnum(3mailmsg)| */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<stdio.h>
#include	<hdrextnum.h>
#include	<localmisc.h>


int main() {
	FILE		*fp = stdout ;
	int		v ;
	int		sl = -1 ;
	cchar	*sp = "(this) 8 (is the thing)" ;

	v = hdrextnum(sp,sl) ;

	fprintf(fp,"v=%d\n",v) ;
	fflush(fp) ;

	return 0 ;
}
/* end subroutine (main) */


