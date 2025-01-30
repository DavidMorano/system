/* tabexpand SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* expand TAB characters to the appropriate number of spaces */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10 David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine expands TAB characters in the given string
	into SPACE characters, as appropriate.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<tabcols.h>
#include	<char.h>
#include	<localmisc.h>		/* |NTABCOLS| */

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
} ;


/* forward references */

static int	dstore_start(dstore *,char *,int) noex ;
static int	dstore_add(dstore *,int) noex ;
static int	dstore_finish(dstore *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tabexpand(char *dbuf,int dlen,int ntabcols,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		dcol = 0 ;
	if (dbuf && sbuf) {
	    dstore	d ;
	    if ((rs = dstore_start(&d,dbuf,dlen)) >= 0) {
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
}
/* end subroutine (tabexpand) */


/* local subroutines */

static int dstore_start(dstore *dp,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (dp && dbuf) {
	    rs = SR_OK ;
	    dp->dlen = dlen ;
	    dp->dp = dbuf ;
	    dp->dl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dstore_start) */

static int dstore_add(dstore *dp,int c) noex {
	int		rs = SR_OVERFLOW ;
	if (dp->dl < dp->dlen) {
	    *(dp->dp)++ = c ;
	    dp->dl += 1 ;
	    rs = dp->dl ;
	}
	return rs ;
}
/* end subroutine (dstore_add) */

static int dstore_finish(dstore *dp) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    char	*bp = dp->dp ;
	    *bp = '\0' ;
	    rs = dp->dl ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dstore_finish) */


