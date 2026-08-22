/* cfxxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written by being adapted from one of
	my previous versions of the same (from the early 1980s;
	embedded work at AT&T), which itself was adapted from an
	original asembly-language version (in Digital Equipment Corp
	VAX assembly).

	= 2013-04-30, David A­D­ Morano
	I made a switch in this code from using (depending on) the
	|cfdigx(3uc)| group of subroutines (for the actual conversions)
	to the |cfsysx(3uc)| group of subroutines.  The latter uses
	the operating system standard C-language library subroutines
	to do the actual conversions, so therefore (it is assumed)
	should be faster than the older conversion routines.  See
	the notes within the source file for the subroutine group
	|cfdigx(3uc)| for more information on this change.

*/

/* Copyright © 1998,2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	cfxxx

	Description:
	Subroutines to convert digit strings to binary integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucstrto.h>		/* LIBUC |uc_strto{x}(3uc)| */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cfxxx.h"

#pragma		GCC dependency		"mod/cfsysx.ccm"

import cfsysx ;

/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* local subroutines */


/* template-subroutines */


/* exported variables */


/* exported subroutines */

int cfxxxi(cchar *sp,int sl,int b,int *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfxxxl(cchar *sp,int sl,int b,long *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfxxxll(cchar *sp,int sl,int b,longlong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfxxxui(cchar *sp,int sl,int b,uint *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfxxxul(cchar *sp,int sl,int b,ulong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfxxxull(cchar *sp,int sl,int b,ulonglong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */


