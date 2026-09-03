/* fputch SUPPORT */
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
	fputch

	Description:
	This is a knock-off of the 'fwrite(3)' subroutine, only
	sensible!

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* STD-C |UCHAR_MAX| */
#include	<cstddef>		/* STD-C */
#include	<cstdlib>		/* STD-C */
#include	<cstdio>		/* STD-C */
#include	<cstring>		/* STD-C */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libf.h"

import libfsup ;			/* |getlenstr(3u)| */
import modsys ;				/* |intsat(3u)| */

/* local defines */


/* external subroutines */

extern "C++" {
    int	fwrite(FILE *,cvoid *,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fputch(FILE *fp,int ch) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fp) ylikely {
	    char wbuf[2] = { char(ch) } ;
	    rs = fwrite(fp,wbuf,1) ;
	    wlen += rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fputch) */


