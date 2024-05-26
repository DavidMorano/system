/* testfmtspec SUPPORT */
/* lang=C++98 */

/* test the FMTSPEC object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This program shows (prints out) object addresess.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstdio>
#include	<string>		/* |strlen(3c)| */
#include	<new>
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<string>
#include	<iostream>
#include	<usystem.h>
#include	<localmisc.h>

#include	"fmtspec.hh"


/* local defines */


/* imported namespaces */

using namespace		std ;		/* yes, we want punishment! */


/* external subroutines */


/* local structures */


/* forward references */

static int sub(int,mainv,...) ;


/* local variables */

constexpr cpcchar	fmts[] = {
	"4.3dxxx",
	"08xxxx",
	"10dxxx",
	"10uxxx",
	"10oxxx",
	"10hljhljdx",
	"12lldxx"
} ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	sub(argc,argv,envv) ;
}
/* end subroutine (main) */


/* local subroutines */

static int sub(int argc,mainv argv,...) {
	va_list		ap ;
	fmtspec		fo ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    va_begin(ap,argv) ;
	    for (auto &sp : fmts) {
		cint	sl = strlen(sp) ;
	        if ((rs = fo.start(ap,sp,sl)) >= 0) {
		    {
			printf("fmt=%s\n",sp) ;
			printf("rs=%d\n",rs) ;
			printf("w=%d\n",fo.width) ;
			printf("p=%d\n",fo.prec) ;
			printf("m=%d\n",fo.lenmod) ;
			printf("c=%c\n",fo.fcode) ;
			printf("c=%d\n",fo.skiplen) ;
		    }
		    rs1 = fo.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end of (fmtspec) */
	    } /* end for */
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (sub) */


