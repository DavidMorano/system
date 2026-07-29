/* tabexpand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* expand TAB characters to the appropriate number of spaces */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10 David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	tabexpand

	Description:
	This subroutine expands TAB characters in the given string
	into SPACE characters, as appropriate.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */
#include	<tabcols.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |NTABCOLS| */

#include	"tabexpand.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct dstore {
	char		*dp ;
	int		dl ;
	int		dlen ;
} ; /* end struct */


/* forward references */

local int	dstore_start(dstore *,char *,int) noex ;
local int	dstore_add(dstore *,int) noex ;
local int	dstore_finish(dstore *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tabexpand(char *dbuf,int dlen,int ntabcols,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		dcol = 0 ;
	if (dbuf && sbuf) ylikely {
	    if (dstore d ; (rs = dstore_start(&d,dbuf,dlen)) >= 0) ylikely {
	        int	sl = slen ;
	        cchar	*sp = sbuf ;
	        while ((dcol >= 0) && sl && sp[0]) {
	            if (*sp == '\t') {
	                cint	n = tabcols(ntabcols,dcol) ;
	                for (int j = 0 ; (dcol >= 0) && (j < n) ; j += 1) {
	                    dcol = dstore_add(&d,' ') ;
		        }
	            } else {
	                dcol = dstore_add(&d,*sp) ;
	            }
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        dcol = dstore_finish(&d) ;
	        if (rs >= 0) rs = dcol ;
	    } /* end if (dstore) */
	} /* end if (non-null) */
	return (rs >= 0) ? dcol : rs ;
} /* end subroutine (tabexpand) */


/* local subroutines */

local int dstore_start(dstore *dp,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (dp && dbuf) ylikely {
	    rs = SR_OK ;
	    dp->dlen = dlen ;
	    dp->dp = dbuf ;
	    dp->dl = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dstore_start) */

local int dstore_add(dstore *dp,int ch) noex {
	int		rs = SR_OVERFLOW ;
	if (dp->dl < dp->dlen) {
	    *(dp->dp)++ = charconv(ch) ;
	    dp->dl += 1 ;
	    rs = dp->dl ;
	}
	return rs ;
} /* end subroutine (dstore_add) */

local int dstore_finish(dstore *dp) noex {
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    char	*bp = dp->dp ;
	    *bp = '\0' ;
	    rs = dp->dl ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dstore_finish) */


