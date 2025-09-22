/* fprint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* print some (binary) data out to a STDIO stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	fprint

	Description:
	This is a knock-off of the 'fwrite(3)' subroutine, only
	sensible!

******************************************************************************/

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<intsat.h>
#include	<localmisc.h>

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */


/* local defines */


/* external subroutines */

extern "C++" {
    int fwrite(FILE *,cvoid *,int) noex ;
}


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fprint(FILE *fp,cchar *lbuf,int µllen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	if(fp && lbuf) {
	    if (int llen ; (llen = getlenstr(lbuf,µllen)) >= 0) {
	        if (llen > 0) {
	            rs = fwrite(fp,lbuf,llen) ;
	            wlen += rs ;
		}
	        if ((rs >= 0) && ((llen == 0) || (lbuf[llen-1] != '\n'))) {
	            rs = fwrite(fp,"\n",1) ;
	            wlen += rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fprint) */


