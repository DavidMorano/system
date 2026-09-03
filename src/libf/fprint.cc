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

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* STD-C */
#include	<cstdlib>		/* STD-C */
#include	<cstdio>		/* STD-C |FILE(3stdio)| */
#include	<cstring>		/* STD-C */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libf.h"

import libfsup ;			/* |getlenstr(3u)| */


/* local defines */


/* external subroutines */

extern "C++" {
    int fwrite(FILE *,cvoid *,int) noex ;
} /* end extern (C) */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fprint(FILE *fp,cchar *lbuf,int µllen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	if(fp && lbuf) ylikely {
	    if (int llen ; (llen = getlenstr(lbuf,µllen)) >= 0) ylikely {
	        if (llen > 0) {
	            rs = fwrite(fp,lbuf,llen) ;
	            wlen += rs ;
		}
	        if ((rs >= 0) && ((llen == 0) || (lbuf[llen-1] != CH_NL))) {
	            rs = fputch(fp,CH_NL) ;
	            wlen += rs ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fprint) */


