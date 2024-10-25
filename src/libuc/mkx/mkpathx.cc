/* mkpathx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a file-path from multiple component c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A�D� Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright � 2001 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine constructs a file path out of one or more path
        componets.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	"mkpathx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mknpathx(char *,int,int,...) noex ;
}


/* external variables */


/* local strutures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkpath1(char *pp,cc *s1) noex {
	return mknpathx(pp,-1,1,s1) ;
}
/* end subroutine (mkpath1) */

int mkpath2(char *pp,cc *s1,cc *s2) noex {
	return mknpathx(pp,-1,2,s1,s2) ;
}
/* end subroutine (mkpath2) */

int mkpath3(char *pp,cc *s1,cc *s2,cc *s3) noex {
	return mknpathx(pp,-1,3,s1,s2,s3) ;
}
/* end subroutine (mkpath3) */

int mkpath4(char *pp,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mknpathx(pp,-1,4,s1,s2,s3,s4) ;
}
/* end subroutine (mkpath4) */

int mkpath5(char *pp,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mknpathx(pp,-1,5,s1,s2,s3,s4,s5) ;
}
/* end subroutine (mkpath5) */


