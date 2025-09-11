/* fwrite SUPPORT */
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
	fwrite

	Description:
	This is a knock-off of the |fwrite(3c)| subroutine, only
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

int fwrite(FILE *fp,cvoid *lbuf,int µllen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fp && lbuf) {
	    cchar *sbuf = charp(lbuf) ;
	    rs = SR_INVALID ;
	    if (cint llen = getlenstr(sbuf,µllen) ; llen > 0) {
		csize lsize = size_t(llen) ;
		csize nsize = 1 ;
	        if (size_t res = fwrite(lbuf,lsize,nsize,fp) ; res > 0) {
		    rs = SR_OK ;
	            wlen += int(res) ;
		} else {
		    rs = (errno) ? (- errno) : SR_OK ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fwrite) */


