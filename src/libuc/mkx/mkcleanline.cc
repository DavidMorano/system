/* mkcleanline SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create a clean (cleaned up) line of text */
/* version %I% last-modified %G% */

#define	CF_CLEAN1	0		/* use CLEAN-1 */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkcleanline

	Description:
	This subroutine cleans up a line of text.  A mode paramter
	specifies how clean the resulting line gets.  To wit:

	mode	delete­or­sub	terminate­on­nul
	----------------------------------------
	0	delete		YES
	1	sub		YES
	2	delete		NO
	3	sub		NO

	Synopsis:
	int mkcleanline(char *lp,int ll,int m) noex

	Arguments:
	lp		line-buffer to get operated on (must be writable)
	ll		length of line-buffer on input
	m		mode parameter (see above on use of mode)

	Returns:
	-		length of resulting line (not NL terminated)

	Important note:
	Resulting lines are nver NL (EOL) terminated - even if the
	input line was!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ascii.h>
#include	<ischarx.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */

#define	MFLAGS		struct mflags

#ifndef	CF_CLEAN1
#define	CF_CLEAN1	0		/* use CLEAN-1 */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct mflags {
	uint	subnul:1 ;
	uint	subbad:1 ;
} ;


/* forward references */

static int	clean1(MFLAGS *,char *,int) noex ;
static int	clean2(MFLAGS *,char *,int) noex ;

static int	isshift(int) noex ;
static int	ischarok(int) noex ;
static int	isend(int) noex ;


/* local variables */

cbool		f_clean1 = CF_CLEAN1 ;	/* use CLEAN-1 */


/* exported variables */


/* exported subroutines */

int mkcleanline(char *lp,int ll,int m) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (lp) {
	    MFLAGS	mf{} ;
	    if (ll < 0) ll = cstrlen(lp) ;
	    while ((ll > 0) && isend(lp[ll - 1])) {
	        ll -= 1 ;
	    }
	    mf.subnul = !!(m & 1) ;
	    mf.subbad = !!(m & 2) ;
	    if_constexpr (f_clean1) {
	        len = clean1(&mf,lp,ll) ;
	    } else {
	        len = clean2(&mf,lp,ll) ;
	    } /* end if_constexpr (f_clean1) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkcleanline) */


/* local subroutines */

static int clean1(MFLAGS *mfp,char *lp,int ll) noex {
	int		ili ;
	int		ch ;
	int		oli = 0 ;
	bool		f_flipped = false ;
	bool		f_rem = false ;
	bool		f ;
	for (ili = 0 ; ili < ll ; ili += 1) {
	    if (! f_rem) {
	        ch = mkchar(lp[ili]) ;
	        if ((ch == 0) && (! mfp->subnul)) break ;
	        if (! isshift(ch)) {
	            f = isprintlatin(ch) || ischarok(ch) ;
	            if (f) {
	                if (f_flipped) {
	                    lp[oli++] = lp[ili] ;
	                } else {
	                    oli += 1 ;
			}
	            } else if (mfp->subbad) {
	                lp[oli++] = char('­') ;
	            } else {
	                f_flipped = true ;
		    }
	        } else {
	            f_rem = true ;
		}
	    } else {
	        f_flipped = true ;
	        f_rem = false ;
	    }
	} /* end for */
	lp[oli] = '\0' ;
	return oli ;
}
/* end subroutine (clean1) */

static int clean2(MFLAGS *mfp,char *lp,int ll) noex {
	int		ili ;
	int		ch ;
	int		oli = 0 ;
	bool		f_flipped = false ;
	bool		f ;
	for (ili = 0 ; ili < ll ; ili += 1) {
	    ch = mkchar(lp[ili]) ;
	    if ((ch == 0) && (! mfp->subnul)) break ;
	    f = isprintlatin(ch) || ischarok(ch) ;
	    if (f) {
	        if (f_flipped) {
	            lp[oli++] = lp[ili] ;
	        } else {
	            oli += 1 ;
		}
	    } else if (mfp->subbad) {
	        lp[oli++] = char('­') ;
	    } else {
	        f_flipped = true ;
	    }
	} /* end for */
	lp[oli] = '\0' ;
	return oli ;
}
/* end subroutine (clean2) */

static int isshift(int ch) noex {
	int		f = false ;
	f = f || (ch == CH_SS2) ;
	f = f || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (isshift) */

static int ischarok(int ch) noex {
	return (ch == '\t') || (ch == '\n') ;
}
/* end subroutine (ischarok) */

static int isend(int ch) noex {
	return (ch == '\n') || (ch == '\r') ;
}
/* end subroutine (isend) */


