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

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* STD-C */
#include	<cstdlib>		/* STD-C */
#include	<cstdio>		/* STD-C */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libf.h"

import libfsup ;			/* |getlenstr()| */
import modsys ;				/* |intsat(3modsys)| */


/* local defines */


/* external subroutines */

extern "C++" {
    int fwrite(FILE *,cvoid *,int) noex ;
} /* end extern (C++) */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fwriter(FILE *fp,cvoid *lbuf,int µllen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fp && lbuf) ylikely {
	    cchar *sbuf = charp(lbuf) ;
	    rs = SR_OK ;
	    if (cint llen = getlenstr(sbuf,µllen) ; llen > 0) ylikely {
		csize nsize = size_t(llen) ;
		csize lsize = 1 ;
	        if (size_t res = fwrite(lbuf,lsize,nsize,fp) ; res > 0) {
	            wlen += intsat(res) ;
		} else {
		    rs = (errno) ? (neg errno) : SR_OK ;
		} /* end if (error) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fwriter) */

int fwrite(FILE *fp,cvoid *wbuf,int wlen) noex {
    	return fwriter(fp,wbuf,wlen) ;
} /* end subroutine */


