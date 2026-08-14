/* bprinter SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* print a line without trailing white-space */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1992-03-01, David A­D­ Morano
	This code was originally written.

	= 1998-09-01, David A­D­ Morano
	This subroutine was modified to remove white-space in front
	of trailing white-space also.

*/

/* Copyright © 1992,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is meant to work only on TROFF source language (of
	whatever sort).

        This subroutine removes trailing white-space from a line, and then
        prints it. But if the line ends up as having only soe punctuation left
        to it and that punctuation starts in the first column, then some
        non-printing blank character is stuffed at the fron of the line to
        prevent the punctuation from starting the line. In the case of a period,
        this is particularly important since that might falsely indicate that a
        TROFF macro is present.

	Synopsis:
	int bprinter(bfile *tfp,int f_bol,cchar *lp,int ll)

	Arguments:
	tfp		pointer to BFILE object
	f_bol		flag indicating Beginning-Of-Line or not
	lp		character buffer to print
	ll		length of buffer to print

	Returns:
	-		number of characters (bytes) printed 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"bprinter.h"


/* local defines */

#undef	NSPUNCTS
#define	NSPUNCTS	10		/* max number of puncts to store */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr char		puncts[] = ".,;!?:" ;


/* exported variables */


/* exported subroutines */

int bprinter(bfile *tfp,int f_bol,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (tfp && lp) {
	    rs = SR_OK ;
	if (ll > 0) {
	    cint	splen = NSPUNCTS ;
	    cint	sch = MKCHAR(lp[0]) ;
	    int		spl = 0 ;
	    bool	f_preserve ;
	    int		f = false ;
	    char	spuncts[NSPUNCTS+1] ;
	    f_preserve = ((strchr(puncts,sch)) != nullptr) && f_bol ;
	    if (lp[ll-1] == '\n') ll -= 1 ;
	    while (ll && CHAR_ISWHITE(lp[ll-1])) ll -= 1 ;
	    while (ll && lp[0]) {
		int	ch = MKCHAR(lp[ll-1]) ;
		if (strchr(puncts,ch) != nullptr) {
		    if (spl < splen) {
			int	i = (splen-1-spl++) ;
			spuncts[i] = char(ch) ;
		    } else {
			f = true ;
		    }
		} else if (! CHAR_ISWHITE(ch)) {
		    f = true ;
		}
		if (f) break ;
		ll -= 1 ;
	    } /* end while */
	    if (ll > 0) {
		rs = bwrite(tfp,lp,ll) ;
		wlen += rs ;
	    }
	    if ((rs >= 0) && (spl > 0)) {
		if ((ll == 0) && (! f_preserve)) {
		    rs = bwrite(tfp,"\\&",-1) ;
		    wlen += rs ;
	        }
		if (rs >= 0) {
		    int	i = (splen-spl) ;
		    rs = bwrite(tfp,(spuncts+i),spl) ;
		    wlen += rs ;
		}
	    }
	} /* end if (non-zero-length line) */
	if (rs >= 0) {
	    rs = bputc(tfp,CH_LF) ;
	    wlen += rs ;
	}
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bprinter) */


